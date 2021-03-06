#ifndef VTK_DATASET_TO_WS_IMPLICITFUNCTION_TEST
#define VTK_DATASET_TO_WS_IMPLICITFUNCTION_TEST 

#include <cxxtest/TestSuite.h>
#include "MantidVatesAPI/vtkDataSetToImplicitFunction.h"
#include "MantidGeometry/MDGeometry/MDImplicitFunction.h"
#include "MockObjects.h"
#include <vtkDataSet.h>
#include "MantidVatesAPI/vtkStructuredGrid_Silent.h"
#include <vtkNew.h>

using namespace Mantid::VATES;


//=====================================================================================
// Functional tests
//=====================================================================================
class vtkDataSetToImplicitFunctionTest : public CxxTest::TestSuite
{

public:

  void testThrowIfvtkDataSetNull()
  {
    vtkDataSet* nullArg = NULL;
    TS_ASSERT_THROWS(vtkDataSetToImplicitFunction temp(nullArg), std::runtime_error);
  }

  void testNoImplcitFunction() {
    vtkNew<vtkStructuredGrid> ds;
    ds->SetFieldData(createFieldDataWithCharArray("<MDInstruction/>"));

    vtkDataSetToImplicitFunction extractor(ds.GetPointer());
    std::unique_ptr<Mantid::Geometry::MDImplicitFunction> func;
    TS_ASSERT_THROWS_NOTHING(
        func = std::unique_ptr<Mantid::Geometry::MDImplicitFunction>(
            extractor.execute()));
    TS_ASSERT_EQUALS("NullImplicitFunction", func->getName());
  }

  void testStaticUsage() {
    vtkNew<vtkStructuredGrid> ds;
    ds->SetFieldData(createFieldDataWithCharArray("<MDInstruction/>"));

    std::unique_ptr<Mantid::Geometry::MDImplicitFunction> func;
    TS_ASSERT_THROWS_NOTHING(
        func = std::unique_ptr<Mantid::Geometry::MDImplicitFunction>(
            vtkDataSetToImplicitFunction::exec(ds.GetPointer())));
    TS_ASSERT_EQUALS("NullImplicitFunction", func->getName());
  }
};

#endif
