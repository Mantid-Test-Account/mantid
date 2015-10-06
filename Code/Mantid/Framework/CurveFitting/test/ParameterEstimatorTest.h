#ifndef MANTID_CURVEFITTING_PARAMETERESTIMATORTEST_H_
#define MANTID_CURVEFITTING_PARAMETERESTIMATORTEST_H_

#include <cxxtest/TestSuite.h>

#include "MantidCurveFitting/ParameterEstimator.h"
#include "MantidAPI/FunctionDomain1D.h"
#include "MantidAPI/FunctionValues.h"
#include "MantidAPI/FunctionFactory.h"
#include "MantidAPI/IFunction.h"

using namespace Mantid::CurveFitting;
using namespace Mantid::API;

class ParameterEstimatorTest : public CxxTest::TestSuite {
public:
  // This pair of boilerplate methods prevent the suite being created statically
  // This means the constructor isn't called when running other tests
  static ParameterEstimatorTest *createSuite() {
    return new ParameterEstimatorTest();
  }
  static void destroySuite(ParameterEstimatorTest *suite) { delete suite; }

  void test_BackToBackExponential() {

    double x[] = {-8.4, -7.4, -6.4, -5.4, -4.4, -3.4, -2.4, -1.4, -0.4,
                  0.6,  1.6,  2.6,  3.6,  4.6,  5.6,  6.6,  7.6,  8.5};
    double y[] = {
        5.899424685451e-06, 0.0007065291085075, 0.03312893859911,
        0.6260554060356,    4.961949842664,     17.4766663495,
        30.00494980772,     29.34773951093,     20.53693564978,
        12.73990150587,     7.739398423952,     4.694362241084,
        2.847186128946,     1.726807862958,     1.047276595461,
        0.6351380161499,    0.3851804649058,    0.1957211887562,
    };
    auto function =
        FunctionFactory::Instance().createFunction("BackToBackExponential");
    estimate(*function, x, y, sizeof(x) / sizeof(double));

    TS_ASSERT_DELTA(function->getParameter("I"), 130.8, 0.1);
    TS_ASSERT_DELTA(function->getParameter("X0"), -2.65, 0.1);
    TS_ASSERT_DELTA(function->getParameter("A"), 100, 0.1);
    TS_ASSERT_DELTA(function->getParameter("B"), 0.84, 0.1);
    TS_ASSERT_DELTA(function->getParameter("S"), 1.46, 0.1);
  }

