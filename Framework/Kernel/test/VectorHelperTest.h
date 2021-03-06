#ifndef MANTID_KERNEL_VECTORHELPERTEST_H_
#define MANTID_KERNEL_VECTORHELPERTEST_H_

#include "MantidKernel/System.h"
#include "MantidKernel/Timer.h"
#include "MantidKernel/VectorHelper.h"
#include <cxxtest/TestSuite.h>
#include <cstdlib>
#include <boost/assign/list_of.hpp>

using namespace Mantid::Kernel;

class VectorHelperTest : public CxxTest::TestSuite {
public:
  // This pair of boilerplate methods prevent the suite being created statically
  // This means the constructor isn't called when running other tests
  static VectorHelperTest *createSuite() { return new VectorHelperTest(); }
  static void destroySuite(VectorHelperTest *suite) { delete suite; }

  VectorHelperTest() : m_test_bins(5, 0.0) {
    m_test_bins[0] = -1.1;
    m_test_bins[1] = -0.2;
    m_test_bins[2] = 0.7;
    m_test_bins[3] = 1.6;
    m_test_bins[4] = 3.2;
  }

  void test_CreateAxisFromRebinParams_Gives_Expected_Number_Bins() {
    std::vector<double> rbParams(3);
    rbParams[0] = 1;
    rbParams[1] = 1;
    rbParams[2] = 10;

    std::vector<double> axis;
    const int numBoundaries =
        VectorHelper::createAxisFromRebinParams(rbParams, axis);

    TS_ASSERT_EQUALS(numBoundaries, 10);
    TS_ASSERT_EQUALS(axis.size(), 10);
  }

  void
  test_CreateAxisFromRebinParams_Gives_Expected_Number_Bins_But_Not_Resized_Axis_When_Requested() {
    std::vector<double> rbParams(3);
    rbParams[0] = 1;
    rbParams[1] = 1;
    rbParams[2] = 10;

    std::vector<double> axis;
    const int numBoundaries =
        VectorHelper::createAxisFromRebinParams(rbParams, axis, false);

    TS_ASSERT_EQUALS(numBoundaries, 10);
    TS_ASSERT_EQUALS(axis.size(), 0);
  }

  void test_CreateAxisFromRebinParams_SingleStep() {
    std::vector<double> rbParams = boost::assign::list_of(0)(2)(5);

    std::vector<double> axis;
    VectorHelper::createAxisFromRebinParams(rbParams, axis, true);

    std::vector<double> expectedAxis = boost::assign::list_of(0)(2)(4)(5);
    TS_ASSERT_EQUALS(axis, expectedAxis);
  }

  void test_CreateAxisFromRebinParams_SingleStep_LastBinTooSmall() {
    std::vector<double> rbParams = boost::assign::list_of(0.0)(2.0)(4.1);

    std::vector<double> axis;
    VectorHelper::createAxisFromRebinParams(rbParams, axis, true);

    std::vector<double> expectedAxis = boost::assign::list_of(0.0)(2.0)(4.1);
    TS_ASSERT_EQUALS(axis, expectedAxis);
  }

  void test_CreateAxisFromRebinParams_MultipleSteps() {
    std::vector<double> rbParams =
        boost::assign::list_of(0)(2)(5)(3)(10)(1)(12);

    std::vector<double> axis;
    VectorHelper::createAxisFromRebinParams(rbParams, axis, true);

    std::vector<double> expectedAxis =
        boost::assign::list_of(0)(2)(4)(5)(8)(10)(11)(12);
    TS_ASSERT_EQUALS(axis, expectedAxis);
  }

  void test_CreateAxisFromRebinParams_FullBinsOnly_SingleStep() {
    std::vector<double> rbParams = boost::assign::list_of(0)(2)(5);

    std::vector<double> axis;
    VectorHelper::createAxisFromRebinParams(rbParams, axis, true, true);

    std::vector<double> expectedAxis = boost::assign::list_of(0)(2)(4);
    TS_ASSERT_EQUALS(axis, expectedAxis);
  }

  void test_CreateAxisFromRebinParams_FullBinsOnly_MultipleSteps() {
    std::vector<double> rbParams =
        boost::assign::list_of(0)(2)(5)(3)(10)(1)(12);

    std::vector<double> axis;
    VectorHelper::createAxisFromRebinParams(rbParams, axis, true, true);

    std::vector<double> expectedAxis =
        boost::assign::list_of(0)(2)(4)(7)(10)(11)(12);
    TS_ASSERT_EQUALS(axis, expectedAxis);
  }

