#ifndef MANTID_CURVEFITTING_SPLINESMOOTHING_H_
#define MANTID_CURVEFITTING_SPLINESMOOTHING_H_

#include "MantidKernel/System.h"
#include "MantidAPI/Algorithm.h"
#include "MantidAPI/WorkspaceFactory.h"
#include "MantidCurveFitting/Functions/BSpline.h"

namespace Mantid {
namespace CurveFitting {
namespace Algorithms {

/** Takes a 2D workspace and produces an output workspace containing a smoothed
  version of the data by selecting
  a number of points to define a spline for each histogram in the workspace.

  @author Samuel Jackson, STFC
  @date 24/07/2013

  Copyright &copy; 2013 ISIS Rutherford Appleton Laboratory, NScD Oak Ridge
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
class DLLExport SplineSmoothing : public API::Algorithm {
public:
  SplineSmoothing();
  ~SplineSmoothing() override;

  const std::string name() const override;
  int version() const override;
  const std::string category() const override;
  /// Summary of algorithms purpose
  const std::string summary() const override {
    return "Smoothes a set of spectra using a cubic spline. Optionally, this "
           "algorithm can also calculate derivatives up to order 2 as a side "
           "product";
  }

private:
  /// number of smoothing points to start with
  const int M_START_SMOOTH_POINTS;

  // Overriden methods
  void init() override;
  void exec() override;

  /// smooth a single spectrum of the workspace
  void smoothSpectrum(int index);

  /// calculate derivatives for a single spectrum
  void calculateSpectrumDerivatives(int index, int order);

  /// setup an output workspace using meta data from inws and taking a number of
  /// spectra
  API::MatrixWorkspace_sptr
  setupOutputWorkspace(API::MatrixWorkspace_const_sptr inws, int size) const;

  /// convert a binned workspace to point data. Uses mean of the bins as point
  API::MatrixWorkspace_sptr
  convertBinnedData(API::MatrixWorkspace_sptr workspace);

  /// set the points used in the spline for smoothing
  void setSmoothingPoint(const int index, const double xpoint,
                         const double ypoint) const;

  /// choose points to define a spline and smooth the data
  void selectSmoothingPoints(API::MatrixWorkspace_const_sptr inputWorkspace,
                             size_t row);

  /// calculate the spline based on the smoothing points chosen
  void calculateSmoothing(API::MatrixWorkspace_const_sptr inputWorkspace,
                          API::MatrixWorkspace_sptr outputWorkspace,
                          size_t row) const;

  /// calculate the derivatives for a set of points on the spline
  void calculateDerivatives(API::MatrixWorkspace_const_sptr inputWorkspace,
                            API::MatrixWorkspace_sptr outputWorkspace,
                            int order, size_t row) const;

  /// add a set of smoothing points to the spline
  void addSmoothingPoints(const std::set<int> &points, const double *xs,
                          const double *ys) const;

  /// check if the difference between smoothing points and data points is within
  /// a certain error bound
  bool checkSmoothingAccuracy(const int start, const int end, const double *ys,
                              const double *ysmooth) const;

  /// Use an existing fit function to tidy smoothing
  void performAdditionalFitting(API::MatrixWorkspace_sptr ws, const int row);

  /// CubicSpline member used to perform smoothing
  boost::shared_ptr<Functions::BSpline> m_cspline;
  /// pointer to the input workspace
  API::MatrixWorkspace_sptr m_inputWorkspace;
  /// pointer to the input workspace converted to point data
  API::MatrixWorkspace_sptr m_inputWorkspacePointData;
  /// pointer to the output workspace group of derivatives
  API::WorkspaceGroup_sptr m_derivativeWorkspaceGroup;
  /// pointer to the smoothed output workspace
  API::MatrixWorkspace_sptr m_outputWorkspace;
};

} // namespace Algorithms
} // namespace CurveFitting
} // namespace Mantid

#endif /* MANTID_CURVEFITTING_SPLINESMOOTHING_H_ */