  void test_Gaussian() {
    double x[] = {
        -10, -9.79797979798, -9.59595959596, -9.393939393939, -9.191919191919,
        -8.989898989899, -8.787878787879, -8.585858585859, -8.383838383838,
        -8.181818181818, -7.979797979798, -7.777777777778, -7.575757575758,
        -7.373737373737, -7.171717171717, -6.969696969697, -6.767676767677,
        -6.565656565657, -6.363636363636, -6.161616161616, -5.959595959596,
        -5.757575757576, -5.555555555556, -5.353535353535, -5.151515151515,
        -4.949494949495, -4.747474747475, -4.545454545455, -4.343434343434,
        -4.141414141414, -3.939393939394, -3.737373737374, -3.535353535354,
        -3.333333333333, -3.131313131313, -2.929292929293, -2.727272727273,
        -2.525252525253, -2.323232323232, -2.121212121212, -1.919191919192,
        -1.717171717172, -1.515151515152, -1.313131313131, -1.111111111111,
        -0.9090909090909, -0.7070707070707, -0.5050505050505, -0.3030303030303,
        -0.1010101010101, 0.1010101010101, 0.3030303030303, 0.5050505050505,
        0.7070707070707, 0.9090909090909, 1.111111111111, 1.313131313131,
        1.515151515152, 1.717171717172, 1.919191919192, 2.121212121212,
        2.323232323232, 2.525252525253, 2.727272727273, 2.929292929293,
        3.131313131313, 3.333333333333, 3.535353535354, 3.737373737374,
        3.939393939394, 4.141414141414, 4.343434343434, 4.545454545455,
        4.747474747475, 4.949494949495, 5.151515151515, 5.353535353535,
        5.555555555556, 5.757575757576, 5.959595959596, 6.161616161616,
        6.363636363636, 6.565656565657, 6.767676767677, 6.969696969697,
        7.171717171717, 7.373737373737, 7.575757575758, 7.777777777778,
        7.979797979798, 8.181818181818, 8.383838383838, 8.585858585859,
        8.787878787879, 8.989898989899, 9.191919191919, 9.393939393939,
        9.59595959596, 9.79797979798, 10};
    double y[] = {-0.01679832208748,   0.003061949355257,   0.002644253505334,
                  0.007151032498319,   -0.001642868594606,  0.007460481779797,
                  -0.007876121307695,  -0.02170221181184,   -0.007135245496128,
                  -0.0008978939644587, 0.005024742108081,   0.01211913129195,
                  0.006743160024371,   4.449153996997e-05,  -0.01248423377186,
                  0.001518063384342,   0.006079605718969,   0.02703003179924,
                  -0.009499418479466,  -0.006709310442121,  -0.002185335142536,
                  0.01863041838785,    0.007607330918372,   -0.02515609466456,
                  0.008348457474897,   -0.003568451481119,  0.001551516128938,
                  0.004523019597412,   -0.004364275475163,  -0.0155381201051,
                  -0.01554647282983,   0.01383261469493,    0.01653548846701,
                  -0.003553807962371,  -0.004307145849321,  0.02352615603877,
                  0.03118385337707,    0.0402695509854,     0.07026957653713,
                  0.09817007992278,    0.1444156370163,     0.2334545501339,
                  0.3448406258228,     0.4123062465179,     0.5527520011601,
                  0.6516890444321,     0.7852951720845,     0.9026847207267,
                  0.9547404505991,     0.9949823007443,     0.9854917923256,
                  0.9485953092356,     0.8661148866137,     0.8004913219165,
                  0.6526653525529,     0.540112205249,      0.4182985729987,
                  0.3325291561869,     0.2171514763306,     0.1587848969103,
                  0.1127256679885,     0.07596063701158,    0.03881248439856,
                  0.01536133247629,    0.01826546893982,    0.02367303847962,
                  0.02235250457942,    0.008949917061784,   0.02648639053831,
                  0.003132877572737,   -0.001059641103068,  0.01820997428338,
                  0.01108733548098,    -0.002457526181233,  0.002597452421665,
                  -0.01186986844314,   -0.0001813514263672, 0.005121653751356,
                  0.01497128833308,    -0.002830506069514,  -0.0007404657292953,
                  -0.01444137132047,   -0.005058000952273,  -0.008889338402017,
                  -0.008992363786279,  0.005603357598834,   -0.002499134684128,
                  0.0004101949212826,  -0.01569308668572,   0.01354234130162,
                  0.006676995784779,   -0.00641270912814,   0.009467532859651,
                  -0.0006461912464962, 0.007521116687525,   0.01764321586654,
                  0.006035919625726,   0.008680619609555,   -0.02007425237712,
                  0.009870974433398};
    auto function = FunctionFactory::Instance().createFunction("Gaussian");
    estimate(*function, x, y, sizeof(x) / sizeof(double));
    TS_ASSERT_DELTA(function->getParameter("Height"), 0, 0.1);
    TS_ASSERT_DELTA(function->getParameter("PeakCentre"), 0, 0.1);
    TS_ASSERT_DELTA(function->getParameter("Sigma"), 1.0, 0.1);
  }

