#include "MantidAlgorithms/ExportTimeSeriesLog.h"
#include "MantidKernel/System.h"
#include "MantidAPI/Axis.h"
#include "MantidAPI/FileProperty.h"
#include "MantidAPI/WorkspaceProperty.h"
#include "MantidAPI/IEventList.h"
#include "MantidAPI/WorkspaceFactory.h"
#include "MantidDataObjects/EventWorkspace.h"
#include "MantidDataObjects/EventList.h"
#include "MantidDataObjects/Events.h"
#include "MantidAPI/WorkspaceProperty.h"
#include "MantidKernel/UnitFactory.h"
#include "MantidGeometry/Instrument.h"
#include "MantidKernel/TimeSeriesProperty.h"
#include <algorithm>
#include <fstream>
#include "MantidKernel/ListValidator.h"

using namespace Mantid;
using namespace Mantid::Kernel;
using namespace Mantid::API;
using namespace Mantid::DataObjects;

using namespace std;

namespace Mantid {
namespace Algorithms {

DECLARE_ALGORITHM(ExportTimeSeriesLog)
//----------------------------------------------------------------------------------------------
/** Constructor
 */
ExportTimeSeriesLog::ExportTimeSeriesLog() {}

//----------------------------------------------------------------------------------------------
/** Destructor
 */
ExportTimeSeriesLog::~ExportTimeSeriesLog() {}

//----------------------------------------------------------------------------------------------
/** Definition of all input arguments
 */
void ExportTimeSeriesLog::init() {
  declareProperty(
      new API::WorkspaceProperty<MatrixWorkspace>("InputWorkspace", "Anonymous",
                                                  Direction::InOut),
      "Name of input Matrix workspace containing the log to export. ");

  declareProperty(
      new WorkspaceProperty<MatrixWorkspace>("OutputWorkspace", "Dummy",
                                             Direction::Output),
      "Name of the workspace containing the log events in Export. ");

  declareProperty("LogName", "", "Log's name to filter events.");

  std::vector<std::string> units{"Seconds", "Nano Seconds"};
  declareProperty(
      "UnitOfTime", "Seconds",
      boost::make_shared<Kernel::StringListValidator>(units),
      "StartTime, StopTime and DeltaTime can be given in various unit."
      "The unit can be 'Seconds' or 'Nanoseconds' from run start time."
      "They can also be defined as 'Percentage' of total run time.");

  declareProperty("StartTime", EMPTY_DBL(),
                  "Relative starting time of the output series. "
                  "Its unit is determined by property UnitOfTime.");

  declareProperty("StopTime", EMPTY_DBL(),
                  "Relative stopping time of the output series."
                  "Its unit is determined by property UnitOfTime.");

  declareProperty(
      "OutputAbsoluteTime", false,
      "If true, the output times will be absolute time to 1990.01.01.");

  declareProperty(
      "NumberEntriesExport", EMPTY_INT(),
      "Number of entries of the log to be exported.  Default is all entries.");

  declareProperty("IsEventWorkspace", true, "If set to true, output workspace "
                                            "is EventWorkspace.  Otherwise, it "
                                            "is Workspace2D.");

  return;
}

//----------------------------------------------------------------------------------------------
/** Main execution
 */
void ExportTimeSeriesLog::exec() {
  // Get properties
  m_inputWS = this->getProperty("InputWorkspace");
  string logname = getProperty("LogName");

  double start_time = getProperty("StartTime");
  double stop_time = getProperty("StopTime");
  std::string time_unit = getProperty("UnitOfTime");
  bool exportEpochTime = getProperty("OutputAbsoluteTime");

  int numberoutputentries = getProperty("NumberEntriesExport");
  bool outputeventworkspace = getProperty("IsEventWorkspace");

  // Call the main
  exportLog(logname, time_unit, start_time, stop_time, exportEpochTime,
            outputeventworkspace, numberoutputentries);

  // 3. Output
  setProperty("OutputWorkspace", m_outWS);

  return;
}
/*
 *  * @param logname ::
  * @param numentries ::
  * @param outputeventws ::
 * true.
 */
//----------------------------------------------------------------------------------------------
/** Export part of designated log to an file in column format and a output file
 * @brief ExportTimeSeriesLog::exportLog
 * @param logname ::  name of log to export
 * @param timeunit :: unit of time for input start/stop time and output
 * @param starttime :: relative start time of the output time series log
 * @param stoptime :: relative stop time of the output time series log
 * @param exportepoch :: flag to output time as epoch time/absolute time
 * @param outputeventws :: boolean.  output workspace is event workspace if
 * @param numentries :: number of log entries to export
 */
void ExportTimeSeriesLog::exportLog(const std::string &logname,
                                    const std::string timeunit,
                                    const double &starttime,
                                    const double &stoptime,
                                    const bool exportepoch, bool outputeventws,
                                    int numentries) {

  // Get log, time, and etc.
  std::vector<Kernel::DateAndTime> times;
  std::vector<double> values;

  if (logname.size() > 0) {
    // Log
    Kernel::TimeSeriesProperty<double> *tlog =
        dynamic_cast<Kernel::TimeSeriesProperty<double> *>(
            m_inputWS->run().getProperty(logname));
    if (!tlog) {
      std::stringstream errmsg;
      errmsg << "TimeSeriesProperty Log " << logname
             << " does not exist in workspace " << m_inputWS->getName();
      g_log.error(errmsg.str());
      throw std::invalid_argument(errmsg.str());
    }
    times = tlog->timesAsVector();
    values = tlog->valuesAsVector();
  } else {
    throw std::runtime_error("Log name cannot be left empty.");
  }

  // Get start time, stop time and unit factor
  double timeunitfactor = 1.;
  if (timeunit.compare("Seconds") == 0)
    timeunitfactor = 1.E-9;

  // Get index for start time
  size_t i_start = 0;
  size_t i_stop = times.size() - 1;
  // Rule out the case that start time is behind last log entry
  bool i_start_cal = false;
  if (starttime != EMPTY_DBL()) {
    int64_t timerangens =
        times.back().totalNanoseconds() - times.front().totalNanoseconds();
    double timerange = static_cast<double>(timerangens) * timeunitfactor;
    g_log.debug() << "Time range is " << timerange << ", Start time is "
                  << starttime << "\n";
    if (timerange < starttime) {
      i_start = times.size() - 1;
      i_start_cal = true;
    }
  }

  if ((!i_start_cal) && (starttime != EMPTY_DBL() || stoptime != EMPTY_DBL())) {
    bool export_partial = calculateTimeSeriesRangeByTime(
        times, starttime, i_start, stoptime, i_stop, timeunitfactor);
    if (!export_partial)
      throw std::runtime_error(
          "Unable to find proton_charge for run start time. "
          "Failed to get partial time series.");
  }

  // Determine number of export log
  if (numentries == EMPTY_INT()) {
    numentries = static_cast<int>(times.size());
  } else if (numentries <= 0) {
    stringstream errmsg;
    errmsg << "For Export Log, NumberEntriesExport must be greater than 0.  "
              "Input = " << numentries;
    g_log.error(errmsg.str());
    throw std::runtime_error(errmsg.str());
  } else if (static_cast<size_t>(numentries) > times.size()) {
    numentries = static_cast<int>(times.size());
  }

  // Create otuput workspace
  if (outputeventws) {
    setupEventWorkspace(i_start, i_stop, numentries, times, values,
                        exportepoch);
  } else {
    setupWorkspace2D(i_start, i_stop, numentries, times, values, exportepoch,
                     timeunitfactor);
  }

  return;
}

/** Set up the output workspace in a Workspace2D
 * @brief ExportTimeSeriesLog::setupWorkspace2D
 * @param start_index :: array index for the first log entry
 * @param stop_index :: array index for the last log entry
 * @param numentries :: number of log entries to output
 * @param times :: vector of Kernel::DateAndTime
 * @param values :: vector of log value in double
 * @param epochtime :: flag to output time in epoch time/absolute time
 * @param timeunitfactor :: conversion factor for various unit of time for
 * output
 */
void ExportTimeSeriesLog::setupWorkspace2D(
    const size_t &start_index, const size_t &stop_index, int numentries,
    vector<DateAndTime> &times, vector<double> values, const bool &epochtime,
    const double &timeunitfactor) {
  // Determine time shift
  int64_t timeshift(0);
  if (!epochtime) {
    // relative time
    Kernel::DateAndTime runstart(
        m_inputWS->run().getProperty("run_start")->value());
    timeshift = runstart.totalNanoseconds();
  }

  // Determine the size
  size_t outsize = stop_index - start_index + 1;
  if (outsize > static_cast<size_t>(numentries))
    outsize = static_cast<size_t>(numentries);

  // Create 2D workspace
  m_outWS = boost::dynamic_pointer_cast<MatrixWorkspace>(
      WorkspaceFactory::Instance().create("Workspace2D", 1, outsize, outsize));
  if (!m_outWS)
    throw runtime_error(
        "Unable to create a Workspace2D casted to MatrixWorkspace.");

  MantidVec &vecX = m_outWS->dataX(0);
  MantidVec &vecY = m_outWS->dataY(0);
  MantidVec &vecE = m_outWS->dataE(0);

  for (size_t index = 0; index < outsize; ++index) {
    size_t i_time = index + start_index;
    // safety check
    if (i_time >= times.size()) {
      std::stringstream errss;
      errss << "It shouldn't happen that the index is out of boundary."
            << "start index = " << start_index << ", output size = " << outsize
            << ", index = " << index << "\n";
      throw std::runtime_error(errss.str());
    }

    int64_t dtns = times[i_time].totalNanoseconds() - timeshift;
    vecX[index] = static_cast<double>(dtns) * timeunitfactor;
    vecY[index] = values[i_time];
    vecE[index] = 0.0;
  }

  Axis *xaxis = m_outWS->getAxis(0);
  xaxis->setUnit("Time");

  return;
}

//----------------------------------------------------------------------------------------------
/** Set up an Event workspace
 * @brief ExportTimeSeriesLog::setupEventWorkspace
 * @param start_index
 * @param stop_index
 * @param numentries :: number of log entries to output
 * @param times :: vector of Kernel::DateAndTime
 * @param values :: vector of log value in double
 * @param epochtime :: boolean flag for output time is absolute time/epoch time.
 */
void ExportTimeSeriesLog::setupEventWorkspace(
    const size_t &start_index, const size_t &stop_index, int numentries,
    vector<DateAndTime> &times, vector<double> values, const bool &epochtime) {
  Kernel::DateAndTime runstart(
      m_inputWS->run().getProperty("run_start")->value());

  // Get some stuff from the input workspace
  const size_t numberOfSpectra = 1;
  const int YLength = static_cast<int>(m_inputWS->blocksize());
  // determine output size
  size_t outsize = stop_index - start_index + 1;
  if (outsize > static_cast<size_t>(numentries))
    outsize = static_cast<size_t>(numentries);

  // Make a brand new EventWorkspace
  EventWorkspace_sptr outEventWS = boost::dynamic_pointer_cast<EventWorkspace>(
      API::WorkspaceFactory::Instance().create(
          "EventWorkspace", numberOfSpectra, YLength + 1, YLength));
  // Copy geometry over.
  API::WorkspaceFactory::Instance().initializeFromParent(m_inputWS, outEventWS,
                                                         false);

  m_outWS = boost::dynamic_pointer_cast<MatrixWorkspace>(outEventWS);
  if (!m_outWS)
    throw runtime_error(
        "Output workspace cannot be casted to a MatrixWorkspace.");

  // Create the output event list (empty)
  EventList &outEL = outEventWS->getOrAddEventList(0);
  outEL.switchTo(WEIGHTED_NOTIME);

  // Allocate all the required memory
  outEL.reserve(outsize);
  outEL.clearDetectorIDs();

  int64_t time_shift_ns(0);
  if (!epochtime) {
    // relative time
    time_shift_ns = runstart.totalNanoseconds();
  }

  for (size_t i = 0; i < outsize; i++) {
    Kernel::DateAndTime tnow = times[i + start_index];
    int64_t dt = tnow.totalNanoseconds() - time_shift_ns;

    // convert to microseconds
    double dtmsec = static_cast<double>(dt) / 1000.0;
    outEL.addEventQuickly(WeightedEventNoTime(dtmsec, values[i + start_index],
                                              values[i + start_index]));
  }
  // Ensure thread-safety
  outEventWS->sortAll(TOF_SORT, nullptr);

  // Now, create a default X-vector for histogramming, with just 2 bins.
  Kernel::cow_ptr<MantidVec> axis;
  MantidVec &xRef = axis.access();
  xRef.resize(2);
  std::vector<WeightedEventNoTime> &events = outEL.getWeightedEventsNoTime();
  xRef[0] = events.begin()->tof();
  xRef[1] = events.rbegin()->tof();

  // Set the binning axis using this.
  outEventWS->setX(0, axis);

  return;
}

/** Calculate the range of time vector by start time and stop time
 * @brief calculateTimeSeriesRangeByTime
 * @param vec_times :: vector for time
 * @param rel_start_time :: starting time relative to run start
 * @param i_start :: output of the index of of first log entry
 * @param rel_stop_time :: stopping time relative to run start
 * @param i_stop :: output of the index of the last log entry
 * @param time_factor :: factor of time unit. for example, nanosecond is 1,
 * second is 1E-9
 */
bool ExportTimeSeriesLog::calculateTimeSeriesRangeByTime(
    std::vector<Kernel::DateAndTime> &vec_times, const double &rel_start_time,
    size_t &i_start, const double &rel_stop_time, size_t &i_stop,
    const double &time_factor) {
  // Initialize if there is something wrong.
  i_start = 0;
  i_stop = vec_times.size() - 1;

  // Check existence of proton_charge as run start
  Kernel::DateAndTime run_start(0);
  if (m_inputWS->run().hasProperty("proton_charge"))
    run_start = dynamic_cast<TimeSeriesProperty<double> *>(
                    m_inputWS->run().getProperty("proton_charge"))->nthTime(0);
  else {
    g_log.warning("Property proton_charge does not exist so it is unable to "
                  "determine run start time. "
                  "StartTime and StopTime are ignored.  TimeSeriesProperty is "
                  "exported in full length.");
    return false;
  }

  // Get time 0
  if (rel_start_time != EMPTY_DBL()) {
    int64_t start_time_ns = run_start.totalNanoseconds() +
                            static_cast<int64_t>(rel_start_time / time_factor);
    Kernel::DateAndTime start_time(start_time_ns);
    i_start = static_cast<size_t>(
        std::lower_bound(vec_times.begin(), vec_times.end(), start_time) -
        vec_times.begin());

    // try to record the log value before starting time
    if (i_start >= 1)
      --i_start;
  }

  if (rel_stop_time != EMPTY_DBL()) {
    int64_t stop_time_ns = run_start.totalNanoseconds() +
                           static_cast<int64_t>(rel_stop_time / time_factor);
    Kernel::DateAndTime stop_time(stop_time_ns);
    i_stop = static_cast<size_t>(
        std::lower_bound(vec_times.begin(), vec_times.end(), stop_time) -
        vec_times.begin());
  }

  return true;
}

} // namespace Mantid
} // namespace Algorithms
