#include "MantidQtCustomDialogs/GetNegMuMuonicXRDDialog.h"
#include "MantidQtAPI/AlgorithmInputHistory.h"
#include "MantidQtCustomDialogs/MantidGLWidget.h"
#include <QCheckBox>
#include <QMessageBox>
#include <QLineEdit>
#include <QValidator>
#include <QFormLayout>

namespace MantidQt {
namespace CustomDialogs {
DECLARE_DIALOG(GetNegMuMuonicXRDDialog)

/**
 * Default constructor.
 * @param parent :: Parent dialog.
 */

GetNegMuMuonicXRDDialog::GetNegMuMuonicXRDDialog(QWidget *parent)
    : API::AlgorithmDialog(parent), m_periodicTable(NULL), m_yPosition(NULL),
      m_groupWorkspaceNameInput(NULL), m_showLegendCheck(NULL) {}

/// Initialise the layout
void GetNegMuMuonicXRDDialog::initLayout() {
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  this->setMaximumHeight(450);
  this->setMaximumWidth(675);
  // assign periodicTable member to a new periodicTable
  m_periodicTable = new PeriodicTableWidget();

  // assign m_yPosition member to a new QLineEdit
  m_yPosition = new QDoubleSpinBox();
  // assign GroupWorkspaceName member to a new QLineEdit
  m_groupWorkspaceNameInput = new QLineEdit();
  auto *groupWsInputLabel = new QLabel("OutputWorkspace");
  m_groupWorkspaceNameInput->setMaximumWidth(250);
  // Disable all buttons on the periodicTable
  // as we only have a select few that need to be
  // enabled.
  m_periodicTable->disableAllElementButtons();

  /*Elements Enabled Correspond to those for which we
  * have data for in the dictionary found in
  * GetNegMuMuonicXRD.py file
  */
  enableElementsForGetNegMuMuonicXRD();

  // main layout for the dialog (everything will be added to this)
  auto *main_layout = new QVBoxLayout(this);

  // run button for executing the algorithm
  auto *runButton = new QPushButton("Run");

  // label for the QLineEdit for m_yPosition property
  auto *m_yPositionLabel = new QLabel("Y Position");

  // YPosition LineEdit Attributes
  m_yPosition->setMaximumWidth(250);
  m_yPosition->setRange(-100, 100);
  m_yPosition->setSingleStep(0.1);

  // Run Button Attributes and signal/slot assignment
  runButton->setMaximumWidth(100);
  connect(runButton, SIGNAL(clicked()), this, SLOT(runClicked()));
  connect(this, SIGNAL(validInput()), this, SLOT(accept()));

  // Show Legend button attributes and signal/slot asssignment
  m_showLegendCheck = new QCheckBox("Show Legend");
  connect(m_showLegendCheck, SIGNAL(clicked()), this, SLOT(showLegend()));
  // Adding Widgets to Layout
  main_layout->addWidget(m_periodicTable);
  main_layout->addWidget(m_showLegendCheck);
  main_layout->addWidget(m_yPositionLabel);
  main_layout->addWidget(m_yPosition);
  main_layout->addWidget(groupWsInputLabel);
  main_layout->addWidget(m_groupWorkspaceNameInput);
  main_layout->addWidget(runButton);
}

/**
 *
 */
void GetNegMuMuonicXRDDialog::showLegend() {
  bool checked = m_showLegendCheck->isChecked();
  m_periodicTable->showGroupLegend(checked);
}

/**
 * Enables the buttons for which we have data for in the GetNegMuMuonicXRD.py
 * dictionary of elements, by Periodic Table symbol.
 * i.e Au corresponds to Gold.
 */
void GetNegMuMuonicXRDDialog::enableElementsForGetNegMuMuonicXRD() {
  /* The GetNegMuMuonic algorithm only has data for these elements
   * The dictionary of elements and data can edited in the python file
   * for the algorithm, and the button for that element can be enabled
   * the same as the elements are below.
   */
  m_periodicTable->enableButtonByName("Ag");
  m_periodicTable->enableButtonByName("As");
  m_periodicTable->enableButtonByName("Au");
  m_periodicTable->enableButtonByName("Ba");
  m_periodicTable->enableButtonByName("Bi");
  m_periodicTable->enableButtonByName("Ca");
  m_periodicTable->enableButtonByName("Cd");
  m_periodicTable->enableButtonByName("Ce");
  m_periodicTable->enableButtonByName("Co");
  m_periodicTable->enableButtonByName("Cr");
  m_periodicTable->enableButtonByName("Cs");
  m_periodicTable->enableButtonByName("Cu");
  m_periodicTable->enableButtonByName("Fe");
  m_periodicTable->enableButtonByName("Hg");
  m_periodicTable->enableButtonByName("I");
  m_periodicTable->enableButtonByName("In");
  m_periodicTable->enableButtonByName("La");
  m_periodicTable->enableButtonByName("Mn");
  m_periodicTable->enableButtonByName("Mo");
  m_periodicTable->enableButtonByName("Nb");
  m_periodicTable->enableButtonByName("Nd");
  m_periodicTable->enableButtonByName("Ni");
  m_periodicTable->enableButtonByName("Pb");
  m_periodicTable->enableButtonByName("Pd");
  m_periodicTable->enableButtonByName("Pr");
  m_periodicTable->enableButtonByName("Rh");
  m_periodicTable->enableButtonByName("Sb");
  m_periodicTable->enableButtonByName("Sn");
  m_periodicTable->enableButtonByName("Sr");
  m_periodicTable->enableButtonByName("Te");
  m_periodicTable->enableButtonByName("Ti");
  m_periodicTable->enableButtonByName("Tl");
  m_periodicTable->enableButtonByName("V");
  m_periodicTable->enableButtonByName("Y");
  m_periodicTable->enableButtonByName("Zn");
  m_periodicTable->enableButtonByName("Zr");
}

/**
 * Used for checking if the input is none empty for Y-Position Property
 * and if any elements have been selected from the periodicTableWidget
 * @param input :: A QString that is checked to see if it is empty.
*/

bool GetNegMuMuonicXRDDialog::validateDialogInput(QString input) {
  // empty check on input
  return (input != "");
}

/**
 * The Slot to gather input from the dialog, store it in the propertyValue
 * and then emit the signal for valid input. Preparing for accept() to be run.
*/
void GetNegMuMuonicXRDDialog::runClicked() {
  // getting a list of strings of elements selected from periodicTableWidget
  QString elementsSelectedStr = m_periodicTable->getAllCheckedElementsStr();
  // if no elements are selected from the PeriodicTableWidget, a pop-up appears
  // to the user.
  if (!validateDialogInput(elementsSelectedStr)) {
    QMessageBox::information(
        this, "GetNegMuMuonicXRDDialog",
        "No elements were selected, Please select an element from the table");
  }
  // If elements have been selected and y-position text is non-empty then
  // store the inputs as the corresponding propertyValues and emit validInput
  // signal.
  if (validateDialogInput(elementsSelectedStr)) {
    storePropertyValue("Elements", elementsSelectedStr);
    if (validateDialogInput(m_yPosition->text())) {
      storePropertyValue("YAxisPosition",
                         QString::number(m_yPosition->value()));
    }
    if (validateDialogInput(m_groupWorkspaceNameInput->text())) {
      storePropertyValue("OutputWorkspace", m_groupWorkspaceNameInput->text());
    }
    emit validInput();
  }
}
}
}