  void test_ConvertToBinBoundary_EmptyInputVector() {
    std::vector<double> bin_centers;
    std::vector<double> bin_edges;
    VectorHelper::convertToBinBoundary(bin_centers, bin_edges);

    TS_ASSERT_EQUALS(bin_edges.size(), 0);
  }

  void test_ConvertToBinBoundary_Size1InputVector() {
    std::vector<double> bin_centers = boost::assign::list_of(0.4);
    std::vector<double> bin_edges;
    VectorHelper::convertToBinBoundary(bin_centers, bin_edges);

    TS_ASSERT_EQUALS(bin_edges.size(), 2);
    // In lack of a better guess for the bin width it is set to 1.0.
    TS_ASSERT_DELTA(bin_edges[0], -0.1, 1e-12);
    TS_ASSERT_DELTA(bin_edges[1], 0.9, 1e-12);
  }

  void test_ConvertToBinBoundary_Size2InputVector() {
    std::vector<double> bin_centers = boost::assign::list_of(0.5)(1.5);
    std::vector<double> bin_edges;

    VectorHelper::convertToBinBoundary(bin_centers, bin_edges);

    TS_ASSERT_EQUALS(bin_edges.size(), 3);
    TS_ASSERT_DELTA(bin_edges[0], 0.0, 1e-12);
    TS_ASSERT_DELTA(bin_edges[1], 1.0, 1e-12);
    TS_ASSERT_DELTA(bin_edges[2], 2.0, 1e-12);
  }

  // TODO: More tests of other methods

  void test_splitStringIntoVector() {
    std::vector<int> vec =
        VectorHelper::splitStringIntoVector<int>("1,2,-5,23");
    TS_ASSERT_EQUALS(vec.size(), 4);
    TS_ASSERT_EQUALS(vec[0], 1);
    TS_ASSERT_EQUALS(vec[1], 2);
    TS_ASSERT_EQUALS(vec[2], -5);
    TS_ASSERT_EQUALS(vec[3], 23);
  }

  void test_splitStringIntoVector_empty() {
    std::vector<int> vec = VectorHelper::splitStringIntoVector<int>("");
    TS_ASSERT_EQUALS(vec.size(), 0);
    vec = VectorHelper::splitStringIntoVector<int>(",   ,  ,");
    TS_ASSERT_EQUALS(vec.size(), 0);
  }

  void test_splitStringIntoVector_double() {
    std::vector<double> vec =
        VectorHelper::splitStringIntoVector<double>("1.234, 2.456");
    TS_ASSERT_EQUALS(vec.size(), 2);
    TS_ASSERT_DELTA(vec[0], 1.234, 1e-5);
    TS_ASSERT_DELTA(vec[1], 2.456, 1e-5);
  }

  void test_splitStringIntoVector_string() {
    std::vector<std::string> vec =
        VectorHelper::splitStringIntoVector<std::string>("Hey, Jude");
    TS_ASSERT_EQUALS(vec.size(), 2);
    TS_ASSERT_EQUALS(vec[0], "Hey");
    TS_ASSERT_EQUALS(vec[1], "Jude");
  }

  void test_normalizeVector_and_length() {
    std::vector<double> x;
    std::vector<double> y;
    TS_ASSERT_DELTA(VectorHelper::lengthVector(x), 0.0, 1e-5);

    y = VectorHelper::normalizeVector(x);
    TSM_ASSERT_EQUALS("Pass-through empty vectors", y.size(), 0);
    x.push_back(3.0);
    x.push_back(4.0);
    TS_ASSERT_DELTA(VectorHelper::lengthVector(x), 5.0, 1e-5);
    y = VectorHelper::normalizeVector(x);
    TS_ASSERT_EQUALS(y.size(), 2);
    TS_ASSERT_DELTA(y[0], 0.6, 1e-5);
    TS_ASSERT_DELTA(y[1], 0.8, 1e-5);

    // Handle 0-length
    x[0] = 0.0;
    x[1] = 0.0;
    TS_ASSERT_DELTA(VectorHelper::lengthVector(x), 0.0, 1e-5);
    y = VectorHelper::normalizeVector(x);
    TS_ASSERT_EQUALS(y.size(), 2);
  }

