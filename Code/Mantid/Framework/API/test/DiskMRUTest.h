#ifndef MANTID_API_DISKMRUTEST_H_
#define MANTID_API_DISKMRUTEST_H_


#include "MantidAPI/DiskMRU.h"
#include "MantidAPI/ISaveable.h"
#include "MantidKernel/CPUTimer.h"
#include "MantidKernel/System.h"
#include "MantidKernel/Timer.h"
#include <cxxtest/TestSuite.h>
#include <iomanip>
#include <iostream>

using namespace Mantid;
using namespace Mantid::API;
using namespace Mantid::Kernel;
using Mantid::Kernel::CPUTimer;

class DiskMRUTest : public CxxTest::TestSuite
{
public:

//  void xtest_to_compare_to_stl()
//  {
//    size_t num = 1000000;
//    std::vector<ISaveable*> data;
//    for (size_t i=0; i<num; i++)
//    {
//      data.push_back( new ISaveable(i) );
//    }
//
//    DiskMRU::item_list list;
//    CPUTimer tim;
//    for (size_t i=0; i<num; i++)
//    {
//      std::pair<DiskMRU::item_list::iterator,bool> p;
//      p = list.push_front(data[i]);
//    }
//    std::cout << tim << " to fill the list." << std::endl;
//
//    std::set<ISaveable*> mySet;
//    for (size_t i=0; i<num; i++)
//    {
//      mySet.insert(data[i]);
//    }
//    std::cout << tim << " to fill a set[*]." << std::endl;
//
//    std::map<size_t, ISaveable*> myMap;
//    for (size_t i=0; i<num; i++)
//    {
//      ISaveable * s = data[i];
//      myMap[s->getId()] = s;
//    }
//    std::cout << tim << " to fill a map[size_t, *]." << std::endl;
//  }

  void test_1()
  {
  }
};


#endif /* MANTID_API_DISKMRUTEST_H_ */

