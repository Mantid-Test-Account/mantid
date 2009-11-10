#include "MantidAlgorithms/GetEi.h"
#include "MantidKernel/ArrayProperty.h"
#include "MantidKernel/FileProperty.h"
#include "MantidKernel/PhysicalConstants.h"
#include "MantidAPI/WorkspaceValidators.h"
#include "MantidAPI/SpectraDetectorMap.h"
#include <boost/lexical_cast.hpp>
#include "MantidKernel/Exception.h" 
#include <cmath>

namespace Mantid
{
namespace Algorithms
{

// Register the algorithm into the algorithm factory
DECLARE_ALGORITHM(GetEi)

using namespace Kernel;
using namespace API;
using namespace Geometry;
using namespace DataObjects;

// from the estimated location of the peak search forward by the following fraction and backward by the same fraction
const double GetEi::HALF_WINDOW = 4.0/100;
const double GetEi::PEAK_THRESH_H = 3.0;
const int GetEi::PEAK_THRESH_W = 3;

// progress estimates
const double GetEi::CROP = 0.15;
const double GetEi::GET_COUNT_RATE = 0.15;
const double GetEi::FIT_PEAK = 2.0;

/// Empty default constructor algorith() calls the constructor in the base class
GetEi::GetEi() : Algorithm(),
  m_fracCompl(0.0), m_tempWS()
{
}

void GetEi::init()
{
  // Declare required input parameters for algorithm
  WorkspaceUnitValidator<Workspace2D> *val =
    new WorkspaceUnitValidator<Workspace2D>("TOF");
  declareProperty(new WorkspaceProperty<Workspace2D>(
    "InputWorkspace","",Direction::Input,val),
    "The X units of this workspace must be time of flight with times in\n"
    "micro-seconds");
  BoundedValidator<int> *mustBePositive = new BoundedValidator<int>();
  mustBePositive->setLower(0);
  declareProperty("Monitor1ID", -1, mustBePositive,
    "The detector ID of the first monitor");
  declareProperty("Monitor2ID", -1, mustBePositive->clone(),
    "The detector ID of the second monitor");
  BoundedValidator<double> *positiveDouble = new BoundedValidator<double>();
  positiveDouble->setLower(0);
  declareProperty("EnergyEstimate", -1.0, positiveDouble,
    "An approximate value for the typical incident energy, energy of\n"
    "neutrons leaving the source (meV)");
  declareProperty("IncidentEnergy", -1.0, Direction::Output);

  m_fracCompl = 0.0;
}

/** Executes the algorithm
*  @throw out_of_range if the peak runs off the edge of the histogram
*  @throw NotFoundError if no detector is found for the detector ID given
*  @throw IndexError if there is a problem converting spectra indexes to spectra numbers, which would imply there is a problem with the workspace
*  @throw invalid_argument if a good peak fit wasn't made or the input workspace does not have common binning
*  @throw runtime_error a sub-algorithm falls over
*/
void GetEi::exec()
{
  Workspace2D_const_sptr WS = getProperty("InputWorkspace");
  std::vector<int> monIDs;
  monIDs.push_back(getProperty("Monitor1ID"));
  monIDs.push_back(getProperty("Monitor2ID"));
  double dist2moni0 = -1, dist2moni1 = -1;
  getGeometry(WS->getInstrument(), monIDs[0], monIDs[1],dist2moni0,dist2moni1);

  // the E_i estimate is used to find (identify) the monitor peaks, checking prior to fitting will throw an exception if this estimate is too big or small
  const double E_est = getProperty("EnergyEstimate");
  // we're assuming that the time units for the X-values in the workspace are micro-seconds
  const double peakLoc0 = 1e6*timeToFly(dist2moni0, E_est);
  // write a lot of stuff to the log at user level as it is very possible for fit routines not to the expected thing
  g_log.information() << "Based on the user selected energy the first peak will be searched for at TOF " << peakLoc0 << " micro seconds +/-" << boost::lexical_cast<std::string>(100.0*HALF_WINDOW) << "%" << std::endl;
  const double peakLoc1 = 1e6*timeToFly(dist2moni1, E_est);
  g_log.information() << "Based on the user selected energy the second peak will be searched for at TOF " << peakLoc1 << " micro seconds +/-" << boost::lexical_cast<std::string>(100.0*HALF_WINDOW) << "%" << std::endl;

  // get the histograms created by the monitors
  std::vector<int> indexes = getMonitorSpecIndexs(WS, monIDs);
  g_log.information() << "Looking for a peak in the first monitor spectrum, spectra index " << indexes[0] << std::endl;
  double t_monitor0 = getPeakCentre(WS, indexes[0], peakLoc0);
  g_log.information() << "The first peak has been found at TOF = " << t_monitor0 << std::endl;
  g_log.information() << "Looking for a peak in the second monitor spectrum, spectra index " << indexes[1] << std::endl;
  double t_monitor1 = getPeakCentre(WS, indexes[1], peakLoc1);
  g_log.information() << "The second peak has been found at TOF = " << t_monitor1 << std::endl;

  // assumes that the source and the both mintors lie on one straight line, the 1e-6 converts microseconds to seconds as the mean speed needs to be in m/s
  double meanSpeed = (dist2moni1 - dist2moni0)/(1e-6*(t_monitor1 - t_monitor0));

  // uses 0.5mv^2 to get the kinetic energy in joules which we then convert to meV
  double E_i = neutron_E_At(meanSpeed)/PhysicalConstants::meV;
  g_log.notice() << "The incident energy is estimated to be " << E_i << " meV" << " (your estimated value was " << E_est << " meV)" << std::endl;

  setProperty("IncidentEnergy", E_i);
}
/** Gets the distances between the source and detectors whose IDs you pass to it
*  @param geometry the instrument from a workspace
*  @param det0ID ID number of a detector
*  @param det1ID ID number of a detector
*  @param monitor0Dist the calculated distance to the detector whose ID was passed to this function first
*  @param monitor1Dist calculated distance to the detector whose ID was passed to this function second
*  @throw NotFoundError if no detector is found for the detector ID given
*/
void GetEi::getGeometry(IInstrument_const_sptr geometry, int det0ID, int det1ID, double &monitor0Dist, double &monitor1Dist) const
{
  const IObjComponent_sptr source = geometry->getSource();

  // retrieve a pointer to the first detector and get its distance
  Geometry::IDetector_sptr det = geometry->getDetector(det0ID);
  const IComponent * const first = det->getComponent();
  monitor0Dist = source->getDistance(*first);

  // repeat above for the second detector
  det = geometry->getDetector(det1ID);
  const IComponent * const second = det->getComponent();
  monitor1Dist = source->getDistance(*second);
}
/** Converts detector IDs to spectra indexes
*  @param WS the workspace on which the calculations are being performed
*  @param detIDs the detector IDs to search for
*  @return the indexes of the histograms created by the detector whose ID were passed
*  @throw IndexError if there is a problem converting spectra indexes to spectra numbers, which would imply there is a problem with the workspace
*/
std::vector<int> GetEi::getMonitorSpecIndexs(Workspace2D_const_sptr WS, const std::vector<int> &detIDs) const
{// getting spectra numbers from detector IDs is hard because the map works the other way, getting index numbers from spectra numbers has the same problem and we are about to do both
  std::vector<int> specInds;
  // this function creates a new multimap for detectorIDs to index numbers
  std::vector<int> spectraList = WS->spectraMap().getSpectra(detIDs);
  // this creates a new map from spectra numbers to indices
  WorkspaceHelpers::getIndicesFromSpectra(WS, spectraList, specInds);
  return specInds;
}
/** Uses E_KE = mv^2/2 and s = vt to calculate the time required for a neutron
*  to travel a distance, s
* @param s ditance travelled in meters
* @param E_KE kinetic energy in meV
* @return the time to taken to travel that uninterrupted distance in seconds
*/
double GetEi::timeToFly(double s, double E_KE) const
{
  // E_KE = mv^2/2, s = vt
  // t = s/v, v = sqrt(2*E_KE/m)
  // t = s/sqrt(2*E_KE/m)

  // convert E_KE to joules kg m^2 t^-2
  E_KE *= PhysicalConstants::meV;

  return s/sqrt(2*E_KE/PhysicalConstants::NeutronMass);
}

/** Looks for and examines a peak close to that specified by the input parameters and
*  examines it to find a representative time for when the neutrons hit the detector
*  @param WS the workspace containing the monitor spectrum
*  @param monitIn the index of the histogram that contains the monitor spectrum
*  @param peakTime the estimated TOF of the monitor peak in the time units of the workspace
*  @return a time half way between the two half height locations on the peak
*  @throw invalid_argument if a good peak fit wasn't made or the input workspace does not have common binning
*  @throw out_of_range if the peak runs off the edge of the histogram
*  @throw runtime_error a sub-algorithm just falls over
*/
double GetEi::getPeakCentre(Workspace2D_const_sptr WS, const int monitIn, const double peakTime)
{
  // we search for the peak only inside some window because there are often more peaks in the monitor histogram
  double halfWin = WS->readX(monitIn).back()*HALF_WINDOW;
  // runs CropWorkspace as a sub-algorithm to and puts the result in a new temporary workspace that will be deleted when this algorithm has finished
  extractSpec(monitIn, peakTime-halfWin, peakTime+halfWin);
  // converting the workspace to count rate is required by the fitting algorithm if the bin widths are not all the same
  WorkspaceHelpers::makeDistribution(m_tempWS);
  // look out for user cancel messgages as the above command can take a bit of time
  advanceProgress(GET_COUNT_RATE);

  // to store fit results
  double height, centreGaussian;
  getPeakEstimates(height, centreGaussian);
  // look out for user cancel messgages
  advanceProgress(FIT_PEAK);

  // find the index of the centre point. The centre can't be at index zero as this is at the edge of the spectrum, so centreIndex = 0 is the error value
  MantidVec::size_type cenGausIn = 0;
  for ( MantidVec::size_type i = 0; i < m_tempWS->readX(0).size(); ++i )
  {// assumes that the bin boundaries are all in order of increasing time
    if ( m_tempWS->readX(0)[i] > centreGaussian )
    {
      cenGausIn = i;
      break;
    }
  }
  // the peak centre is defined as the centre of the two half maximum points as this is better for asymmetric peaks
  // first loop backwards along the histogram to get the first half height point
  MantidVec::size_type lHalf = findHalfLoc(cenGausIn, height, GO_LEFT);
  // go forewards to get the half height on the otherside of the peak
  MantidVec::size_type rHalf = findHalfLoc(cenGausIn, height, GO_RIGHT);
  
  // the centre is the mean of the two end values
  double centreInd = static_cast<double>(lHalf + rHalf)/2.0;
  // convert the index into a time of flight value
  double tCalcu = m_tempWS->readX(0)[static_cast<int>(floor(centreInd))];
  // centreInd could be an integer or a half integer so return the value of the bin boundry or the mean of two bin boundaries
  tCalcu += m_tempWS->readX(0)[static_cast<int>(ceil(centreInd))];
  return tCalcu/2;
}
/** Calls CropWorkspace as a sub-algorithm and passes to it the InputWorkspace property
*  @param specInd the index number of the histogram to extract
*  @param start the number of the first bin to include (starts counting bins at 0)
*  @param end the number of the last bin to include (starts counting bins at 0)
*  @throw out_of_range if start, end or specInd are set outside of the vaild range for the workspace
*  @throw runtime_error if the algorithm just falls over
*  @throw invalid_argument if the input workspace does not have common binning
*/
void GetEi::extractSpec(int specInd, double start, double end)
{
  IAlgorithm_sptr childAlg =
    createSubAlgorithm("CropWorkspace", 100*m_fracCompl, 100*m_fracCompl+CROP );
  m_fracCompl += CROP;
  
  childAlg->setPropertyValue( "InputWorkspace",
                              getPropertyValue("InputWorkspace") );
  childAlg->setProperty( "XMin", start);
  childAlg->setProperty( "XMax", end);
  childAlg->setProperty( "StartWorkspaceIndex", specInd);
  childAlg->setProperty( "EndWorkspaceIndex", specInd);

  try
  {
    childAlg->execute();
  }
  catch (std::exception&)
  {
    g_log.error("Exception thrown while running CropWorkspace as a sub-algorithm");
    throw;
  }

  if ( ! childAlg->isExecuted() )
  {
    g_log.error("The CropWorkspace algorithm failed unexpectedly, aborting.");
    throw std::runtime_error(name() + " failed trying to run CropWorkspace");
  }
  m_tempWS = childAlg->getProperty("OutputWorkspace");

//DEBUGGING CODE uncomment out the line below if you want to see the TOF window that was analysed
//AnalysisDataService::Instance().addOrReplace("croped_dist_del", m_tempWS);
}

/** Finds the largest peak by looping through the histogram and finding the maximum
*  value 
* @param height this will became the peak height found by the fit
* @param centre will be set to the location of the peak center
* @throw invalid_argument if the peak is not much above the background
*/
void GetEi::getPeakEstimates(double &height, double &centre) const
{
  // take note of the number of background counts as error checking, do we have a peak or just a bump in the background
  double backgroundCounts = 0;
  // start at the first Y value
  height = m_tempWS->readY(0)[0];
  centre = m_tempWS->readX(0)[0];
  // then loop through all the Y values and find the tallest peak
  for ( MantidVec::size_type i = 1; i < m_tempWS->readY(0).size(); ++i )
  {
    backgroundCounts += m_tempWS->readY(0)[i];
    if ( m_tempWS->readY(0)[i] > height )
    {
      height = m_tempWS->readY(0)[i];
      centre = m_tempWS->readX(0)[i];
    }
  }
  if ( height < PEAK_THRESH_H*backgroundCounts/m_tempWS->readY(0).size() )
  {
    throw std::invalid_argument("No peak was found or its height is less than the threshold of " + boost::lexical_cast<std::string>(PEAK_THRESH_H) + " times the mean background");
  }
  g_log.debug() << "Initial guess of peak position, based on the maximum Y value in the monitor spectrum, is at TOF " << centre << " (peak height " << height << " counts/microsecond)" << std::endl;
}
/** Gets the index of the bin that is closest to the bin given and contains a number of
*  counts less half of the number passed to this function, bin indexes start at zero
*  @param startInd index of the bin to search around, e.g. the index of the peak centre
*  @param height the number of counts (or count rate) to compare against e.g. a peak height
*  @param go either GetEi::GO_LEFT or GetEi::GO_RIGHT
*  @return the index number of the first bin found where the counts are less than half
*  @throw out_of_range if the end of the histogram is reached before the point is found
*  @throw invalid_argument if the peak is too thin
*/
int GetEi::findHalfLoc(MantidVec::size_type startInd, double height, direction go) const
{
  MantidVec::size_type endInd = startInd;
  while ( m_tempWS->readY(0)[endInd] >  height/2.0 )
  {
    endInd += go;
    if ( endInd < 0 )
    {
      throw std::out_of_range("Can't analyse peak, some of the peak is outside the " + boost::lexical_cast<std::string>(HALF_WINDOW*100) + "% window, at TOF values that are too low");
    }
    if ( endInd >= m_tempWS->readY(0).size())
    {
      throw std::out_of_range("Can't analyse peak, some of the peak is outside the " + boost::lexical_cast<std::string>(HALF_WINDOW*100) + "% window, at TOF values that are too high");
    }
  }
  if ( std::abs(static_cast<int>(endInd - startInd)) < PEAK_THRESH_W )
  {// we didn't find a insignificant peak
    throw std::invalid_argument("No peak was found or its width is less than the threshold 2x" + boost::lexical_cast<std::string>(PEAK_THRESH_W) + " bins");
  }
  g_log.debug() << "One half height point found at TOF = " << m_tempWS->readX(0)[endInd] << " microseconds" << std::endl;
  return static_cast<int>(endInd);
}

/** Get the kinetic energy of a neuton in joules given it speed using E=mv^2/2
*  @param speed the instantanious speed of a neutron in metres per second
*  @return the energy in joules
*/
double GetEi::neutron_E_At(double speed) const
{
  // E_KE = mv^2/2
  return PhysicalConstants::NeutronMass*speed*speed/(2);
}

/// Update the percentage complete estimate assuming that the algorithm has completed a task with estimated RunTime toAdd
void GetEi::advanceProgress(double toAdd)
{
  m_fracCompl += toAdd;
  progress(m_fracCompl);
  // look out for user cancel messgages
  interruption_point();
}

} // namespace Algorithms
} // namespace Mantid
