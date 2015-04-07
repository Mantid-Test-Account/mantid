#ifndef MULTIDATASETFIT_H_
#define MULTIDATASETFIT_H_

#include "MantidQtCustomInterfaces/DllConfig.h"
#include "MantidQtAPI/UserSubWindow.h"
#include "MantidAPI/AlgorithmObserver.h"
#include "MantidQtAPI/WorkspaceObserver.h"
#include "ui_MultiDatasetFit.h"
#include "ui_AddWorkspace.h"
#include "ui_EditLocalParameterDialog.h"

#include <QStyledItemDelegate>

#include <boost/shared_ptr.hpp>
#include <QMap>
#include <vector>

// Forward declarations
class QwtPlot;
class QwtPlotZoomer;
class QwtPlotPanner;
class QwtPlotMagnifier;
class QTableWidget;
class QComboBox;
class QPushButton;
class QAction;

namespace Mantid
{
namespace API
{
  class IFunction;
  class IAlgorithm;
  class MatrixWorkspace;
}
}

namespace MantidQt
{

// Forward declarations
namespace MantidWidgets
{
  class FunctionBrowser;
  class RangeSelector;
  class FitOptionsBrowser;
}

namespace API
{
  class AlgorithmRunner;
}

namespace CustomInterfaces
{

// Forward declarations
class PlotController;
class DatasetPlotData;
class DataController;

/**
 * Class MultiDatasetFitDialog implements a dialog for setting up a multi-dataset fit
 * and displaying the results.
 */

class MANTIDQT_CUSTOMINTERFACES_DLL MultiDatasetFit: public API::UserSubWindow
{
  Q_OBJECT
public:
  /// The name of the interface as registered into the factory
  static std::string name() { return "Multi dataset fitting"; }
  // This interface's categories.
  static QString categoryInfo() { return "General"; }
  /// Constructor
  MultiDatasetFit(QWidget *parent = NULL);
  ~MultiDatasetFit();
  /// Get the name of the output workspace
  QString getOutputWorkspaceName() const {return QString::fromStdString(m_outputWorkspaceName);}
  /// Workspace name for the i-th spectrum
  std::string getWorkspaceName(int i) const;
  /// Workspace index of the i-th spectrum
  int getWorkspaceIndex(int i) const;
  /// Get the fitting range for the i-th spectrum
  std::pair<double,double> getFittingRange(int i) const;
  /// Total number of spectra (datasets).
  int getNumberOfSpectra() const;
  /// Display info about the plot.
  void showPlotInfo();
  /// Check that the data sets in the table are valid
  void checkSpectra();
  /// Get value of a local parameter
  double getLocalParameterValue(const QString& parName, int i) const;
  /// Set value of a local parameter
  void setLocalParameterValue(const QString& parName, int i, double value);
  /// Check if a local parameter is fixed
  bool isLocalParameterFixed(const QString& parName, int i) const;
  /// Fix/unfix local parameter
  void setLocalParameterFixed(const QString& parName, int i, bool fixed);

public slots:
  void reset();

private slots:
  void fit();
  void editLocalParameterValues(const QString& parName);
  void finishFit(bool);
  void enableZoom();
  void enablePan();
  void enableRange();

protected:
  /// To be overridden to set the appropriate layout
  virtual void initLayout();

private:
  void createPlotToolbar();
  boost::shared_ptr<Mantid::API::IFunction> createFunction() const;
  void updateParameters(const Mantid::API::IFunction& fun);
  void showInfo(const QString& text);
  bool eventFilter(QObject *widget, QEvent *evn);
  void showFunctionBrowserInfo();
  void showFitOptionsBrowserInfo();
  void showTableInfo();
  void removeSpectra(QList<int> rows);
  void loadSettings();
  void saveSettings() const;

  /// The form generated by Qt Designer
  Ui::MultiDatasetFit m_uiForm;
  /// Controls the plot and plotted data.
  PlotController *m_plotController;
  /// Contains all logic of dealing with data sets.
  DataController *m_dataController;
  /// Function editor
  MantidWidgets::FunctionBrowser *m_functionBrowser;
  /// Browser for setting other Fit properties
  MantidWidgets::FitOptionsBrowser *m_fitOptionsBrowser;
  /// Name of the output workspace
  std::string m_outputWorkspaceName;
  /// Fit algorithm runner
  boost::shared_ptr<API::AlgorithmRunner> m_fitRunner;
};

/*==========================================================================================*/
/**
  * A dialog for selecting a workspace from the ADS.
  */
class AddWorkspaceDialog: public QDialog
{
  Q_OBJECT
public:
  AddWorkspaceDialog(QWidget *parent);
  QString workspaceName() const {return m_workspaceName;} 
  std::vector<int> workspaceIndices() const {return m_wsIndices;}
private slots:
  void accept();
  void reject();
  void workspaceNameChanged(const QString&);
  void selectAllSpectra(int state);
private:
  /// Name of the selected workspace
  QString m_workspaceName;
  /// Selected workspace index
  std::vector<int> m_wsIndices;
  /// Maximum index in the selected workspace
  int m_maxIndex;
  Ui::AddWorkspace m_uiForm;
};

/*==========================================================================================*/
/**
  * A class for controlling the plot widget and the displayed data.
  */
class PlotController: public QObject
{
  Q_OBJECT
public:
  PlotController(MultiDatasetFit *parent, QwtPlot *plot, QTableWidget *table, QComboBox *plotSelector, QPushButton *prev, QPushButton *next);
  ~PlotController();
  void clear();
  void update();
  int getCurrentIndex() const {return m_currentIndex;}
  bool isZoomEnabled() const;
  bool isPanEnabled() const;
  bool isRangeSelectorEnabled() const;
signals:
  void currentIndexChanged(int);
  void fittingRangeChanged(int, double, double);
public slots:
  void enableZoom();
  void enablePan();
  void enableRange();
  void updateRange(int index);
private slots:
  void tableUpdated();
  void prevPlot();
  void nextPlot();
  void plotDataSet(int);
  void updateFittingRange(double startX, double endX);
private:
  MultiDatasetFit *owner() const {return static_cast<MultiDatasetFit*>(parent());}
  void disableAllTools();
  template<class Tool>
  void enableTool(Tool* tool, int cursor);
  bool eventFilter(QObject *widget, QEvent *evn);
  void resetRange();
  void zoomToRange();
  boost::shared_ptr<DatasetPlotData> getData(int i);

