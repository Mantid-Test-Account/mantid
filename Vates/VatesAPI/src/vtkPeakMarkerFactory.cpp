#include "MantidVatesAPI/vtkPeakMarkerFactory.h"
#include "MantidVatesAPI/ProgressAction.h"
#include "MantidVatesAPI/vtkEllipsoidTransformer.h"
#include <boost/math/special_functions/fpclassify.hpp>
#include "MantidAPI/Workspace.h"
#include "MantidAPI/IPeaksWorkspace.h"
#include "MantidGeometry/Crystal/IPeak.h"
#include "MantidGeometry/Crystal/PeakShape.h"
#include "MantidDataObjects/PeakShapeSpherical.h"
#include "MantidDataObjects/PeakShapeEllipsoid.h"
#include "MantidKernel/V3D.h"
#include "MantidKernel/ReadLock.h"

#include <vtkAxes.h>
#include "vtkParametricEllipsoid.h"
#include "vtkParametricFunctionSource.h"
#include <vtkPolyDataAlgorithm.h>
#include <vtkAppendPolyData.h>
#include <vtkVertex.h>
#include "MantidVatesAPI/vtkGlyph3D_Silent.h"
#include <vtkSphereSource.h>
#include <vtkUnstructuredGrid.h>
#include <vtkFloatArray.h>
#include <vtkCellData.h>
#include <vtkPolyData.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkPVGlyphFilter.h>
#include <vtkRegularPolygonSource.h>
#include <vtkSmartPointer.h>
#include <vtkNew.h>

#include <vtkLineSource.h>
#include <cmath>

using Mantid::API::IPeaksWorkspace;
using Mantid::Geometry::IPeak;
using Mantid::Kernel::V3D;

namespace Mantid
{
namespace VATES 
{

  vtkPeakMarkerFactory::vtkPeakMarkerFactory(const std::string& scalarName, ePeakDimensions dimensions) :
  m_scalarName(scalarName), m_dimensionToShow(dimensions), m_peakRadius(-1)
  {
  }

  /**
  Assigment operator
  @param other : vtkPeakMarkerFactory to assign to this instance from.
  @return ref to assigned current instance.
  */
  vtkPeakMarkerFactory& vtkPeakMarkerFactory::operator=(const vtkPeakMarkerFactory& other)
  {
    if(this != &other)
    {
      this->m_scalarName = other.m_scalarName;
      this->m_dimensionToShow = other.m_dimensionToShow;
      this->m_workspace = other.m_workspace;
    }
    return *this;
  }

  /**
  Copy Constructor
  @param other : instance to copy from.
  */
  vtkPeakMarkerFactory::vtkPeakMarkerFactory(const vtkPeakMarkerFactory& other)
  {
   this->m_scalarName = other.m_scalarName;
   this->m_dimensionToShow = other.m_dimensionToShow;
   this->m_workspace = other.m_workspace;
   this->m_peakRadius = other.m_peakRadius;
  }

  void vtkPeakMarkerFactory::initialize(Mantid::API::Workspace_sptr workspace)
  {
    m_workspace = boost::dynamic_pointer_cast<IPeaksWorkspace>(workspace);
    validateWsNotNull();

    try
    {
      m_peakRadius = atof(m_workspace->run().getProperty("PeakRadius")->value().c_str());
    }
    catch(Mantid::Kernel::Exception::NotFoundError&)
    {
    }
  }

  double vtkPeakMarkerFactory::getIntegrationRadius() const
  {
    return m_peakRadius;
  }

  bool vtkPeakMarkerFactory::isPeaksWorkspaceIntegrated() const
  {
    return (m_peakRadius > 0);
  }

  void vtkPeakMarkerFactory::validateWsNotNull() const
  {
    if(!m_workspace)
      throw std::runtime_error("IPeaksWorkspace is null");
  }

  void vtkPeakMarkerFactory::validate() const
  {
    validateWsNotNull();
  }

  vtkSmartPointer<vtkRegularPolygonSource> getThreeCircles(float radius,
                                                           int numberOfSizes) {
    auto circleX = vtkSmartPointer<vtkRegularPolygonSource>::New();
    circleX->GeneratePolygonOff(); // Only want the outline of the circle
    circleX->SetNumberOfSides(numberOfSizes);
    circleX->SetRadius(radius);
    circleX->SetCenter(0, 0, 0);
    return circleX;
  }

