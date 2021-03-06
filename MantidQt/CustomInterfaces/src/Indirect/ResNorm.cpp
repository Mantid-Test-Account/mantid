#include "MantidQtCustomInterfaces/Indirect/ResNorm.h"

#include "MantidQtCustomInterfaces/UserInputValidator.h"
#include "MantidAPI/WorkspaceFactory.h"
#include "MantidAPI/ITableWorkspace.h"

using namespace Mantid::API;

namespace MantidQt {
namespace CustomInterfaces {
ResNorm::ResNorm(QWidget *parent) : IndirectBayesTab(parent), m_previewSpec(0) {
  m_uiForm.setupUi(parent);

  // Create range selector
  auto eRangeSelector = m_uiForm.ppPlot->addRangeSelector("ResNormERange");
  connect(eRangeSelector, SIGNAL(minValueChanged(double)), this,
          SLOT(minValueChanged(double)));
  connect(eRangeSelector, SIGNAL(maxValueChanged(double)), this,
          SLOT(maxValueChanged(double)));

  // Add the properties browser to the ui form
  m_uiForm.treeSpace->addWidget(m_propTree);

  m_properties["EMin"] = m_dblManager->addProperty("EMin");
  m_properties["EMax"] = m_dblManager->addProperty("EMax");

  m_dblManager->setDecimals(m_properties["EMin"], NUM_DECIMALS);
  m_dblManager->setDecimals(m_properties["EMax"], NUM_DECIMALS);

  m_propTree->addProperty(m_properties["EMin"]);
  m_propTree->addProperty(m_properties["EMax"]);

  // Connect data selector to handler method
  connect(m_uiForm.dsVanadium, SIGNAL(dataReady(const QString &)), this,
          SLOT(handleVanadiumInputReady(const QString &)));
  connect(m_uiForm.dsResolution, SIGNAL(dataReady(const QString &)), this,
          SLOT(handleResolutionInputReady(const QString &)));

  // Connect the preview spectrum selector
  connect(m_uiForm.spPreviewSpectrum, SIGNAL(valueChanged(int)), this,
          SLOT(previewSpecChanged(int)));

  connect(m_batchAlgoRunner, SIGNAL(batchComplete(bool)), this,
          SLOT(handleAlgorithmComplete(bool)));
}

void ResNorm::setup() {}

/**
 * Validate the form to check the program can be run
 *
 * @return :: Whether the form was valid
 */
bool ResNorm::validate() {
  UserInputValidator uiv;
  QString errors("");

  const bool vanValid = uiv.checkDataSelectorIsValid("Vanadium", m_uiForm.dsVanadium);
  const bool resValid = uiv.checkDataSelectorIsValid("Resolution", m_uiForm.dsResolution);

  if (vanValid) {
    // Check vanadium input is _red ws
    QString vanadiumName = m_uiForm.dsVanadium->getCurrentDataName();
    int cutIndex = vanadiumName.lastIndexOf("_");
    QString vanadiumSuffix =
        vanadiumName.right(vanadiumName.size() - (cutIndex + 1));
    if (vanadiumSuffix.compare("red") != 0) {
      uiv.addErrorMessage(
          "The Vanadium run is not a reduction (_red) workspace");
    }

    // Check Res and Vanadium are the same Run
    if (resValid) {
      // Check that Res file is still in ADS if not, load it
      auto resolutionWs =
          AnalysisDataService::Instance().retrieveWS<MatrixWorkspace>(
              m_uiForm.dsResolution->getCurrentDataName().toStdString());
      auto vanadiumWs =
          AnalysisDataService::Instance().retrieveWS<MatrixWorkspace>(
              vanadiumName.toStdString());

      const int resRun = resolutionWs->getRunNumber();
      const int vanRun = vanadiumWs->getRunNumber();

      if (resRun != vanRun) {
        uiv.addErrorMessage("The provided Vanadium and Resolution do not have "
                            "matching run numbers");
      }
    }
  }

  // check eMin and eMax values
  const auto eMin = m_dblManager->value(m_properties["EMin"]);
  const auto eMax = m_dblManager->value(m_properties["EMax"]);
  if (eMin >= eMax)
    errors.append("EMin must be strictly less than EMax.\n");

  // Create and show error messages
  errors.append(uiv.generateErrorMessage());
  if (!errors.isEmpty()) {
    emit showMessageBox(errors);
    return false;
  }

  return true;
}

/**
 * Run the ResNorm v2 algorithm.
 */
void ResNorm::run() {
  QString vanWsName(m_uiForm.dsVanadium->getCurrentDataName());
  QString resWsName(m_uiForm.dsResolution->getCurrentDataName());

  double eMin(m_dblManager->value(m_properties["EMin"]));
  double eMax(m_dblManager->value(m_properties["EMax"]));

  QString outputWsName = getWorkspaceBasename(resWsName) + "_ResNorm";


  IAlgorithm_sptr resNorm = AlgorithmManager::Instance().create("ResNorm", 2);
  resNorm->initialize();
  resNorm->setProperty("VanadiumWorkspace", vanWsName.toStdString());
  resNorm->setProperty("ResolutionWorkspace", resWsName.toStdString());
  resNorm->setProperty("EnergyMin", eMin);
  resNorm->setProperty("EnergyMax", eMax);
  resNorm->setProperty("CreateOutput", true);
  resNorm->setProperty("OutputWorkspace", outputWsName.toStdString());
  resNorm->setProperty("OutputWorkspaceTable", (outputWsName + "_Fit").toStdString());
  m_batchAlgoRunner->addAlgorithm(resNorm);

  // Handle saving
  bool save(m_uiForm.ckSave->isChecked());
  if (save)
    addSaveWorkspaceToQueue(outputWsName);

  m_pythonExportWsName = outputWsName.toStdString();
  m_batchAlgoRunner->executeBatchAsync();

}

/**
 * Handle completion of the algorithm.
 *
 * @param error If the algorithm failed
 */
void ResNorm::handleAlgorithmComplete(bool error) {
  if (error)
    return;

  QString outputBase = (m_uiForm.dsResolution->getCurrentDataName()).toLower();
  const int indexCut = outputBase.lastIndexOf("_");
  outputBase = outputBase.left(indexCut);
  outputBase += "_ResNorm";

  std::string outputBaseStr = outputBase.toStdString();

  WorkspaceGroup_sptr fitWorkspaces =
      AnalysisDataService::Instance().retrieveWS<WorkspaceGroup>(
          outputBaseStr + "_Fit_Workspaces");
  QString fitWsName("");
  if (fitWorkspaces)
    fitWsName =
        QString::fromStdString(fitWorkspaces->getItem(m_previewSpec)->name());

  // MantidPlot plotting
  QString plotOptions(m_uiForm.cbPlot->currentText());
  if (plotOptions == "Intensity" || plotOptions == "All")
    plotSpectrum(QString::fromStdString(m_pythonExportWsName) + "_Intensity");
  if (plotOptions == "Stretch" || plotOptions == "All")
    plotSpectrum(QString::fromStdString(m_pythonExportWsName) + "_Stretch");
  if (plotOptions == "Fit" || plotOptions == "All")
    plotSpectrum(fitWsName, 0, 1);

  loadFile(m_uiForm.dsResolution->getFullFilePath(),
           m_uiForm.dsResolution->getCurrentDataName());

  // Update preview plot
  previewSpecChanged(m_previewSpec);
}

/**
 * Set the data selectors to use the default save directory
 * when browsing for input files.
 *
 * @param settings :: The current settings
 */
void ResNorm::loadSettings(const QSettings &settings) {
  m_uiForm.dsVanadium->readSettings(settings.group());
  m_uiForm.dsResolution->readSettings(settings.group());
}

/**
 * Plots the loaded file to the miniplot and sets the guides
 * and the range
 *
 * @param filename :: The name of the workspace to plot
 */
void ResNorm::handleVanadiumInputReady(const QString &filename) {
  // Plot the vanadium
  m_uiForm.ppPlot->addSpectrum("Vanadium", filename, m_previewSpec);

  QPair<double, double> res;
  QPair<double, double> range = m_uiForm.ppPlot->getCurveRange("Vanadium");

  MatrixWorkspace_sptr vanWs =
      AnalysisDataService::Instance().retrieveWS<MatrixWorkspace>(
          filename.toStdString());
  m_uiForm.spPreviewSpectrum->setMaximum(
      static_cast<int>(vanWs->getNumberHistograms()) - 1);

  auto eRangeSelector = m_uiForm.ppPlot->getRangeSelector("ResNormERange");

  // Use the values from the instrument parameter file if we can
  if (getResolutionRangeFromWs(filename, res)) {
    // ResNorm resolution should be +/- 10 * the IPF resolution
    res.first = res.first * 10;
    res.second = res.second * 10;

    setRangeSelector(eRangeSelector, m_properties["EMin"], m_properties["EMax"],
                     res);
  } else {
    setRangeSelector(eRangeSelector, m_properties["EMin"], m_properties["EMax"],
                     range);
  }

  setPlotPropertyRange(eRangeSelector, m_properties["EMin"],
                       m_properties["EMax"], range);
}

/**
 * Plots the loaded resolution file on the mini plot.
 *
 * @param filename Name of the workspace to plot
 */
void ResNorm::handleResolutionInputReady(const QString &filename) {
  // Plot the resolution
  m_uiForm.ppPlot->addSpectrum("Resolution", filename, 0, Qt::blue);
}

/**
 * Updates the property manager when the lower guide is moved on the mini plot
 *
 * @param min :: The new value of the lower guide
 */
void ResNorm::minValueChanged(double min) {
  m_dblManager->setValue(m_properties["EMin"], min);
}

/**
 * Updates the property manager when the upper guide is moved on the mini plot
 *
 * @param max :: The new value of the upper guide
 */
void ResNorm::maxValueChanged(double max) {
  m_dblManager->setValue(m_properties["EMax"], max);
}

/**
 * Handles when properties in the property manager are updated.
 *
 * @param prop :: The property being updated
 * @param val :: The new value for the property
 */
void ResNorm::updateProperties(QtProperty *prop, double val) {
  UNUSED_ARG(val);

  auto eRangeSelector = m_uiForm.ppPlot->getRangeSelector("ResNormERange");

  if (prop == m_properties["EMin"] || prop == m_properties["EMax"]) {
    auto bounds = qMakePair(m_dblManager->value(m_properties["EMin"]),
                            m_dblManager->value(m_properties["EMax"]));
    setRangeSelector(eRangeSelector, m_properties["EMin"], m_properties["EMax"],
                     bounds);
  }
}

/**
 * Sets a new preview spectrum for the mini plot.
 *
 * @param value Spectrum index
 */
void ResNorm::previewSpecChanged(int value) {
  m_previewSpec = value;

  // Update vanadium plot
  if (m_uiForm.dsVanadium->isValid())
    m_uiForm.ppPlot->addSpectrum(
        "Vanadium", m_uiForm.dsVanadium->getCurrentDataName(), m_previewSpec);

  // Update fit plot
  std::string fitWsGroupName(m_pythonExportWsName + "_Fit_Workspaces");
  std::string fitParamsName(m_pythonExportWsName + "_Fit");
  if (AnalysisDataService::Instance().doesExist(fitWsGroupName)) {
    WorkspaceGroup_sptr fitWorkspaces =
        AnalysisDataService::Instance().retrieveWS<WorkspaceGroup>(
            fitWsGroupName);
    ITableWorkspace_sptr fitParams =
        AnalysisDataService::Instance().retrieveWS<ITableWorkspace>(
            fitParamsName);
    if (fitWorkspaces && fitParams) {
      Column_const_sptr scaleFactors = fitParams->getColumn("Scaling");
      std::string fitWsName(fitWorkspaces->getItem(m_previewSpec)->name());
      MatrixWorkspace_const_sptr fitWs =
          AnalysisDataService::Instance().retrieveWS<MatrixWorkspace>(
              fitWsName);

      MatrixWorkspace_sptr fit = WorkspaceFactory::Instance().create(fitWs, 1);
      fit->setX(0, fitWs->readX(1));
      fit->getSpectrum(0)->setData(fitWs->readY(1), fitWs->readE(1));

      for (size_t i = 0; i < fit->blocksize(); i++)
        fit->dataY(0)[i] /= scaleFactors->cell<double>(m_previewSpec);

      m_uiForm.ppPlot->addSpectrum("Fit", fit, 0, Qt::red);
    }
  }
}

} // namespace CustomInterfaces
} // namespace MantidQt
