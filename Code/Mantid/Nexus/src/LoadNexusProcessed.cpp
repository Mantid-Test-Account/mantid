// LoadNexusProcessed
// @author Ronald Fowler, based on SaveNexus
//----------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------
#include "MantidNexus/LoadNexusProcessed.h"
#include "MantidDataObjects/Workspace1D.h"
#include "MantidDataObjects/Workspace2D.h"
#include "MantidKernel/UnitFactory.h"
#include "MantidKernel/ArrayProperty.h"
#include "MantidKernel/FileValidator.h"
#include "MantidKernel/ConfigService.h"
#include "MantidKernel/System.h"
#include "MantidAPI/Progress.h"
#include "MantidAPI/AlgorithmFactory.h"
#include "MantidNexus/NexusFileIO.h"
#include "MantidNexus/NexusClasses.h"

#include "Poco/Path.h"
#include <cmath>
#include <boost/shared_ptr.hpp>

namespace Mantid
{
namespace NeXus
{

// Register the algorithm into the algorithm factory
DECLARE_ALGORITHM(LoadNexusProcessed)

using namespace Kernel;
using namespace API;
using namespace DataObjects;

// Initialise logger
Logger& LoadNexusProcessed::g_log = Logger::get("LoadNexusProcessed");

/// Default constructor
LoadNexusProcessed::LoadNexusProcessed() :
  Algorithm(),                                    //call the constructor for the base class
  nexusFile(new NexusFileIO()), m_filename(), m_entrynumber(0), m_list(false), m_interval(false),
  m_spec_list(), m_spec_min(0), m_spec_max(unSetInt)
{}

/// Delete NexusFileIO in destructor
LoadNexusProcessed::~LoadNexusProcessed()
{
  delete nexusFile;
}

/** Initialisation method.
 *
 */
void LoadNexusProcessed::init()
{
  // Declare required input parameters for algorithm
  std::vector<std::string> exts;
  exts.push_back("NXS");
  exts.push_back("nxs");
  exts.push_back("nx5");
  exts.push_back("NX5");
  exts.push_back("xml");
  exts.push_back("XML");
  // required
  declareProperty("FileName", "", new FileValidator(exts),
    "Name of the Nexus file to read, as a full or relative path");
  declareProperty(new WorkspaceProperty<DataObjects::Workspace2D> ("OutputWorkspace", "",
      Direction::Output),
      "The name of the workspace to be created as the output of the\n"
      "algorithm. For multiperiod files, one workspace may be\n"
      "generated for each period. Currently only one workspace can\n"
      "be saved at a time so multiperiod Mantid files are not\n"
      "generated");
  // optional
  BoundedValidator<int> *mustBePositive = new BoundedValidator<int> ();
  mustBePositive->setLower(0);
  declareProperty("spectrum_min", 0, mustBePositive,
    "Index number of the first spectrum to read, only used if\n"
    "spectrum_max is set and only for single period data, not\n"
    " yet implemented (default 0)");
  declareProperty("spectrum_max", unSetInt, mustBePositive->clone(),
    "Index of last spectrum to read, only for single period data,\n"
    " not yet implemented (default the last spectrum).");
  declareProperty(new ArrayProperty<int> ("spectrum_list"),
    "Array, or comma separated list, of indexes of spectra to\n"
    "load. Not yet implemented.");
  declareProperty("EntryNumber", 0, mustBePositive->clone(),
    "Index number of the workspace to load");
}

/** Executes the algorithm. Reading in the file and creating and populating
 *  the output workspace
 *
 *  @throw runtime_error Thrown if algorithm cannot execute
 */
void LoadNexusProcessed::exec()
{
  // Retrieve the filename from the properties
  m_filename = getPropertyValue("FileName");
  // Need to extract the user-defined output workspace name
  Property *ws = getProperty("OutputWorkspace");
  std::string localWSName = ws->value();
  boost::shared_ptr<Sample> sample;
  //
  m_entrynumber = getProperty("EntryNumber");

  if (nexusFile->openNexusRead(m_filename, m_entrynumber) != 0)
  {
    g_log.error("Failed to read file " + m_filename);
    throw Exception::FileError("Failed to read to file", m_filename);
  }
  if (nexusFile->getWorkspaceSize(m_numberofspectra, m_numberofchannels, m_xpoints, m_uniformbounds,
      m_axes, m_yunits) != 0)
  {
    g_log.error("Failed to read data size");
    throw Exception::FileError("Failed to read data size", m_filename);
  }

  // validate the optional parameters, if set
  checkOptionalProperties();
  int total_specs;

  // Create the 2D workspace for the output
  if (m_interval || m_list)
  {
    total_specs = m_spec_list.size();
    if (m_interval)
    {
      total_specs += (m_spec_max - m_spec_min + 1);
      m_spec_max += 1;
    }
  }
  else
  {
    total_specs = m_numberofspectra;
    m_spec_min = 1;
    m_spec_max = m_numberofspectra + 1;
  }

  // create output workspace of required size
  DataObjects::Workspace2D_sptr localWorkspace = boost::dynamic_pointer_cast<DataObjects::Workspace2D>(
      WorkspaceFactory::Instance().create("Workspace2D", total_specs, m_xpoints, m_numberofchannels));
  // set first axis name
  size_t colon = m_axes.find(":");
  if (colon != std::string::npos)
  {
    try
    {
      localWorkspace->getAxis(0)->unit() = UnitFactory::Instance().create(m_axes.substr(0, colon));
    } catch (std::runtime_error&)
    { 
      g_log.warning("Unable to set Axis(0) units");
    }
  }
  // set Yunits
  if (m_yunits.size() > 0)
    localWorkspace->setYUnit(m_yunits);

  Histogram1D::RCtype xValues;
  xValues.access() = localWorkspace->dataX(0);
  if (m_uniformbounds)
    nexusFile->getXValues(xValues.access(), 0);
  int counter = 0;
  API::Progress progress(this,0.,1.,m_numberofspectra);
  for (int i = 1; i <= m_numberofspectra; ++i)
  {
    //int histToRead = i + period*(m_numberOfSpectra+1);
    if ((i >= m_spec_min && i < m_spec_max) || (m_list
        && find(m_spec_list.begin(), m_spec_list.end(), i) != m_spec_list.end()))
    {
      MantidVec& values = localWorkspace->dataY(counter);
      MantidVec& errors = localWorkspace->dataE(counter);
      nexusFile->getSpectra(values, errors, i);
      if (!m_uniformbounds)
      {
        nexusFile->getXValues(xValues.access(), i - 1);
      }
      localWorkspace->setX(counter,xValues);
      localWorkspace->getAxis(1)->spectraNo(counter) = i;
      //
      ++counter;
      progress.report();
    }
  }

  sample = localWorkspace->getSample();
  nexusFile->readNexusProcessedSample(sample);
  // Run the LoadIntsturment algorithm if name available
  m_instrumentName = nexusFile->readNexusInstrumentName();
  if ( ! m_instrumentName.empty() )
    runLoadInstrument(localWorkspace);
//  if (nexusFile->readNexusInstrumentXmlName(m_instrumentxml, m_instrumentdate, m_instrumentversion))
//  {
//    if (m_instrumentxml != "NoXmlFileFound" && m_instrumentxml != "NoNameAvailable")
//      runLoadInstrument(localWorkspace);
  else
    g_log.warning("No instrument file name found in the Nexus file");
//  }
  // get any spectraMap info
  boost::shared_ptr<IInstrument> localInstrument = localWorkspace->getInstrument();
  SpectraDetectorMap& spectraMap = localWorkspace->mutableSpectraMap();
  nexusFile->readNexusProcessedSpectraMap(spectraMap, m_spec_min, m_spec_max);
  nexusFile->readNexusParameterMap(localWorkspace);
  // Assign the result to the output workspace property
  std::string outputWorkspace = "OutputWorkspace";
  setProperty(outputWorkspace, localWorkspace);
  nexusFile->closeNexusFile();

  loadAlgorithmHistory(localWorkspace);

  return;
}

/// Validates the optional 'spectra to read' properties, if they have been set
void LoadNexusProcessed::checkOptionalProperties()
{
  //read in the settings passed to the algorithm
  m_spec_list = getProperty("spectrum_list");
  m_spec_max = getProperty("spectrum_max");
  //Are we using a list of spectra or all the spectra in a range?
  m_list = !m_spec_list.empty();
  m_interval = (m_spec_max != unSetInt);
  if ( m_spec_max == unSetInt ) m_spec_max = 0;
  
  // Check validity of spectra list property, if set
  if (m_list)
  {
    m_list = true;
    const int minlist = *min_element(m_spec_list.begin(), m_spec_list.end());
    const int maxlist = *max_element(m_spec_list.begin(), m_spec_list.end());
    if (maxlist > m_numberofspectra || minlist == 0)
    {
      g_log.error("Invalid list of spectra");
      throw std::invalid_argument("Inconsistent properties defined");
    }
  }

  // Check validity of spectra range, if set
  if (m_interval)
  {
    m_interval = true;
    m_spec_min = getProperty("spectrum_min");
    if (m_spec_max < m_spec_min || m_spec_max > m_numberofspectra)
    {
      g_log.error("Invalid Spectrum min/max properties");
      throw std::invalid_argument("Inconsistent properties defined");
    }
  }
}

/** Run the sub-algorithm LoadInstrument (as for LoadRaw)
 *  @param localWorkspace The workspace to insert the instrument into
 */
void LoadNexusProcessed::runLoadInstrument(DataObjects::Workspace2D_sptr localWorkspace)
{
  // Determine the search directory for XML instrument definition files (IDFs)
  std::string directoryName = Kernel::ConfigService::Instance().getString("instrumentDefinition.directory");
  if (directoryName.empty())
  {
    // This is the assumed deployment directory for IDFs, where we need to be relative to the
    // directory of the executable, not the current working directory.
    directoryName = Poco::Path(Mantid::Kernel::ConfigService::Instance().getBaseDir()).resolve(
        "../Instrument").toString();
  }

  // For Nexus Mantid processed, Instrument XML file name is read from nexus 
//  std::string instrumentID = m_instrumentxml;
  std::string instrumentID = m_instrumentName;
  // force ID to upper case
  std::transform(instrumentID.begin(), instrumentID.end(), instrumentID.begin(), toupper);
//  std::string fullPathIDF = directoryName + "/" + instrumentID;
  std::string fullPathIDF = directoryName + "/" + instrumentID + "_Definition.xml";

  IAlgorithm_sptr loadInst = createSubAlgorithm("LoadInstrument");
  loadInst->setPropertyValue("Filename", fullPathIDF);
  loadInst->setProperty<MatrixWorkspace_sptr>("Workspace", localWorkspace);

  // Now execute the sub-algorithm. Catch and log any error, but don't stop.
  try
  {
    loadInst->execute();
  } catch (std::runtime_error&)
  {
    g_log.information("Unable to successfully run LoadInstrument sub-algorithm");
  }

  // If loading instrument definition file fails, run LoadInstrumentFromNexus instead
  // This does not work at present as the example files do not hold the necessary data
  // but is a place holder. Hopefully the new version of Nexus Muon files should be more
  // complete.
  //if ( ! loadInst->isExecuted() )
  //{
  //runLoadInstrumentFromNexus(localWorkspace);
  //}
}

void LoadNexusProcessed::loadAlgorithmHistory(DataObjects::Workspace2D_sptr localWorkspace)
{
    NXRoot root(m_filename);
    std::ostringstream path;
    int entrynumber = m_entrynumber ? m_entrynumber : 1;
    path << "mantid_workspace_"<<entrynumber<<"/process";
    NXMainClass process = root.openNXClass<NXMainClass>(path.str());
    for(size_t i=0;i<process.groups().size();i++)
    {
        NXClassInfo inf = process.groups()[i];
        if (inf.nxname.substr(0,16) != "MantidAlgorithm_") continue;
        NXNote history = process.openNXNote(inf.nxname);
        std::vector< std::string >& hst = history.data();
        if (hst.size() == 0) continue;
        try
        {
            std::istringstream ianame(hst[0]);
            std::string name,vers,dummy;
            ianame >> dummy >> name >> vers;
            int version = atoi( vers.substr(1).c_str() );
            boost::shared_ptr<API::Algorithm> alg = 
                boost::dynamic_pointer_cast<API::Algorithm>(API::AlgorithmFactory::Instance().create(name,version));
            alg->initialize();
            for(size_t i=4;i<hst.size();i++)
            {
                std::string str = hst[i];
                std::string name,value,deflt,direction;
                size_t i0 = str.find("Name:");
                size_t i1 = str.find(", Value:",i0+1);
                size_t i2 = str.find(", Default?:",i1+1);
                size_t i3 = str.find(", Direction",i2+1);
                name = str.substr(i0+6,i1-i0-6);
                value = str.substr(i1+9,i2-i1-9);
                deflt = str.substr(i2+12,i3-i2-12);
                direction = str.substr(i3+13);
                if (deflt == "No")
                {
                    alg->setPropertyValue(name,value);
                }
            }
            API::AlgorithmHistory ahist(alg.get());
            time_t tim = createTime_t_FromString(hst[1].substr(16,21));
            size_t ii = hst[2].find("sec");
            double dur = atof(hst[2].substr(20,ii-21).c_str());
            ahist.addExecutionInfo(tim,dur);
            localWorkspace->history().addAlgorithmHistory(ahist);

        }
        catch(...)
        {
            g_log.warning("Cannot load algorithm history: cannot create algorithm");
            continue;
        }
    }
}

std::time_t LoadNexusProcessed::createTime_t_FromString(const std::string &str)
{

    std::map<std::string,int> Month;
    Month["Jan"] = 1;
    Month["Feb"] = 2;
    Month["Mar"] = 3;
    Month["Apr"] = 4;
    Month["May"] = 5;
    Month["Jun"] = 6;
    Month["Jul"] = 7;
    Month["Aug"] = 8;
    Month["Sep"] = 9;
    Month["Oct"] = 10;
    Month["Nov"] = 11;
    Month["Dec"] = 12;

    std::tm time_since_1900;
    time_since_1900.tm_isdst = -1;

    // create tm struct
    time_since_1900.tm_year = atoi(str.substr(0,4).c_str()) - 1900;
    std::string month = str.substr(5,3);

    time_since_1900.tm_mon = Month[str.substr(5,3)];
    time_since_1900.tm_mday = atoi(str.substr(9,2).c_str());
    time_since_1900.tm_hour = atoi(str.substr(12,2).c_str());
    time_since_1900.tm_min = atoi(str.substr(15,2).c_str());
    time_since_1900.tm_sec = atoi(str.substr(18,2).c_str());

    return std::mktime(&time_since_1900);
}

} // namespace NeXus
} // namespace Mantid