  // TODO: Figure out proper behavior if given stupidity as inputs
  //  void test_splitStringIntoVector_badNumber_gives0()
  //  {
  //    std::vector<int> vec;
  //    vec = VectorHelper::splitStringIntoVector<int>("2, monkey, potato,
  //    134");
  //    TS_ASSERT_EQUALS( vec.size(), 4);
  //    TS_ASSERT_EQUALS( vec[0], 2);
  //    TS_ASSERT_EQUALS( vec[1], 0);
  //    TS_ASSERT_EQUALS( vec[2], 0);
  //    TS_ASSERT_EQUALS( vec[3], 134);
  //  }

  void test_getBinIndex_Returns_Zero_For_Value_Lower_Than_Input_Range() {
    const double testValue = m_test_bins.front() - 1.1;
    int index(-1);

    TS_ASSERT_THROWS_NOTHING(
        index = VectorHelper::getBinIndex(m_test_bins, testValue));
    TS_ASSERT_EQUALS(index, 0);
  }

  void
  test_getBinIndex_Returns_Zero_For_Value_Equal_To_Lowest_In_Input_Range() {
    const double testValue = m_test_bins.front();
    int index(-1);

    TS_ASSERT_THROWS_NOTHING(
        index = VectorHelper::getBinIndex(m_test_bins, testValue));
    TS_ASSERT_EQUALS(index, 0);
  }

  void
  test_getBinIndex_Returns_Last_Bin_For_Value_Equal_To_Highest_In_Input_Range() {
    const double testValue = m_test_bins.back();
    int index(-1);

    TS_ASSERT_THROWS_NOTHING(
        index = VectorHelper::getBinIndex(m_test_bins, testValue));
    TS_ASSERT_EQUALS(index, 3);
  }

  void
  test_getBinIndex_Returns_Index_Of_Last_Bin_For_Value_Greater_Than_Input_Range() {
    const double testValue = m_test_bins.back() + 10.1;
    int index(-1);

    TS_ASSERT_THROWS_NOTHING(
        index = VectorHelper::getBinIndex(m_test_bins, testValue));
    TS_ASSERT_EQUALS(index, 3);
  }

  void test_getBinIndex_Returns_Correct_Bins_Index_For_Value_Not_On_Edge() {
    const double testValue = m_test_bins[1] + 0.3;
    int index(-1);

    TS_ASSERT_THROWS_NOTHING(
        index = VectorHelper::getBinIndex(m_test_bins, testValue));
    TS_ASSERT_EQUALS(index, 1);
  }

  void
  test_getBinIndex_Returns_Index_For_Bin_On_RHS_Of_Boundary_When_Given_Value_Is_Equal_To_A_Boundary() {
    const double testValue = m_test_bins[2];
    int index(-1);

    TS_ASSERT_THROWS_NOTHING(
        index = VectorHelper::getBinIndex(m_test_bins, testValue));
    TS_ASSERT_EQUALS(index, 2);
  }
  void test_RunningAveraging() {
    double id[] = {1, 2, 3, 4, 5, 6};
    std::vector<double> inputData(id, id + sizeof(id) / sizeof(double));
    double ib[] = {0, 1, 2, 3, 4, 5};
    std::vector<double> inputBoundaries(ib, ib + sizeof(ib) / sizeof(double));

    std::vector<double> output;
    TS_ASSERT_THROWS(
        VectorHelper::smoothInRange(inputData, output, 6, &inputBoundaries),
        std::invalid_argument);
    inputBoundaries.push_back(6);
    VectorHelper::smoothInRange(inputData, output, 6, &inputBoundaries);

    TS_ASSERT_DELTA(output[1] - output[0], 0.492, 1.e-3);
    TS_ASSERT_DELTA(output[3] - output[2], 0.4545, 1.e-3);
    TS_ASSERT_DELTA(output[5] - output[4], 0.492, 1.e-3);
    inputBoundaries[1] = 1;
    inputBoundaries[2] = 3;
    inputBoundaries[3] = 6;
    inputBoundaries[4] = 10;
    inputBoundaries[5] = 15;
    inputBoundaries[6] = 21;
    VectorHelper::smoothInRange(inputData, output, 6, &inputBoundaries);
    TS_ASSERT_DELTA(output[2], 3, 1.e-8);
    TS_ASSERT_DELTA(output[0], 1, 1.e-8);
    TS_ASSERT_DELTA(output[5], 6, 1.e-8);

    std::vector<double> out_bins;
    VectorHelper::smoothInRange(inputData, output, 3, &inputBoundaries, 1, 5,
                                &out_bins);
    TS_ASSERT_EQUALS(output.size(), 4);
    TS_ASSERT_DELTA(output[1], 3, 1.e-8);
  }

