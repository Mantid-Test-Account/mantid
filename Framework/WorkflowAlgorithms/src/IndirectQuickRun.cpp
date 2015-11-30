#include "MantidWorkflowAlgorithms/IndirectQuickRun.h"

#include "MantidAPI/AlgorithmManager.h"

#include "MantidKernel/MandatoryValidator.h"
#include "MantidKernel/ListValidator.h"

namespace Mantid {
namespace Algorithms {

using namespace API;
using namespace Kernel;

// Register the algorithm into the AlgorithmFactory
DECLARE_ALGORITHM(IndirectQuickRun)

//----------------------------------------------------------------------------------------------
/** Constructor
 */
IndirectQuickRun::IndirectQuickRun() {}

//----------------------------------------------------------------------------------------------
/** Destructor
 */
IndirectQuickRun::~IndirectQuickRun() {}

//----------------------------------------------------------------------------------------------

/// Algorithms name for identification. @see Algorithm::name
const std::string IndirectQuickRun::name() const { return "IndirectQuickRun"; }

/// Algorithm's version for identification. @see Algorithm::version
int IndirectQuickRun::version() const { return 1; }

/// Algorithm's category for identification. @see Algorithm::category
const std::string IndirectQuickRun::category() const {
  return "Workflow\\MIDAS";
}

/// Algorithm's summary for use in the GUI and help. @see Algorithm::summary
const std::string IndirectQuickRun::summary() const {
  return "Reduces input data if required then runs Elwin and optionally I(Q, t)";
}

//----------------------------------------------------------------------------------------------
/** Initialize the algorithm's properties.
 */
void IndirectQuickRun::init() {
  declareProperty(
      "InputFiles", "", boost::make_shared<MandatoryValidator<std::string>>(),
      "A comma seperated list or range of input files", Direction::Input);

  // Options for the different instruments that can be used
  auto instrumentOptions = std::vector<std::string>();
  instrumentOptions.push_back("Iris");
  instrumentOptions.push_back("Osiris");

  declareProperty("Instrument", "Iris",
                  boost::make_shared<StringListValidator>(instrumentOptions),
                  "The instrument you which to run QuickRun for.",
                  Direction::Input);

  // Options for Instrument analyser that can be used
  auto analyserOptions = std::vector<std::string>();
  analyserOptions.push_back("graphite");
  analyserOptions.push_back("mica");
  analyserOptions.push_back("fmica");

  declareProperty("Analyser", "graphite",
                  boost::make_shared<StringListValidator>(analyserOptions),
                  "The Analyser for the input files. mica and fmica "
                  "should only be used for Iris.",
                  Direction::Input);

  // Options for the reflection of the instrument
  auto reflectionOptions = std::vector<std::string>();
  reflectionOptions.push_back("002");
  reflectionOptions.push_back("004");

  declareProperty("Reflection", "002",
                  boost::make_shared<StringListValidator>(reflectionOptions),
                  "The Reflection for the input files ", Direction::Input);

  declareProperty(
      "Iqt", true,
      "If ture, quick run will also run the data through the I(Q, t) algorithm",
      Direction::Input);

  declareProperty(
      new WorkspaceProperty<MatrixWorkspace>(
          "ResolutionWorkspace", "", Direction::Input, PropertyMode::Optional),
      "The resolution workspace to be used in the I(Q, t) algorithm");

  declareProperty(
      new WorkspaceProperty<>("OutputWorkspace", "", Direction::Output),
      "An output workspace.");
}

//----------------------------------------------------------------------------------------------
/** Execute the algorithm.
 */
void IndirectQuickRun::exec() {
  //------------------------------------GetProperties-------------------------------------------
  const std::string inputFiles = getProperty("InputFiles");
  const std::string instrument = getProperty("Instrument");
  const std::string analyser = getProperty("Analyser");
  const std::string reflection = getProperty("Reflection");
  const bool usingIqt = getProperty("Iqt");
  MatrixWorkspace_sptr resolutionWs = getProperty("ResolutionWorkspace");

  //------------------------------------ValidateInputs------------------------------------------
  if (usingIqt) {
    // Check resolution workspace is given (not null)
  }

  //--------------------------------------ReduceData--------------------------------------------

  //----------------------------------------Elwin-----------------------------------------------

  //----------------------------------------I(Q,t)----------------------------------------------
  if (usingIqt) {
  }
}

} // namespace WorkflowAlgorithms
} // namespace Mantid
