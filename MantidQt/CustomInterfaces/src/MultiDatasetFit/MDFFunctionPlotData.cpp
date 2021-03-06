#include "MantidQtCustomInterfaces/MultiDatasetFit/MDFFunctionPlotData.h"
#include "MantidQtMantidWidgets/ErrorCurve.h"

#include "MantidAPI/AnalysisDataService.h"
#include "MantidAPI/IFunction.h"
#include "MantidAPI/FunctionDomain1D.h"
#include "MantidAPI/FunctionValues.h"
#include "MantidAPI/MatrixWorkspace.h"
#include "MantidKernel/Exception.h"

#include <qwt_plot_curve.h>

namespace MantidQt
{
namespace CustomInterfaces
{
namespace MDF
{

/// Default size of the function domain
size_t MDFFunctionPlotData::g_defaultDomainSize = 100;

namespace {
  /// Default curve color
  auto FUNCTION_CURVE_COLOR = Qt::magenta;
}
  
/// Constructor.
/// @param fun :: A function to plot.
/// @param startX :: A lower bound of the evaluation interval.
/// @param endX :: An upper bound of the evaluation interval.
/// @param nX :: A number of values to evaluate.
MDFFunctionPlotData::MDFFunctionPlotData(
    boost::shared_ptr<Mantid::API::IFunction> fun, double startX, double endX,
    size_t nX)
    : m_function(fun), m_functionCurve(new QwtPlotCurve()) 
{
  setDomain(startX, endX, nX);
  auto pen = m_functionCurve->pen();
  pen.setColor(FUNCTION_CURVE_COLOR);
  m_functionCurve->setPen(pen);
}

/// Destructor.
MDFFunctionPlotData::~MDFFunctionPlotData()
{
  m_functionCurve->detach();
  delete m_functionCurve;
}

/// Define function's domain and set the data to the curve.
void MDFFunctionPlotData::setDomain(double startX, double endX, size_t nX)
{
  Mantid::API::FunctionDomain1DVector x(startX, endX, nX);
  Mantid::API::FunctionValues y(x);
  m_function->function(x, y);
  m_functionCurve->setData(x.getPointerAt(0), y.getPointerToCalculated(0), static_cast<int>(x.size()));
}

/// Show the curves on a plot.
void MDFFunctionPlotData::show(QwtPlot *plot)
{
  m_functionCurve->attach(plot);
}

/// Hide the curves from any plot.
void MDFFunctionPlotData::hide()
{
  m_functionCurve->detach();
}

/// Get the bounding rect including all plotted data.
QwtDoubleRect MDFFunctionPlotData::boundingRect() const
{
  QwtDoubleRect rect = m_functionCurve->boundingRect();
  return rect;
}

/// Update function parameters and attributes
/// @param fun :: A function to copy attributes and parameters from.
void MDFFunctionPlotData::updateFunction(const Mantid::API::IFunction& fun)
{
  if (!m_function) return;
  if (m_function->nParams() != fun.nParams())
  {
    throw std::logic_error("Cannot update function: different number of parameters.");
  }
  if (m_function->nAttributes() != fun.nAttributes())
  {
    throw std::logic_error("Cannot update function: different number of attributes.");
  }
  // Copy the attributes
  auto attributes = fun.getAttributeNames();
  for(auto attr = attributes.begin(); attr != attributes.end(); ++attr)
  {
    auto value = fun.getAttribute(*attr);
    m_function->setAttribute(*attr, value);
  }
  // Copy the parameters
  for(size_t i = 0; i < fun.nParams(); ++i)
  {
    auto name = fun.parameterName(i);
    auto value = fun.getParameter(i);
    m_function->setParameter(name, value);
  }
}


} // MDF
} // CustomInterfaces
} // MantidQt