  /// The plot widget
  QwtPlot *m_plot;

  ///@name Plot tools
  ///@{
  /// The zoomer
  QwtPlotZoomer *m_zoomer;
  /// The panner
  QwtPlotPanner *m_panner;
  /// The magnifier
  QwtPlotMagnifier *m_magnifier;
  /// The fitting range selector
  MantidWidgets::RangeSelector* m_rangeSelector;
  ///@}

  /// The workspace table
  QTableWidget *m_table;
  QComboBox *m_plotSelector;
  QPushButton *m_prevPlot;
  QPushButton *m_nextPlot;
  QMap<int,boost::shared_ptr<DatasetPlotData>> m_plotData;
  int m_currentIndex;
};

/*==========================================================================================*/
/**
  * A dialog for displaying and editing values of local parameters.
  */
class EditLocalParameterDialog: public QDialog
{
  Q_OBJECT
public:
  EditLocalParameterDialog(MultiDatasetFit *parent, const QString &parName);
  QList<double> getValues() const;
  QList<bool> getFixes() const;
  bool isFixed(int i) const {return m_fixes[i];}
private slots:
  void valueChanged(int,int);
  void setAllValues(double);
  void fixParameter(int,bool);
  void setAllFixed(bool);
  void paste();
private:
  bool eventFilter(QObject * obj, QEvent * ev);
  void showContextMenu();
  Ui::EditLocalParameterDialog m_uiForm;
  QString m_parName;
  QList<double> m_values;
  QList<bool> m_fixes;
};

/*==========================================================================================*/
class LocalParameterEditor: public QWidget
{
  Q_OBJECT
public:
  LocalParameterEditor(QWidget *parent, int index, bool fixed);
signals:
  void setAllValues(double);
  void fixParameter(int,bool);
  void setAllFixed(bool);
private slots:
  void buttonPressed();
  void fixParameter();
  void fixAll();
  void unfixAll();
private:
  QLineEdit* m_editor;
  QAction *m_fixAction;
  int m_index;
  bool m_fixed;
};

class LocalParameterItemDelegate: public QStyledItemDelegate
{
  Q_OBJECT
public:
  LocalParameterItemDelegate(EditLocalParameterDialog *parent = NULL);
  QWidget* createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
  void setEditorData(QWidget * editor, const QModelIndex & index) const;
  void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;
signals:
  void setAllValues(double);
  void fixParameter(int,bool);
  void setAllFixed(bool);
protected:
  void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
private:
  bool eventFilter(QObject * obj, QEvent * ev);
  EditLocalParameterDialog *owner() const {return static_cast<EditLocalParameterDialog*>(parent());}
  mutable LocalParameterEditor* m_currentEditor;
};

/*==========================================================================================*/
/**
  * A class for controlling the plot widget and the displayed data.
  */
class DataController: public QObject
{
  Q_OBJECT
public:
  DataController(MultiDatasetFit *parent, QTableWidget *dataTable);
  std::string getWorkspaceName(int i) const;
  int getWorkspaceIndex(int i) const;
  int getNumberOfSpectra() const;
  void checkSpectra();
  std::pair<double,double> getFittingRange(int i) const;

signals:
  void dataTableUpdated();
  void dataSetUpdated(int i);
  void hasSelection(bool);
  void spectraRemoved(QList<int>);
  void spectraAdded(int n);

public slots:
  void setFittingRangeGlobal(bool);
  void setFittingRange(int, double, double);

private slots:
  void addWorkspace();
  void workspaceSelectionChanged();
  void removeSelectedSpectra();
  void updateDataset(int, int);

private:
  MultiDatasetFit *owner() const {return static_cast<MultiDatasetFit*>(parent());}
  void addWorkspaceSpectrum(const QString &wsName, int wsIndex, const Mantid::API::MatrixWorkspace& ws);
  void removeSpectra(QList<int> rows);

  /// Table with data set names and other data.
  QTableWidget *m_dataTable;
  /// Flag for setting the fitting range.
  bool m_isFittingRangeGlobal;
};

} // CustomInterfaces
} // MantidQt


#endif /*MULTIDATASETFITDIALOG_H_*/
