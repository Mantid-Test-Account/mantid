#ifndef MANTID_ALGORITHMS_TOFSANSRESOLUTIONBYPIXELCALCULATOR_H_
#define MANTID_ALGORITHMS_TOFSANSRESOLUTIONBYPIXELCALCULATOR_H_
#include "MantidKernel/System.h"
namespace Mantid {
namespace Algorithms {

/**Helper class which provides the uncertainty calculations for the
TOFSANSResolutionByPixel class


Copyright &copy; 2015 ISIS Rutherford Appleton Laboratory & NScD Oak Ridge
National Laboratory

This file is part of Mantid.

Mantid is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

Mantid is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

File change history is stored at: <https://github.com/mantidproject/mantid>
Code Documentation is available at: <http://doxygen.mantidproject.org>
*/
class DLLExport TOFSANSResolutionByPixelCalculator {
public:
  double getWavelengthIndependentFactor(double r1, double r2, double deltaR,
                                        double l1, double l2) const;
  double getSigmaQValue(double moderatorValue,
                        double wavlengthIndependentFactor, double q,
                        double wavelength, double deltaWavelength,
                        double lCollim, double l2) const;
};
}
}

#endif