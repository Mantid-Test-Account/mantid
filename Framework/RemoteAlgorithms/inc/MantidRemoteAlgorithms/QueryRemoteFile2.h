#ifndef QUERYREMOTEFILE2_H_
#define QUERYREMOTEFILE2_H_

#include "MantidAPI/Algorithm.h"

namespace Mantid {
namespace RemoteAlgorithms {

/**
Retrieve the list of files available from a remote compute resource.

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

File change history is stored at: <https://github.com/mantidproject/mantid>.
Code Documentation is available at: <http://doxygen.mantidproject.org>
*/
class DLLExport QueryRemoteFile2 : public Mantid::API::Algorithm {
public:
  /// (Empty) Constructor
  QueryRemoteFile2() : Mantid::API::Algorithm() {}
  /// Virtual destructor
  ~QueryRemoteFile2() override {}
  /// Algorithm's name
  const std::string name() const override { return "QueryRemoteFile"; }
  /// Summary of algorithms purpose
  const std::string summary() const override {
    return "Retrieve a list of the files from a remote compute resource.";
  }

  /// Algorithm's version
  int version() const override { return (2); }
  /// Algorithm's category for identification
  const std::string category() const override { return "Remote"; }

private:
  void init() override;
  /// Execution code
  void exec() override;
};

} // end namespace RemoteAlgorithms
} // end namespace Mantid

#endif /*QUERYREMOTEFILE2_H_*/