  void test_Smooth_keeps_peakPosition() {

    std::vector<double> output;
    std::vector<double> inputBoundaries(21);
    inputBoundaries[0] = 0;
    double step(1);
    for (size_t i = 1; i < 21; i++) {
      inputBoundaries[i] = inputBoundaries[i - 1] + step;
      step *= 1.1;
    }
    double norm = 100 / inputBoundaries[20];
    for (size_t i = 0; i < 21; i++) {
      inputBoundaries[i] *= norm;
    }

    std::vector<double> inputData(20);
    for (size_t i = 0; i < 20; i++) {
      double dev = 0.5 * (inputBoundaries[i] + inputBoundaries[i + 1]) - 50;
      inputData[i] =
          exp(-dev * dev / 100) * (inputBoundaries[i + 1] - inputBoundaries[i]);
    }
    int indOfMax = VectorHelper::getBinIndex(inputBoundaries, 50.);
    double fMax = inputData[indOfMax] /
                  (inputBoundaries[indOfMax + 1] - inputBoundaries[indOfMax]);
    double iLeft = inputData[indOfMax - 1] /
                   (inputBoundaries[indOfMax] - inputBoundaries[indOfMax - 1]);
    double iRight = inputData[indOfMax + 1] / (inputBoundaries[indOfMax + 2] -
                                               inputBoundaries[indOfMax + 1]);

    TS_ASSERT(iLeft < fMax);
    TS_ASSERT(iRight < fMax);
    VectorHelper::smoothInRange(inputData, output, 10, &inputBoundaries);
    fMax = output[indOfMax] /
           (inputBoundaries[indOfMax + 1] - inputBoundaries[indOfMax]);
    iLeft = inputData[indOfMax - 1] /
            (inputBoundaries[indOfMax] - inputBoundaries[indOfMax - 1]);
    iRight = inputData[indOfMax + 1] /
             (inputBoundaries[indOfMax + 2] - inputBoundaries[indOfMax + 1]);

    TS_ASSERT(iLeft < fMax);
    TS_ASSERT(iRight < fMax);

    output.swap(inputData);
    VectorHelper::smoothInRange(inputData, output, 10, &inputBoundaries);

    fMax = output[indOfMax] /
           (inputBoundaries[indOfMax + 1] - inputBoundaries[indOfMax]);
    iLeft = inputData[indOfMax - 1] /
            (inputBoundaries[indOfMax] - inputBoundaries[indOfMax - 1]);
    iRight = inputData[indOfMax + 1] /
             (inputBoundaries[indOfMax + 2] - inputBoundaries[indOfMax + 1]);

    //  TS_ASSERT(iLeft<fMax);
    TS_ASSERT(iRight < fMax);

    output.swap(inputData);
    VectorHelper::smoothInRange(inputData, output, 10, &inputBoundaries);

    fMax = output[indOfMax] /
           (inputBoundaries[indOfMax + 1] - inputBoundaries[indOfMax]);
    iLeft = inputData[indOfMax - 1] /
            (inputBoundaries[indOfMax] - inputBoundaries[indOfMax - 1]);
    iRight = inputData[indOfMax + 1] /
             (inputBoundaries[indOfMax + 2] - inputBoundaries[indOfMax + 1]);

    // TS_ASSERT(iLeft<fMax);
    TS_ASSERT(iRight < fMax);

    output.swap(inputData);
    VectorHelper::smoothInRange(inputData, output, 10, &inputBoundaries);

    fMax = output[indOfMax] /
           (inputBoundaries[indOfMax + 1] - inputBoundaries[indOfMax]);
    iLeft = inputData[indOfMax - 1] /
            (inputBoundaries[indOfMax] - inputBoundaries[indOfMax - 1]);
    iRight = inputData[indOfMax + 1] /
             (inputBoundaries[indOfMax + 2] - inputBoundaries[indOfMax + 1]);

    TS_ASSERT(inputData[indOfMax - 1] < output[indOfMax]);
    TS_ASSERT(inputData[indOfMax + 1] < output[indOfMax]);
  }

private:
  /// Testing bins
  std::vector<double> m_test_bins;
};

#endif /* MANTID_KERNEL_VECTORHELPERTEST_H_ */
