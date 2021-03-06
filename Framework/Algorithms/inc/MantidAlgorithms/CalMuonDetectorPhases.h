#ifndef MANTID_ALGORITHMS_CALMUONDETECTORPHASES_H_
#define MANTID_ALGORITHMS_CALMUONDETECTORPHASES_H_

#include "MantidAlgorithms/DllConfig.h"
#include "MantidAPI/Algorithm.h"
#include "MantidAPI/ITableWorkspace_fwd.h"

namespace Mantid {
namespace Algorithms {

/** CalMuonDetectorPhases : Calculates asymmetry and phase for each spectra in a
  workspace

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
class DLLExport CalMuonDetectorPhases : public API::Algorithm {
public:
  /// Default constructor
  CalMuonDetectorPhases() : API::Algorithm(){};
  /// Destructor
  ~CalMuonDetectorPhases() override{};
  /// Algorithm's name for identification overriding a virtual method
  const std::string name() const override { return "CalMuonDetectorPhases"; }
  /// Summary of algorithms purpose
  const std::string summary() const override {
    return "Calculates the asymmetry and phase for each detector in a "
           "workspace.";
  }

  /// Algorithm's version for identification overriding a virtual method
  int version() const override { return 1; }
  /// Algorithm's category for identification overriding a virtual method
  const std::string category() const override { return "Muon"; }

private:
  /// Initialise the algorithm
  void init() override;
  /// Execute the algorithm
  void exec() override;
  /// Validate the inputs
  std::map<std::string, std::string> validateInputs() override;
  /// Prepare workspace for fit by extracting data
  API::MatrixWorkspace_sptr extractDataFromWorkspace(double startTime,
                                                     double endTime);
  /// Remove exponential data from workspace
  API::MatrixWorkspace_sptr
  removeExpDecay(const API::MatrixWorkspace_sptr &wsInput);
  /// Fit the workspace
  void fitWorkspace(const API::MatrixWorkspace_sptr &ws, double freq,
                    std::string groupName, API::ITableWorkspace_sptr &resTab,
                    API::WorkspaceGroup_sptr &resGroup);
  /// Create the fitting function as string
  std::string createFittingFunction(double freq, bool fixFreq);
  /// Extract asymmetry and phase from fitting results
  void extractDetectorInfo(const API::ITableWorkspace_sptr &paramTab,
                           const API::ITableWorkspace_sptr &resultsTab,
                           const int ispec);
  /// Find frequency to use in sequential fit
  double getFrequency(const API::MatrixWorkspace_sptr &ws);
  /// Get frequency hint to use when finding frequency
  double getFrequencyHint() const;
  /// Get start time for fit
  double getStartTime() const;
  /// Get end time for fit
  double getEndTime() const;
  /// Calculate detector efficiency (alpha)
  double getAlpha(const API::MatrixWorkspace_sptr &ws,
                  const std::vector<int> &forward,
                  const std::vector<int> &backward);
  /// Calculate asymmetry
  API::MatrixWorkspace_sptr getAsymmetry(const API::MatrixWorkspace_sptr &ws,
                                         const std::vector<int> &forward,
                                         const std::vector<int> &backward,
                                         const double alpha);
  /// Fit asymmetry to get frequency
  double fitFrequencyFromAsymmetry(const API::MatrixWorkspace_sptr &wsAsym);
  /// Find the grouping from the instrument
  void getGroupingFromInstrument(const API::MatrixWorkspace_sptr &ws,
                                 std::vector<int> &forward,
                                 std::vector<int> &backward);
  /// Report progress in GUI
  void reportProgress(const int thisSpectrum, const int totalSpectra);
  /// Pointer to input workspace
  API::MatrixWorkspace_sptr m_inputWS;
};
} // namespace Algorithms
} // namespace Mantid

#endif /* MANTID_ALGORITHMS_CALMUONDETECTORPHASES_H_ */