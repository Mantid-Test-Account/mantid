#ifndef MANTIDWIDGETS_FITOPTIONSBROWSER_H_
#define MANTIDWIDGETS_FITOPTIONSBROWSER_H_

#include "WidgetDllOption.h"

#include <QWidget>
#include <QMap>

/* Forward declarations */
class QtProperty;
class QtTreePropertyBrowser;
class QtDoublePropertyManager;
class QtIntPropertyManager;
class QtBoolPropertyManager;
class QtStringPropertyManager;
class QtEnumPropertyManager;
class QtGroupPropertyManager;
class QSettings;

namespace Mantid
{
namespace Kernel
{
  class Property;
}
namespace API
{
  class IAlgorithm;
}
}

namespace MantidQt
{
namespace MantidWidgets
{

/**
 * Class FitOptionsBrowser implements QtPropertyBrowser to display 
 * and set properties of Fit algorithm (excluding Function and Workspace)
 * 
 */
class EXPORT_OPT_MANTIDQT_MANTIDWIDGETS FitOptionsBrowser: public QWidget
{
  Q_OBJECT
public:
  /// Support for fitting algorithms:
  ///   Simultaneous: Fit
  ///   Sequential:   PlotPeakByLogValue
  ///   SimultaneousAndSequential: both Fit and PlotPeakByLogValue, toggled with
  ///       "Fitting" property.
  enum FittingType {Simultaneous = 0, Sequential, SimultaneousAndSequential};

  /// Constructor
  FitOptionsBrowser(QWidget *parent = NULL, FittingType fitType = Simultaneous);
  QString getProperty(const QString& name) const;
  void setProperty(const QString& name, const QString& value);
  void copyPropertiesToAlgorithm(Mantid::API::IAlgorithm& fit) const;
  void saveSettings(QSettings& settings) const;
  void loadSettings(const QSettings& settings);
  FittingType getCurrentFittingType() const;
  void setCurrentFittingType(FittingType fitType);
  void lockCurrentFittingType(FittingType fitType);
  void unlockCurrentFittingType();
  void setLogNames(const QStringList& logNames);
  void setParameterNamesForPlotting(const QStringList& parNames);
  QString getParameterToPlot() const;

signals:
  void changedToSequentialFitting();

private slots:

  void enumChanged(QtProperty*);

private:

  void createBrowser();
  void createProperties();
  void createCommonProperties();
  void createSimultaneousFitProperties();
  void createSequentialFitProperties();
  void updateMinimizer();
  void switchFitType();
  void displayNormalFitProperties();
  void displaySequentialFitProperties();

  QtProperty* createPropertyProperty(Mantid::Kernel::Property* prop);
  QtProperty* addDoubleProperty(const QString& name);

  void addProperty(const QString& name, QtProperty* prop,
    QString (FitOptionsBrowser::*getter)(QtProperty*)const, 
    void (FitOptionsBrowser::*setter)(QtProperty*,const QString&));

  //  Setters and getters
  QString getMinimizer(QtProperty*) const;
  void setMinimizer(QtProperty*, const QString&);

  QString getIntProperty(QtProperty*) const;
  void setIntProperty(QtProperty*, const QString&);
  QString getBoolProperty(QtProperty*) const;
  void setBoolProperty(QtProperty*, const QString&);
  QString getStringEnumProperty(QtProperty*) const;
  void setStringEnumProperty(QtProperty*, const QString&);
  QString getStringProperty(QtProperty*) const;
  void setStringProperty(QtProperty*, const QString&);

  void setPropertyEnumValues(QtProperty* prop, const QStringList& values);

  /// Qt property browser which displays properties
  QtTreePropertyBrowser* m_browser;

  /// Manager for double properties
  QtDoublePropertyManager* m_doubleManager;
  /// Manager for int properties
  QtIntPropertyManager* m_intManager;
  /// Manager for bool properties
  QtBoolPropertyManager* m_boolManager;
  /// Manager for string properties
  QtStringPropertyManager* m_stringManager;
  /// Manager for the string list properties
  QtEnumPropertyManager* m_enumManager;
  /// Manager for groups of properties
  QtGroupPropertyManager* m_groupManager;

  /// FitType property
  QtProperty* m_fittingTypeProp;
  /// Minimizer group property
  QtProperty* m_minimizerGroup;
  /// Minimizer property
  QtProperty* m_minimizer;
  /// CostFunction property
  QtProperty* m_costFunction;
  /// MaxIterations property
  QtProperty* m_maxIterations;
  
  // Fit properties
  /// Output property
  QtProperty* m_output;
  /// IgnoreInvalidData property
  QtProperty* m_ignoreInvalidData;

  // PlotPeakByLogValue properties
  /// FitType property
  QtProperty* m_fitType;
  /// OutputWorkspace property
  QtProperty* m_outputWorkspace;
  /// LogValue property
  QtProperty* m_logValue;
  /// Property for a name of a parameter to plot
  /// against LogValue
  QtProperty* m_plotParameter;

  /// Precision of doubles in m_doubleManager
  int m_decimals;

  typedef  void (FitOptionsBrowser::*SetterType)(QtProperty*, const QString&);
  typedef  QString (FitOptionsBrowser::*GetterType)(QtProperty*)const;
  /// Maps algorithm property name to the QtProperty
  QMap<QString,QtProperty*> m_propertyNameMap;
  /// Store for the properties setter methods
  QMap<QtProperty*,SetterType> m_setters;
  /// Store for the properties getter methods
  QMap<QtProperty*,GetterType> m_getters;

  /// The Fitting Type
  FittingType m_fittingType;
  /// Store special properties of the normal Fit
  QList<QtProperty*> m_simultaneousProperties;
  /// Store special properties of the sequential Fit
  QList<QtProperty*> m_sequentialProperties;
};

} // MantidWidgets

} // MantidQt

#endif /*MANTIDWIDGETS_FITOPTIONSBROWSER_H_*/
