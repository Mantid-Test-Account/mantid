#ifndef MANTID_WORKFLOWALGORITHMS_INDIRECTQUICKRUN_H_
#define MANTID_WORKFLOWALGORITHMS_INDIRECTQUICKRUN_H_

#include "MantidWorkflowAlgorithms/DllConfig.h"
#include "MantidAPI/Algorithm.h"
namespace Mantid {
namespace WorkflowAlgorithms {

/** 
  IndirectQuickRun : Reduces input data if required then runs Elwin and
  optionally I(Q, t). This algorithm should give an overview of the data
  without performing excessive data analysis.

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
class MANTID_WORKFLOWALGORITHMS_DLL IndirectQuickRun : public API::Algorithm {
public:
  IndirectQuickRun();
  virtual ~IndirectQuickRun();

  virtual const std::string name() const;
  virtual int version() const;
  virtual const std::string category() const;
  virtual const std::string summary() const;

private:
  void init();
  void exec();
};

} // namespace WorkflowAlgorithms
} // namespace Mantid

#endif /* MANTID_WORKFLOWALGORITHMS_INDIRECTQUICKRUN_H_ */