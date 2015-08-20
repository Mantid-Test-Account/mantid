#include "MantidAlgorithms/ConvolutionFitSequential.h"

#include "MantidAPI/AlgorithmManager.h"
#include "MantidAPI/FunctionDomain1D.h"
#include "MantidAPI/FunctionFactory.h"
#include "MantidAPI/IFunction.h"
#include "MantidAPI/ITableWorkspace.h"
#include "MantidAPI/WorkspaceGroup.h"

#include "MantidKernel/MandatoryValidator.h"
#include "MantidKernel/BoundedValidator.h"
#include "MantidKernel/ListValidator.h"
#include "MantidKernel/StringContainsValidator.h"

#include <cmath>

namespace Mantid {
namespace Algorithms {

using namespace API;
using namespace Kernel;

// Register the algorithm into the AlgorithmFactory
DECLARE_ALGORITHM(ConvolutionFitSequential)

//----------------------------------------------------------------------------------------------
/** Constructor
 */
ConvolutionFitSequential::ConvolutionFitSequential() {}

//----------------------------------------------------------------------------------------------
/** Destructor
 */
ConvolutionFitSequential::~ConvolutionFitSequential() {}

//----------------------------------------------------------------------------------------------

/// Algorithms name for identification. @see Algorithm::name
const std::string ConvolutionFitSequential::name() const {
  return "ConvolutionFitSequential";
}

/// Algorithm's version for identification. @see Algorithm::version
int ConvolutionFitSequential::version() const { return 1; }

/// Algorithm's category for identification. @see Algorithm::category
const std::string ConvolutionFitSequential::category() const {
  return "Workflow\\MIDAS";
}

/// Algorithm's summary for use in the GUI and help. @see Algorithm::summary
const std::string ConvolutionFitSequential::summary() const {
  return "Performs a sequential fit for a convolution workspace";
}

//----------------------------------------------------------------------------------------------
/** Initialize the algorithm's properties.
 */
void ConvolutionFitSequential::init() {
  declareProperty(
      new WorkspaceProperty<>("InputWorkspace", "", Direction::Input),
      "The input workspace for the fit.");

  auto scv = boost::make_shared<StringContainsValidator>();
  auto requires = std::vector<std::string>();
  requires.push_back("Convolution");
  requires.push_back("Resolution");
  scv->setRequiredStrings(requires);

  declareProperty("Function", "", scv,
                  "The function that describes the parameters of the fit.",
                  Direction::Input);

  declareProperty(
      "Start X", EMPTY_DBL(), boost::make_shared<MandatoryValidator<double>>(),
      "The start of the range for the fit function.", Direction::Input);

  declareProperty(
      "End X", EMPTY_DBL(), boost::make_shared<MandatoryValidator<double>>(),
      "The end of the range for the fit function.", Direction::Input);

  declareProperty("Temperature", EMPTY_DBL(),
                  boost::make_shared<MandatoryValidator<double>>(),
                  "The Temperature correction for the fit. If no there is no "
                  "temperature correction, use 0.0",
                  Direction::Input);

  auto boundedV = boost::make_shared<BoundedValidator<int>>();
  boundedV->setLower(0);

  declareProperty("Spec Min", 0, boundedV, "The first spectrum to be used in "
                                           "the fit. Spectra values can not be "
                                           "negative",
                  Direction::Input);

  declareProperty("Spec Max", 0, boundedV, "The final spectrum to be used in "
                                           "the fit. Spectra values can not be "
                                           "negative",
                  Direction::Input);

  declareProperty("Convolve", true,
                  "If true, the fit is treated as a convolution workspace.",
                  Direction::Input);

  std::vector<std::string> minimizers;
  minimizers.push_back("Levenberg-Marquardt");
  minimizers.push_back("Simplex");
  minimizers.push_back("FABADA");
  minimizers.push_back("Conjugate gradient (Fletcher-Reeves imp.)");
  minimizers.push_back("Conjugate gradient (Polak-Ribiere imp.)");
  minimizers.push_back("BFGS");
  declareProperty("Minimizer", std::string("Levenberg-Marquardt"),
                  boost::make_shared<StringListValidator>(minimizers),
                  "Minimizer to use for fitting. Minimizers available are: "
                  "'Levenberg-Marquardt', 'Simplex', 'FABADA', 'Conjugate "
                  "gradient (Fletcher-Reeves imp.)', 'Conjugate gradient "
                  "(Polak-Ribiere imp.)' and 'BFGS'",
                  Direction::Input);

  declareProperty("Max Iterations", 500, boundedV,
                  "The maximum number of iterations permitted",
                  Direction::Input);
}

//----------------------------------------------------------------------------------------------
/** Execute the algorithm.
 */
void ConvolutionFitSequential::exec() {
  // Initialise variables with properties
  MatrixWorkspace_sptr inputWs = getProperty("InputWorkspace");
  const std::string function = getProperty("Function");
  const double startX = getProperty("Start X");
  const double endX = getProperty("End X");
  const double temperature = getProperty("Temperature");
  const int specMin = getProperty("Spec Min");
  const int specMax = getProperty("Spec max");
  const bool convolve = getProperty("Convolve");
  const int maxIter = getProperty("Max Iterations");
  const std::string minimizer = getProperty("Minimizer");

  // Handle empty/non-empty temp property
  bool usingTemp = false;
  if (temperature != 0.0) {
    usingTemp = true;
  }

  // Inspect function to obtain fit Type and background
  std::vector<std::string> functionValues = findValuesFromFunction(function);
  const std::string LorentzNum = functionValues[0];
  const std::string backgroundName = functionValues[1];
  const std::string funcName = functionValues[2];
  bool usingLorentzians = false;
  if (boost::lexical_cast<int>(LorentzNum) > 0) {
    usingLorentzians = true;
  }

  // Check if a delta function is being used
  bool delta = false;
  std::string usingDelta = "false";
  auto pos = function.find("Delta");
  if (pos != std::string::npos) {
    delta = true;
    usingDelta = "true";
  }

  // Add logger information
  m_log.information("Input files: " + inputWs->getName());
  m_log.information("Fit type: Delta=" + usingDelta + "; Lorentzians=" +
                    LorentzNum);
  m_log.information("Background type: " + backgroundName);

  // Output workspace name
  std::string outputWsName = inputWs->getName();
  pos = outputWsName.rfind("_");
  if (pos != std::string::npos) {
    outputWsName = outputWsName.substr(0, pos + 1);
  }
  outputWsName += "conv_";
  if (delta) {
    outputWsName += "Delta";
  }
  if (LorentzNum.compare("0") != 0) {
    outputWsName += LorentzNum + "L";
  }
  outputWsName += backgroundName + "_s";
  outputWsName += boost::lexical_cast<std::string>(specMin);
  outputWsName += "_to_";
  outputWsName += boost::lexical_cast<std::string>(specMax);

  // Convert input workspace to get Q axis
  const std::string tempFitWsName = "__convfit_fit_ws";
  auto tempFitWs = convertInputToElasticQ(inputWs, tempFitWsName);

  // Fit all spectra in workspace
  std::string plotPeakInput = "";
  for (int i = 0; i < specMax + 1; i++) {
    std::string nextWs = tempFitWsName + ",i";
    nextWs += std::to_string(i);
    plotPeakInput += nextWs + ";";
  }

  // passWSIndex
  bool passIndex = false;
  if (funcName.find("Diffusion") != std::string::npos ||
      funcName.find("Stretched") != std::string::npos) {
    passIndex = true;
  }

  // Run PlotPeaksByLogValue
  auto plotPeaks = createChildAlgorithm("PlotPeakByLogValue", -1, -1, true);
  plotPeaks->setAlwaysStoreInADS(true);
  plotPeaks->setProperty("Input", plotPeakInput);
  plotPeaks->setProperty("OutputWorkspace", outputWsName);
  plotPeaks->setProperty("Function", function);
  plotPeaks->setProperty("StartX", startX);
  plotPeaks->setProperty("EndX", endX);
  plotPeaks->setProperty("FitType", "Sequential");
  plotPeaks->setProperty("CreateOutput", true);
  plotPeaks->setProperty("OutputCompositeMembers", true);
  plotPeaks->setProperty("ConvolveMembers", convolve);
  plotPeaks->setProperty("MaxIterations", maxIter);
  plotPeaks->setProperty("Minimizer", minimizer);
  plotPeaks->setProperty("PassWSIndexToFunction", passIndex);
  plotPeaks->executeAsChildAlg();

  ITableWorkspace_sptr outputWs =
      AnalysisDataService::Instance().retrieveWS<ITableWorkspace>(outputWsName);

  // Delete workspaces
  std::string deleteWorkspaces[] = {
      (outputWsName + "_NormalisedCovarianceMatrices"),
      (outputWsName + "_Parameters"), tempFitWsName};
  auto deleter = createChildAlgorithm("DeleteWorkspace", -1, -1, true);
  for (int i = 0; i < 3; i++) {
    deleter->setProperty("WorkSpace", deleteWorkspaces[i]);
    deleter->executeAsChildAlg();
  }

  // Construct output workspace
  std::string resultWsName = outputWsName + "_Result";

  // Define params for use in convertParametersToWorkSpace (Refactor to generic)
  auto paramNames = std::vector<std::string>();
  auto func = FunctionFactory::Instance().createFunction(funcName);
  for (size_t i = 0; i < func->nParams(); i++) {
    paramNames.push_back(func->parameterName(i));
  }
  if (funcName.compare("Lorentzian") == 0) {
    // remove peak centre
    paramNames.erase(paramNames.begin() + 1);
    paramNames.push_back("EISF");
  }

  // Run calcEISF if Delta
  if (delta) {
    auto columns = outputWs->getColumnNames();
    calculateEISF(outputWs);
  }

  // Construct comma separated list for ProcessIndirectFirParameters
  std::string paramNamesList = "";
  const size_t maxNames = paramNames.size();
  for (size_t i = 0; i < maxNames; i++) {
    paramNamesList += paramNames.at(i);
    if (i != (maxNames - 1)) {
      paramNamesList += ",";
    }
  }

  // Run ProcessIndirectFitParameters
  auto pifp =
      createChildAlgorithm("ProcessIndirectFitParameters", -1, -1, true);
  pifp->setAlwaysStoreInADS(true);
  pifp->setProperty("InputWorkspace", outputWs);
  pifp->setProperty("ColumnX", "axis-1");
  pifp->setProperty("ParameterNames", paramNamesList);
  pifp->setProperty("OutputWorkspace", resultWsName);
  pifp->executeAsChildAlg();

  API::MatrixWorkspace_sptr resultWs =
      AnalysisDataService::Instance().retrieveWS<MatrixWorkspace>(resultWsName);

  // Set x units to be momentum transfer
  auto axis = resultWs->getAxis(0);
  axis->setUnit("MomentumTransfer");

  // Handle sample logs
  auto logCopier = createChildAlgorithm("CopyLogs", -1, -1, true);
  logCopier->setProperty("InputWorkspace", inputWs);
  logCopier->setProperty("OutputWorkspace", resultWs);
  logCopier->executeAsChildAlg();

  resultWs = logCopier->getProperty("OutputWorkspace");

  // Create Sample Log
  auto sampleLogStrings = std::map<std::string, std::string>();
  sampleLogStrings["sam_workspace"] = inputWs->getName();
  sampleLogStrings["convolve_members"] =
      boost::lexical_cast<std::string>(convolve);
  sampleLogStrings["fit_program"] = "ConvFit";
  sampleLogStrings["background"] = backgroundName;
  sampleLogStrings["delta_function"] = usingDelta;
  sampleLogStrings["temperature_correction"] =
      boost::lexical_cast<std::string>(usingTemp);
  auto sampleLogNumeric = std::map<std::string, std::string>();
  sampleLogNumeric["lorentzians"] =
      boost::lexical_cast<std::string>(usingLorentzians);

  if (usingTemp) {
    sampleLogNumeric["temperature_value"] =
        boost::lexical_cast<std::string>(temperature);
  }

  // Add String Logs
  auto logAdder = createChildAlgorithm("AddSampleLog", -1, -1, true);
  for (auto it = sampleLogStrings.begin(); it != sampleLogStrings.end(); ++it) {
    logAdder->setProperty("Workspace", resultWs);
    logAdder->setProperty("LogName", it->first);
    logAdder->setProperty("LogText", it->second);
    logAdder->setProperty("LogType", "String");
    logAdder->executeAsChildAlg();
  }
  // Add Numeric Logs
  for (auto it = sampleLogNumeric.begin(); it != sampleLogNumeric.end(); it++) {
    logAdder->setProperty("Workspace", resultWs);
    logAdder->setProperty("LogName", it->first);
    logAdder->setProperty("LogText", it->second);
    logAdder->setProperty("LogType", "Number");
    logAdder->executeAsChildAlg();
  }

  // Copy Logs to GroupWorkspace
  logCopier = createChildAlgorithm("CopyLogs", -1, -1, false);
  logCopier->setProperty("InputWorkspace", resultWs);
  logCopier->setProperty("OutputWorkspace",
                         (outputWs->getName() + "_Workspaces"));
  logCopier->executeAsChildAlg();

  // Rename TableWorkspace
  auto renamer = createChildAlgorithm("RenameWorkspace", -1, -1, true);
  renamer->setAlwaysStoreInADS(true);
  renamer->setProperty("InputWorkspace", outputWs);
  renamer->setProperty("OutputWorkspace",
                       (outputWs->getName() + "_Parameters"));
  renamer->executeAsChildAlg();

  // Rename Workspaces in group
  WorkspaceGroup_sptr groupWs =
      AnalysisDataService::Instance().retrieveWS<WorkspaceGroup>(outputWsName +
                                                                 "_Workspaces");
  auto groupWsNames = groupWs->getNames();
  renamer = createChildAlgorithm("RenameWorkspace", -1, -1, true);
  for (int i = specMin; i < specMax + 1; i++) {
    renamer->setProperty("InputWorkspace", groupWsNames.at(i));
    std::string outName = outputWs->getName() + "_";
    outName += boost::lexical_cast<std::string>(i);
    outName += "_Workspace";
    renamer->setProperty("OutputWorkspace", outName);
    renamer->executeAsChildAlg();
  }
}

/**
 * Check function to establish if it is for one lorentzian or Two
 * @param subfunction The unchecked substring of the function
 * @return true if the function is two lorentzian false if one lorentzian
 */
bool ConvolutionFitSequential::checkForTwoLorentz(
    const std::string &subFunction) {
  std::string fitType = "";
  auto pos = subFunction.rfind("name=");
  if (pos != std::string::npos) {
    fitType = subFunction.substr(0, pos);
    pos = fitType.rfind("name=");
    fitType = subFunction.substr(pos, subFunction.size());
    pos = fitType.find_first_of(",");
    fitType = fitType.substr(5, pos - 5);
    if (fitType.compare("Lorentzian") == 0) {
      return true;
    }
  }
  return false;
}

/**
 * Finds specific values embedded in the function supplied to the algorithm
 * @param function The full function string
 * @return all values of interest from the function (0 - fitType, 1 -
 * background, 2 - functionName)
 */
std::vector<std::string>
ConvolutionFitSequential::findValuesFromFunction(const std::string &function) {
  std::vector<std::string> result;
  std::string fitType = "";
  std::string functionName = "";
  auto startPos = function.rfind("name=");
  if (startPos != std::string::npos) {
    fitType = function.substr(startPos, function.size());
    auto nextPos = fitType.find_first_of(",");
    fitType = fitType.substr(5, nextPos - 5);
    functionName = fitType;
    if (fitType.compare("Lorentzian") == 0) {
      std::string newSub = function.substr(0, startPos);
      bool isTwoL = checkForTwoLorentz(function.substr(0, startPos));
      if (isTwoL == true) {
        fitType = "2";
      } else {
        fitType = "1";
      }
    } else {
      fitType = "0";
    }
    result.push_back(fitType);
  }

  std::string background = "";
  auto pos = function.find("name=");
  if (pos != std::string::npos) {
    background = function.substr(pos, function.size());
    pos = background.find_first_of(",");
    background = background.substr(5, pos - 5);
    result.push_back(background);
  }
  result.push_back(functionName);
  return result;
}

/**
 * Searchs for a given fit parameter within the a vector of columnNames
 * @param suffix - The string to search for within the columnName
 * @param columns - A vector of column names to be searched through
 * @return A vector of all the column names that contained the given suffix
 * string
 */
std::vector<std::string> ConvolutionFitSequential::searchForFitParams(
    const std::string &suffix, const std::vector<std::string> &columns) {
  auto fitParams = std::vector<std::string>();
  const size_t totalColumns = columns.size();
  for (size_t i = 0; i < totalColumns; i++) {
    auto pos = columns.at(i).rfind(suffix);
    if (pos != std::string::npos) {
      auto endCheck = pos + suffix.size();
      if (endCheck == columns.at(i).size()) {
        fitParams.push_back(columns.at(i));
      }
    }
  }
  return fitParams;
}

/**
 * Inline function to Square a number
 */
static inline double computeSquare(double x) { return x * x; }

/**
 * Squares all the values inside a vector of type double
 * @param target - The vector to be squared
 * @return The vector after being squared
 */
std::vector<double>
ConvolutionFitSequential::squareVector(std::vector<double> target) {
  std::transform(target.begin(), target.end(), target.begin(), computeSquare);
  return target;
}

/**
 * Creates a vector of type double using the values of another vector
 * @param original - The original vector to be cloned
 * @return A clone of the original vector
 */
std::vector<double>
ConvolutionFitSequential::cloneVector(const std::vector<double> &original) {
  auto result = std::vector<double>(original.begin(), original.end());
  return result;
}

/**
 * Converts the input workspaces to get the Elastic Q axis
 * @param inputWs - The MatrixWorkspace to be converted
 * @param wsName - The desired name of the output workspace
 * @return Shared pointer to the converted workspace
 */
API::MatrixWorkspace_sptr ConvolutionFitSequential::convertInputToElasticQ(
    API::MatrixWorkspace_sptr &inputWs, const std::string &wsName) {
  auto tempFitWs = WorkspaceFactory::Instance().create(
      "Workspace2D", inputWs->getNumberHistograms(), 2, 1);
  auto axis = inputWs->getAxis(1);
  if (axis->isSpectra()) {
    auto convSpec = createChildAlgorithm("ConvertSpectrumAxis", -1, -1, true);
    convSpec->setAlwaysStoreInADS(true);
    convSpec->setProperty("InputWorkSpace", inputWs);
    convSpec->setProperty("OutputWorkSpace", wsName);
    convSpec->setProperty("Target", "ElasticQ");
    convSpec->setProperty("EMode", "Indirect");
    convSpec->executeAsChildAlg();
    tempFitWs = convSpec->getProperty("OutputWorkspace");
  } else if (axis->isNumeric()) {
    // Check that units are Momentum Transfer
    if (axis->unit()->unitID() != "MomentumTransfer") {
      throw std::runtime_error("Input must have axis values of Q");
    }
    auto cloneWs = createChildAlgorithm("CloneWorkspace");
    cloneWs->setProperty("InputWorkspace", inputWs);
    cloneWs->setProperty("OutputWorkspace", wsName);
    cloneWs->executeAsChildAlg();
    tempFitWs = cloneWs->getProperty("OutputWorkspace");
    std::string tempName = tempFitWs->getName();
  } else {
    throw std::runtime_error(
        "Input workspace must have either spectra or numeric axis.");
  }

  return tempFitWs;
}

/**
 * Calculates the EISF if the fit includes a Delta function
 * @param tableWs - The TableWorkspace to append the EISF calculation to
 */
void ConvolutionFitSequential::calculateEISF(API::ITableWorkspace_sptr &tableWs) {
  // Get height data from parameter table
  auto columns = tableWs->getColumnNames();
  std::string height = searchForFitParams("Height", columns).at(0);
  std::string heightErr = searchForFitParams("Height_Err", columns).at(0);
  auto heightY = std::vector<double>();
  tableWs->getColumn(height)->numeric_fill(heightY);
  auto heightE = std::vector<double>();
  tableWs->getColumn(heightErr)->numeric_fill(heightE);

  // Get amplitude column names
  auto ampNames = searchForFitParams("Amplitude", columns);
  auto ampErrorNames = searchForFitParams("Amplitude_Err", columns);

  // For each lorentzian, calculate EISF
  size_t maxSize = ampNames.size();
  if (ampErrorNames.size() > maxSize) {
    maxSize = ampErrorNames.size();
  }
  for (size_t i = 0; i < maxSize; i++) {
    // Get amplitude from column in table workspace
    std::string ampName = ampNames.at(i);
    auto ampY = std::vector<double>();
    tableWs->getColumn(ampName)->numeric_fill(ampY);
    std::string ampErrorName = ampErrorNames.at(i);
    auto ampErr = std::vector<double>();
    tableWs->getColumn(ampErrorName)->numeric_fill(ampErr);

    // Calculate EISF and EISF error
    // total = heightY + ampY
    auto total = cloneVector(heightY);
    std::transform(total.begin(), total.end(), ampY.begin(), total.begin(),
                   std::plus<double>());
    // eisfY = heightY / total
    auto eisfY = cloneVector(heightY);
    std::transform(eisfY.begin(), eisfY.end(), total.begin(), eisfY.begin(),
                   std::divides<double>());
    // heightE squared
    auto heightESq = cloneVector(heightE);
    heightE = squareVector(heightESq);
    // ampErr squared
    auto ampErrSq = cloneVector(ampErr);
    ampErrSq = squareVector(ampErrSq);
    // totalErr = heightE squared + ampErr squared
    auto totalErr = cloneVector(heightESq);
    std::transform(totalErr.begin(), totalErr.end(), ampErrSq.begin(),
                   totalErr.begin(), std::plus<double>());
    // heightY squared
    auto heightYSq = cloneVector(heightY);
    heightYSq = squareVector(heightYSq);
    // total Squared
    auto totalSq = cloneVector(total);
    totalSq = squareVector(totalSq);
    // errOverTotalSq = totalErr / total squared
    auto errOverTotalSq = cloneVector(totalErr);
    std::transform(errOverTotalSq.begin(), errOverTotalSq.end(),
                   totalSq.begin(), errOverTotalSq.begin(),
                   std::divides<double>());
    // heightESqOverYSq = heightESq / heightYSq
    auto heightESqOverYSq = cloneVector(heightESq);
    std::transform(heightESqOverYSq.begin(), heightESqOverYSq.end(),
                   heightYSq.begin(), heightESqOverYSq.begin(),
                   std::divides<double>());
    // sqrtESqOverYSq = squareRoot( heightESqOverYSq )
    auto sqrtESqOverYSq = cloneVector(heightESqOverYSq);
    std::transform(sqrtESqOverYSq.begin(), sqrtESqOverYSq.end(),
                   sqrtESqOverYSq.begin(), (double (*)(double))sqrt);
    // eisfYSumRoot = eisfY * sqrtESqOverYSq
    auto eisfYSumRoot = cloneVector(eisfY);
    std::transform(eisfYSumRoot.begin(), eisfYSumRoot.end(),
                   sqrtESqOverYSq.begin(), eisfYSumRoot.begin(),
                   std::multiplies<double>());
    // eisfErr = eisfYSumRoot + errOverTotalSq
    auto eisfErr = cloneVector(eisfYSumRoot);
    std::transform(eisfErr.begin(), eisfErr.end(), errOverTotalSq.begin(),
                   eisfErr.begin(), std::plus<double>());

    // Append the calculated values to the table workspace
    std::string columnName =
        ampName.substr(0, (ampName.size() - std::string("Amplitude").size()));
    columnName += "EISF";
    std::string errorColumnName = ampErrorName.substr(
        0, (ampName.size() - std::string("Amplitude_Err").size()));
    errorColumnName += "EISF_Err";

    tableWs->addColumn("double", columnName);
    tableWs->addColumn("double", errorColumnName);
    size_t maxEisf = eisfY.size();
    if (eisfErr.size() > maxEisf) {
      maxEisf = eisfErr.size();
    }

    Column_sptr col = tableWs->getColumn(columnName);
    Column_sptr errCol = tableWs->getColumn(errorColumnName);
    for (size_t j = 0; j < maxEisf; j++) {
      col->cell<double>(j) = eisfY.at(j);
      errCol->cell<double>(j) = eisfErr.at(j);
    }
  }
}

} // namespace Algorithms
} // namespace Mantid