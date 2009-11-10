#ifndef GETE_ITEST_H_
#define GETE_ITEST_H_

#include <cxxtest/TestSuite.h>
#include "MantidAlgorithms/GetEi.h"
#include "MantidDataHandling/LoadRaw3.h"
#include "MantidAPI/WorkspaceProperty.h"
#include "MantidDataObjects/Workspace2D.h"
#include "Poco/Path.h"
#include <algorithm>
#include <vector>
#include <iostream>

using namespace Mantid::Kernel;
using namespace Mantid::API;
using namespace Mantid::Geometry;
using namespace Mantid::DataObjects;
using namespace Mantid::DataHandling;
using namespace Mantid::Algorithms;

class GetEiTest : public CxxTest::TestSuite
{
public:
  void testOnMARI()
  {
    GetEi grouper;

    TS_ASSERT_EQUALS( grouper.name(), "GetEi" )
    TS_ASSERT_EQUALS( grouper.version(), 1 )
    TS_ASSERT_EQUALS( grouper.category(), "CorrectionFunctions" )
    TS_ASSERT_THROWS_NOTHING( grouper.initialize() )
    TS_ASSERT( grouper.isInitialized() )
    
    // the following is for a MARI workspace that is loaded in the constructor and the values I found from experimenting
    grouper.setPropertyValue("InputWorkspace", m_WS);
    grouper.setPropertyValue("Monitor1ID", "2");
    grouper.setPropertyValue("Monitor2ID", "3");
    grouper.setPropertyValue("EnergyEstimate", "12");

    //MAPS one off test, takes to long to do it every time. To activate uncomment the code below and two sections below that
  // when the MAP workspace that is load in the constructor
/*    grouper.setPropertyValue("InputWorkspace", m_WS);
    grouper.setPropertyValue("Monitor1ID", "1000002");
    grouper.setPropertyValue("Monitor2ID", "1000003");
    grouper.setPropertyValue("EnergyEstimate", "400");*/

    TS_ASSERT_THROWS_NOTHING( grouper.execute());
    TS_ASSERT( grouper.isExecuted() );

    double finalAnswer = grouper.getProperty("IncidentEnergy");
    TS_ASSERT_DELTA( finalAnswer, 12.9462875, 1e-6 )
/*MAPS one off test, takes to long to do it every time. */ //TS_ASSERT_DELTA( finalAnswer, 398.7392468, 1e-6 )
  }

  void loadRawFile()
  {
    LoadRaw3 loader;
    loader.initialize();

    loader.setPropertyValue("Filename", MAR11001);
    loader.setPropertyValue("OutputWorkspace", m_WS);

    TS_ASSERT_THROWS_NOTHING(loader.execute());
  }

  GetEiTest() : m_WS("GetEi_input_workspace"), MAR11001()
  {
    MAR11001 = Poco::Path(Poco::Path::current()).resolve("../../../../Test/Data/MAR11001.RAW").toString();
/*MAPS one off test, takes to long to do it every time. */   //    MAR11001 = Poco::Path(Poco::Path::current()).resolve("../../../../Test/Data/MAP10266.RAW").toString();
    loadRawFile();
  }
  
  ~GetEiTest()
  {
    AnalysisDataService::Instance().remove(m_WS);
  }

  private:
    const std::string m_WS;
    std::string MAR11001;
};

#endif /*GETE_ITEST_H_*/