    /**
  Create the vtkStructuredGrid from the provided workspace
  @param progressUpdating: Reporting object to pass progress information up the stack.
  @return vtkPolyData glyph.
  */
  vtkPolyData* vtkPeakMarkerFactory::create(ProgressAction& progressUpdating) const
  {
    validate();

    int numPeaks = m_workspace->getNumberPeaks();

   // Acquire a scoped read-only lock to the workspace (prevent segfault from algos modifying ws)
    Mantid::Kernel::ReadLock lock(*m_workspace);

    vtkEllipsoidTransformer ellipsoidTransformer;

    const int resolution = 8;
    double progressFactor = 1.0/double(numPeaks);

    // Point
    vtkPoints *peakPoint = vtkPoints::New();
    peakPoint->Allocate(numPeaks);

    vtkFloatArray *peakSignal = vtkFloatArray::New();
    peakSignal->Allocate(numPeaks);
    peakSignal->SetName(m_scalarName.c_str());
    peakSignal->SetNumberOfComponents(1);

    vtkFloatArray *radiusSignal = vtkFloatArray::New();
    radiusSignal->Allocate(numPeaks);
    radiusSignal->SetName("RADIUS");
    radiusSignal->SetNumberOfComponents(1);

    // What we'll return
    vtkUnstructuredGrid *peakDataSet = vtkUnstructuredGrid::New();
    peakDataSet->Allocate(numPeaks);
    peakDataSet->SetPoints(peakPoint);
    peakDataSet->GetCellData()->SetScalars(peakSignal);
    peakDataSet->GetCellData()->SetScalars(radiusSignal);

    // Go peak-by-peak
    for (int i=0; i < numPeaks; i++)
    {
      progressUpdating.eventRaised(double(i)*progressFactor);

      IPeak & peak = m_workspace->getPeak(i);

      // Choose the dimensionality of the position to show
      V3D pos;
      switch (m_dimensionToShow)
      {
      case Peak_in_Q_lab:
        pos = peak.getQLabFrame();
        break;
      case Peak_in_Q_sample:
        pos = peak.getQSampleFrame();
        break;
      case Peak_in_HKL:
        pos = peak.getHKL();
        break;
      default:
        pos = peak.getQLabFrame();
      }

      double x = pos.X();
      double y = pos.Y();
      double z = pos.Z();

      // One point per peak
      vtkVertex * vertex = vtkVertex::New();
      vtkIdType id_xyz = peakPoint->InsertNextPoint(x,y,z);
      vertex->GetPointIds()->SetId(0, id_xyz);

      peakDataSet->InsertNextCell(VTK_VERTEX, vertex->GetPointIds());

      // The integrated intensity = the signal on that point.
      peakSignal->InsertNextValue(static_cast<float>( peak.getIntensity() ));

      // Add a glyph and append to the appendFilter
      const Mantid::Geometry::PeakShape& shape = m_workspace->getPeakPtr(i)->getPeakShape();

      // Pick the radius up from the factory if possible, otherwise use the user-provided value.
      if(shape.shapeName() == Mantid::DataObjects::PeakShapeSpherical::sphereShapeName())
      {
        const Mantid::DataObjects::PeakShapeSpherical& sphericalShape = dynamic_cast<const Mantid::DataObjects::PeakShapeSpherical&>(shape);
        radiusSignal->InsertNextValue(
            static_cast<float>(sphericalShape.radius()));
      } else {
        throw std::runtime_error("not implemented yet!");
      }

    } // for each peak

    auto circX = getThreeCircles(1.0, resolution);
    circX->SetNormal(1, 0, 0);
    auto circY = getThreeCircles(1.0, resolution);
    circY->SetNormal(0, 1, 0);
    auto circZ = getThreeCircles(1.0, resolution);
    circZ->SetNormal(0, 0, 1);

    vtkNew<vtkPVGlyphFilter> glyphFilter1;
    glyphFilter1->SetInputData(peakDataSet);
    glyphFilter1->SetSourceConnection(0, circX->GetOutputPort());
    glyphFilter1->SetScaleModeToScaleByVector();
    glyphFilter1->SetInputArrayToProcess(
        1, 0, 0, vtkDataObject::FIELD_ASSOCIATION_CELLS, "RADIUS");
    glyphFilter1->Update();

    vtkNew<vtkPVGlyphFilter> glyphFilter2;
    glyphFilter2->SetInputData(peakDataSet);
    glyphFilter2->SetSourceConnection(0, circY->GetOutputPort());
    glyphFilter2->SetScaleModeToScaleByVector();
    glyphFilter2->SetInputArrayToProcess(
        1, 0, 0, vtkDataObject::FIELD_ASSOCIATION_CELLS, "RADIUS");
    glyphFilter2->Update();

    vtkNew<vtkPVGlyphFilter> glyphFilter3;
    glyphFilter3->SetInputData(peakDataSet);
    glyphFilter3->SetSourceConnection(0, circZ->GetOutputPort());
    glyphFilter3->SetScaleModeToScaleByVector();
    glyphFilter3->SetInputArrayToProcess(
        1, 0, 0, vtkDataObject::FIELD_ASSOCIATION_CELLS, "RADIUS");
    glyphFilter3->Update();

    vtkAppendPolyData *appendFilter = vtkAppendPolyData::New();
    appendFilter->AddInputData(glyphFilter1->GetOutput());
    appendFilter->AddInputData(glyphFilter2->GetOutput());
    appendFilter->AddInputData(glyphFilter3->GetOutput());
    appendFilter->Update();
    return appendFilter->GetOutput();
  }

  vtkPeakMarkerFactory::~vtkPeakMarkerFactory()
  {
  }
}
}