  void test_Lorentzian() {
    double x[] = {
        -10, -9.79797979798, -9.59595959596, -9.393939393939, -9.191919191919,
        -8.989898989899, -8.787878787879, -8.585858585859, -8.383838383838,
        -8.181818181818, -7.979797979798, -7.777777777778, -7.575757575758,
        -7.373737373737, -7.171717171717, -6.969696969697, -6.767676767677,
        -6.565656565657, -6.363636363636, -6.161616161616, -5.959595959596,
        -5.757575757576, -5.555555555556, -5.353535353535, -5.151515151515,
        -4.949494949495, -4.747474747475, -4.545454545455, -4.343434343434,
        -4.141414141414, -3.939393939394, -3.737373737374, -3.535353535354,
        -3.333333333333, -3.131313131313, -2.929292929293, -2.727272727273,
        -2.525252525253, -2.323232323232, -2.121212121212, -1.919191919192,
        -1.717171717172, -1.515151515152, -1.313131313131, -1.111111111111,
        -0.9090909090909, -0.7070707070707, -0.5050505050505, -0.3030303030303,
        -0.1010101010101, 0.1010101010101, 0.3030303030303, 0.5050505050505,
        0.7070707070707, 0.9090909090909, 1.111111111111, 1.313131313131,
        1.515151515152, 1.717171717172, 1.919191919192, 2.121212121212,
        2.323232323232, 2.525252525253, 2.727272727273, 2.929292929293,
        3.131313131313, 3.333333333333, 3.535353535354, 3.737373737374,
        3.939393939394, 4.141414141414, 4.343434343434, 4.545454545455,
        4.747474747475, 4.949494949495, 5.151515151515, 5.353535353535,
        5.555555555556, 5.757575757576, 5.959595959596, 6.161616161616,
        6.363636363636, 6.565656565657, 6.767676767677, 6.969696969697,
        7.171717171717, 7.373737373737, 7.575757575758, 7.777777777778,
        7.979797979798, 8.181818181818, 8.383838383838, 8.585858585859,
        8.787878787879, 8.989898989899, 9.191919191919, 9.393939393939,
        9.59595959596, 9.79797979798, 10};
    double y[] = {0.01366332320148,  0.0143431768965,   0.01507477940406,
                  0.01586348761067,  0.01671536544891,  0.017637298123,
                  0.01863712829072,  0.01972381911902,  0.0209076503934,
                  0.02220045549009,  0.02361590913526,  0.02516987863768,
                  0.02688085491348,  0.02877048442773,  0.03086422958227,
                  0.03319219367385,  0.03579015816347,  0.03870089581601,
                  0.04197584496791,  0.04567726016574,  0.04988099615402,
                  0.05468014067373,  0.06018979397759,  0.06655340973588,
                  0.0739512777979,   0.08261196439976,  0.09282785640846,
                  0.1049764135994,   0.1195493400507,   0.1371926220265,
                  0.1587610776086,   0.1853911333588,   0.2185932742134,
                  0.2603564655245,   0.3132295863024,   0.3802741513741,
                  0.4646205665074,   0.5680582708104,   0.6877822780359,
                  0.8110359443168,   0.9111532136211,   0.9546481634802,
                  0.9233787643179,   0.830613857915,    0.7090411801509,
                  0.5874548207063,   0.4808527783224,   0.3933143741823,
                  0.3235377894296,   0.268482361346,    0.2250262150497,
                  0.1905237061532,   0.1628947977912,   0.1405550706512,
                  0.1223115399599,   0.1072671959709,   0.09474485752142,
                  0.08422976752886,  0.07532736035567,  0.06773247238409,
                  0.06120691846437,  0.05556310521837,  0.05065198486373,
                  0.04635413496452,  0.04257309982943,  0.03923037866474,
                  0.03626162156433,  0.03361371834451,  0.03124255267633,
                  0.02911125594308,  0.02718883942874,  0.02544911514685,
                  0.02386983853226,  0.02243202290111,  0.02111938782064,
                  0.01991791257232,  0.01881547262127,  0.01780154204922,
                  0.01686694871546,  0.01600367180306,  0.01520467361974,
                  0.01446375922519,  0.01377545877454,  0.01313492849367,
                  0.01253786700598,  0.01198044436249,  0.01145924162771,
                  0.01097119927216,  0.01051357294021,  0.01008389541738,
                  0.009679943827369, 0.00929971125555,  0.008941382131707,
                  0.008603310815123, 0.008284002916182, 0.007982098963178,
                  0.007696360084717, 0.007425655429154, 0.007168951084961,
                  0.006925300301337};
    auto function = FunctionFactory::Instance().createFunction("Lorentzian");
    estimate(*function, x, y, sizeof(x) / sizeof(double));
    TS_ASSERT_DELTA(function->getParameter("Amplitude"), 1.0, 0.1);
    TS_ASSERT_DELTA(function->getParameter("PeakCentre"), 0.0, 0.1);
    TS_ASSERT_DELTA(function->getParameter("FWHM"), 2.2, 0.1);
  }

private:
  void estimate(IFunction &function, double x[], double y[], size_t n) {
    FunctionDomain1DView domain(x, n);
    FunctionValues values(domain);
    for (size_t i = 0; i < n; ++i) {
      values.setFitData(i, y[i]);
    }
    ParameterEstimator::estimate(function, domain, values);
  }
};

#endif /* MANTID_CURVEFITTING_PARAMETERESTIMATORTEST_H_ */