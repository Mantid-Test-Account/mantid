#ifndef MANTID_CUSTOMINTERFACES_PROGRESSABLEVIEW_H_
#define MANTID_CUSTOMINTERFACES_PROGRESSABLEVIEW_H_

#include "MantidQtCustomInterfaces/DllConfig.h"

namespace Mantid {
namespace Kernel {
// Forward dec
class ProgressBase;
}
}

namespace MantidQt {
namespace CustomInterfaces {

/** ProgressableView : Abstract view useful for indicating progress

  Copyright &copy; 2015 ISIS Rutherford Appleton Laboratory, NScD Oak Ridge
  National Laboratory & European Spallation Source

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
class MANTIDQT_CUSTOMINTERFACES_DLL ProgressableView {
public:
  virtual void setProgressRange(int min, int max) = 0;
  virtual void setProgress(int progress) = 0;
  virtual void clearProgress() = 0;
  virtual ~ProgressableView() {}
};

} // namespace CustomInterfaces
} // namespace MantidQt

#endif /* MANTID_CUSTOMINTERFACES_PROGRESSABLEVIEW_H_ */
