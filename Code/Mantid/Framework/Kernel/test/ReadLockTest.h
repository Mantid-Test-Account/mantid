#ifndef MANTID_KERNEL_READLOCKTEST_H_
#define MANTID_KERNEL_READLOCKTEST_H_

#include <cxxtest/TestSuite.h>
#include "MantidKernel/Timer.h"
#include "MantidKernel/System.h"
#include <iostream>
#include <iomanip>

#include "MantidKernel/ReadLock.h"

using namespace Mantid;
using namespace Mantid::Kernel;

//class MockDataItem : public DataItem
//{
//public:
//  virtual const std::string id() const { return "MockDataItem"; }
//  /// The name of the object
//  virtual const std::string name() const{ return "Noone"; }
//  /// Can this object be accessed from multiple threads safely
//  virtual bool threadSafe() const { return true; }
//  /// Serializes the object to a string
//  virtual std::string toString() const { return "Nothing"; }
//
//  Poco::RWLock * getLock()
//  { return m_lock; }
//};



class ReadLockTest : public CxxTest::TestSuite
{
public:

  void test_Something()
  {
  }


};


#endif /* MANTID_KERNEL_READLOCKTEST_H_ */
