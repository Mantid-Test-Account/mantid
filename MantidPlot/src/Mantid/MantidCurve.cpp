#include "MantidCurve.h"

#include <qpainter.h>
#include <qwt_symbol.h>

#include "MantidAPI/AnalysisDataService.h"

#include "../Graph.h"
#include "../ApplicationWindow.h"
#include "../MultiLayer.h"
#include "ErrorBarSettings.h"
#include "MantidKernel/CPUTimer.h"

using Mantid::Kernel::CPUTimer;

/**
Constructor
@param wsName : Name of the workspace
@param err : flag indicating that all error bars should be used.
@param allError : flag indicating that all error bars should be plotted.
*/
MantidCurve::MantidCurve(const QString& wsName, bool err, bool allError)
 : PlotCurve(wsName), WorkspaceObserver(),
   m_drawErrorBars(err), m_drawAllErrorBars(allError), m_errorSettings(new ErrorBarSettings(this))
{
}

/**
Constructor 
@param err : flag indicating that errors should be used.
*/
MantidCurve::MantidCurve(bool err)
 : PlotCurve(), WorkspaceObserver(), 
   m_drawErrorBars(err), m_drawAllErrorBars(false), m_errorSettings(new ErrorBarSettings(this))
{
}

/**
Helper method to apply a chosen style.
@param style : The chosen graph type style
@param ml : pointer to multilayer object
@param lineWidth: ref to linewidth, which may be internally adjusted
*/
void MantidCurve::applyStyleChoice(Graph::CurveType style, MultiLayer* ml, int& lineWidth)
{

  if ( style == Graph::Unspecified )
    style = static_cast<Graph::CurveType>(ml->applicationWindow()->defaultCurveStyle);

  QwtPlotCurve::CurveStyle qwtStyle;
  const int symbolSize = ml->applicationWindow()->defaultSymbolSize;
  const QwtSymbol symbol(QwtSymbol::Ellipse, QBrush(Qt::black),QPen(),QSize(symbolSize,symbolSize));
  switch(style)
  {
  case Graph::Line :
    qwtStyle = QwtPlotCurve::Lines;
    break;
  case Graph::Scatter:
    qwtStyle = QwtPlotCurve::NoCurve;
    this->setSymbol(symbol);
    break;
  case Graph::LineSymbols :
    qwtStyle = QwtPlotCurve::Lines;
    this->setSymbol(symbol);
    break;
  case 15:
    qwtStyle = QwtPlotCurve::Steps;
    break;  // should be Graph::HorizontalSteps but it doesn't work
  default:
    qwtStyle = QwtPlotCurve::Lines;
    break;
  }
  setStyle(qwtStyle);
  lineWidth = static_cast<int>(floor(ml->applicationWindow()->defaultCurveLineWidth));
}

/**
Rebuild the bounding rectangle. Uses the mantidData (QwtMantidWorkspaceData) object to do so.
*/
QwtDoubleRect MantidCurve::boundingRect() const
{
  if (m_boundingRect.isNull())
  {
    const QwtData* data = mantidData();
    if (data->size() == 0) return QwtDoubleRect(0,0,1,1);
    double y_min = std::numeric_limits<double>::infinity();
    double y_max = -y_min;
    for(size_t i=0;i<data->size();++i)
    {
      double y = data->y(i);
      if (y == std::numeric_limits<double>::infinity() || y != y) continue;
      if (y < y_min && (!mantidData()->logScale() || y > 0.)) y_min = y;
      if (y > y_max) y_max = y;
    }
    double x_min = data->x(0);
    double x_max = data->x(data->size()-1);
    if (x_min > x_max)
    {
      std::swap(x_min, x_max);
    }
    m_boundingRect = QwtDoubleRect(x_min,y_min,x_max-x_min,y_max-y_min);
  }
  return m_boundingRect;
}

/**
Slot for axis scale changed. Invalidate and rebuild the bounding rectangle.
@param axis: axis index.
@param toLog: true if switching to a log value
*/
void MantidCurve::axisScaleChanged(int axis, bool toLog)
{
  if (axis == QwtPlot::yLeft || axis == QwtPlot::yRight)
  {
    mantidData()->setLogScale(toLog);
    // force boundingRect calculation at this moment
    invalidateBoundingRect();
    boundingRect();
    mantidData()->saveLowestPositiveValue(m_boundingRect.y());
  }
}

/// Destructor
MantidCurve::~MantidCurve()
{
}


void MantidCurve::itemChanged()
{
  //Forward request onwards
  PlotCurve::itemChanged();
}

QList<ErrorBarSettings*> MantidCurve::errorBarSettingsList() const
{
  QList<ErrorBarSettings*> retval;
  retval.append(m_errorSettings);
  return retval;
}

/** Create the name for a curve which is a copy of another curve.
 *  @param curveName :: The original curve name.
 */
QString MantidCurve::createCopyName(const QString& curveName)
{
  int i = curveName.lastIndexOf(" (copy");
  if (i < 0) return curveName+" (copy)";
  int j = curveName.lastIndexOf(")");
  if (j == i + 5) return curveName.mid(0,i)+" (copy2)";
  int k = curveName.mid(i+5,j-i-5).toInt();
  return curveName.mid(0,i)+" (copy"+QString::number(k+1)+")";
}


void MantidCurve::doDraw(QPainter *p, 
          const QwtScaleMap &xMap, const QwtScaleMap &yMap,
          const QRect&, MantidQwtWorkspaceData const * const d) const
{
  int sh = 0;
  if (symbol().style() != QwtSymbol::NoSymbol)
  {
    sh = symbol().size().height() / 2;
  }

  int xi0 = 0;
  QPen mypen(pen());
  if ( ! m_errorSettings->m_defaultColor)
    mypen.setColor(m_errorSettings->color());
  mypen.setWidthF(m_errorSettings->width());
  p->setPen(mypen);
  const int dx = m_errorSettings->capLength()/2;
  const int dx2 = 2*dx;
  const int x1 = static_cast<int>(floor(xMap.p1()));
  const int x2 = static_cast<int>(floor(xMap.p2()));
  
  const int skipPoints = skipSymbolsCount();
  for (int i = 0; i < static_cast<int>(d->esize()); i += skipPoints)
  {
    const int xi = xMap.transform(d->ex(i));
    if (m_drawAllErrorBars || (xi > x1 && xi < x2 && (i == 0 || abs(xi - xi0) > dx2)))
    {
      const double Y = y(i);
      const double E = d->e(i);
      const int yi = yMap.transform(Y);
      const int ei1 = yMap.transform(Y - E);
      const int ei2 = yMap.transform(Y + E);
      const int yhl = yi - sh;
      const int ylh = yi + sh;


      if ( m_errorSettings->minusSide() )
      {
        // This call can crash MantidPlot if the error is zero,
        //   so protect against this (line of zero length anyway)
        if (E != 0.0)
          p->drawLine(xi, ei1, xi, ylh);
        p->drawLine(xi-dx,ei1,xi+dx,ei1);
      }
      if ( m_errorSettings->plusSide() )
      {
        // This call can crash MantidPlot if the error is zero,
        //   so protect against this (line of zero length anyway)
        if (E != 0.0)
          p->drawLine(xi, yhl, xi, ei2);
        p->drawLine(xi-dx,ei2,xi+dx,ei2);
      }
      if ( m_errorSettings->throughSymbol() )
      {
        if (E != 0.0)
          p->drawLine(xi, yhl, xi, ylh);
      }
      
      xi0 = xi;
    }
  }
}
