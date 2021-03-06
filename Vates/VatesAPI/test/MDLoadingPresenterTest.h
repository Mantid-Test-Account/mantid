#ifndef VATES_API_MD_LOADING_PRESENTER_TEST_H
#define VATES_API_MD_LOADING_PRESENTER_TEST_H

#include "MantidTestHelpers/MDEventsTestHelper.h"
#include "MantidVatesAPI/MDLoadingPresenter.h"
#include "MantidVatesAPI/NoThresholdRange.h"
#include "MantidVatesAPI/ProgressAction.h"
#include "MantidVatesAPI/vtkDataSetFactory.h"
#include "MantidVatesAPI/vtkMDHexFactory.h"


#include <cxxtest/TestSuite.h>
#include "MockObjects.h"
#include "vtkPVChangeOfBasisHelper.h"
#include "vtkMatrix4x4.h"
#include "vtkUnstructuredGrid.h"
#include <vector>
#include <string>

using namespace Mantid::DataObjects;
using namespace Mantid::VATES;

namespace {
  std::string MOCK_GEOMETRY_XML_MD_LOADING_PRESENTER = "geometry";
  std::string MOCK_INSTRUMENT_MD_LOADING_PRESENTER = "instrument";

class MOCKMDLoadingPresenter : public Mantid::VATES::MDLoadingPresenter {
public:
  MOCKMDLoadingPresenter(){}
  ~MOCKMDLoadingPresenter(){}
  vtkSmartPointer<vtkDataSet> execute(Mantid::VATES::vtkDataSetFactory *,
                                      Mantid::VATES::ProgressAction &,
                                      Mantid::VATES::ProgressAction &) {
    return nullptr;
  }
  void executeLoadMetadata() {}
  bool hasTDimensionAvailable(void) const {return true;}
  std::vector<double> getTimeStepValues() const {return std::vector<double>();}
  std::string getTimeStepLabel() const {return std::string();}
  void setAxisLabels(vtkDataSet* ) {}
  bool canReadFile() const {return true;}
  const std::string& getGeometryXML() const {return MOCK_GEOMETRY_XML_MD_LOADING_PRESENTER;}
  const std::string& getInstrument() {return MOCK_INSTRUMENT_MD_LOADING_PRESENTER;}
};
}


class MDLoadingPresenterTest : public CxxTest::TestSuite {
private:
  vtkSmartPointer<vtkUnstructuredGrid> makeDataSet() {
    FakeProgressAction progressUpdate;
    MDEventWorkspace3Lean::sptr ws =
        MDEventsTestHelper::makeMDEW<3>(8, -10.0, 10.0, 1);
    Mantid::VATES::vtkMDHexFactory factory(ThresholdRange_scptr(new NoThresholdRange),
                            VolumeNormalization);
    factory.initialize(ws);
    auto dataset = factory.create(progressUpdate);
    auto grid = vtkUnstructuredGrid::SafeDownCast(dataset.Get());
    return vtkSmartPointer<vtkUnstructuredGrid>(grid);
  }
public:
  void test_that_non_default_cob_is_created() {
    // Arrange
    MOCKMDLoadingPresenter presenter;
    vtkSmartPointer<vtkUnstructuredGrid> dataSet;
    dataSet = makeDataSet();
    // Act
    presenter.setDefaultCOBandBoundaries(dataSet);
    // Assert
    auto cobMatrix = vtkPVChangeOfBasisHelper::GetChangeOfBasisMatrix(dataSet);
    TS_ASSERT_EQUALS(cobMatrix->Element[0][0], 1);
    TS_ASSERT_EQUALS(cobMatrix->Element[0][1], 0);
    TS_ASSERT_EQUALS(cobMatrix->Element[0][2], 0);
    TS_ASSERT_EQUALS(cobMatrix->Element[0][3], 0);

    TS_ASSERT_EQUALS(cobMatrix->Element[1][0], 0);
    TS_ASSERT_EQUALS(cobMatrix->Element[1][1], 1);
    TS_ASSERT_EQUALS(cobMatrix->Element[1][2], 0);
    TS_ASSERT_EQUALS(cobMatrix->Element[1][3], 0);

    TS_ASSERT_EQUALS(cobMatrix->Element[2][0], 0);
    TS_ASSERT_EQUALS(cobMatrix->Element[2][1], 0);
    TS_ASSERT_EQUALS(cobMatrix->Element[2][2], 1);
    TS_ASSERT_EQUALS(cobMatrix->Element[2][3], 0);

    TS_ASSERT_EQUALS(cobMatrix->Element[3][0], 0);
    TS_ASSERT_EQUALS(cobMatrix->Element[3][1], 0);
    TS_ASSERT_EQUALS(cobMatrix->Element[3][2], 0);
    TS_ASSERT_EQUALS(cobMatrix->Element[3][3], 1);

    // Check the bounding box element for axes
    double bounds[6] = {0, 0, 0, 0, 0, 0};
    vtkPVChangeOfBasisHelper::GetBoundingBoxInBasis(dataSet, bounds);

    TS_ASSERT_EQUALS(-10.0, bounds[0]);
    TS_ASSERT_EQUALS(10.0, bounds[1]);
    TS_ASSERT_EQUALS(-10.0, bounds[2]);
    TS_ASSERT_EQUALS(10.0, bounds[3]);
    TS_ASSERT_EQUALS(-10.0, bounds[4]);
    TS_ASSERT_EQUALS(10.0, bounds[5]);
  }
};

#endif
