#ifndef MANTID_SLICEVIEWER_COMPOSITEPEAKSPRESENTER_H_
#define MANTID_SLICEVIEWER_COMPOSITEPEAKSPRESENTER_H_

#include "MantidQtSliceViewer/PeaksPresenter.h"
#include "MantidQtSliceViewer/NullPeaksPresenter.h"
#include "MantidQtSliceViewer/PeakBoundingBox.h"
#include "MantidQtSliceViewer/PeakPalette.h"
#include "MantidQtSliceViewer/ZoomablePeaksView.h"
#include "MantidQtSliceViewer/UpdateableOnDemand.h"
#include "MantidQtSliceViewer/ZoomableOnDemand.h"
#include <vector>
#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include <boost/weak_ptr.hpp>

namespace Mantid {
namespace API {
// Forward declaration
class IPeaksWorkspace;
}
}

namespace MantidQt {
namespace SliceViewer {
/*---------------------------------------------------------
CompositePeaksPresenter

Composite implmentation of the Peaks presenter. Holds 0 - N nested
PeaksPresenters.
Note that it's default behaviour is identical to that of the NullPeaksPresenter.
----------------------------------------------------------*/
class DLLExport CompositePeaksPresenter : public PeaksPresenter,
                                          public UpdateableOnDemand,
                                          public ZoomableOnDemand {
public:
  // Overrriden methods from Peaks Presenter
  void update() override;
  void updateWithSlicePoint(const PeakBoundingBox &) override;
  bool changeShownDim() override;
  bool isLabelOfFreeAxis(const std::string &label) const override;
  SetPeaksWorkspaces presentedWorkspaces() const override;
  void setForegroundColor(const QColor) override { /*Do nothing*/
  }
  void setBackgroundColor(const QColor) override { /*Do nothing*/
  }
  void showBackgroundRadius(const bool) override { /*Do nothing*/
  }
  void setShown(const bool) override { /*Do nothing*/
  }
  PeakBoundingBox getBoundingBox(const int peakIndex) const override {
    return m_default->getBoundingBox(peakIndex);
  }
  void sortPeaksWorkspace(const std::string &,
                          const bool) override { /*Do Nothing*/
  }
  bool getShowBackground() const override {
    return m_default->getShowBackground();
  }
  void zoomToPeak(const int) override { /* Do nothing */
  }
  std::string getTransformName() const override;
  bool isHidden() const override { return m_default->isHidden(); }
  void reInitialize(
      boost::shared_ptr<Mantid::API::IPeaksWorkspace>) override { /*Do nothing*/
  }
  bool deletePeaksIn(PeakBoundingBox box) override;
  bool addPeakAt(double plotCoordsPointX, double plotCoordsPointY) override;

  /// Constructor
  CompositePeaksPresenter(ZoomablePeaksView *const zoomablePlottingWidget,
                          PeaksPresenter_sptr defaultPresenter =
                              PeaksPresenter_sptr(new NullPeaksPresenter));
  /// Destructor
  ~CompositePeaksPresenter() override;
  /// Add a peaks presenter onto the composite.
  void addPeaksPresenter(PeaksPresenter_sptr presenter);
  /// Get the number of subjects.
  size_t size() const;
  /// Clear the owned presenters.
  void clear();
  /// Set the peaks size within the current projection
  void setPeakSizeOnProjection(const double fraction) override;
  /// Set the peaks size into the current projection
  void setPeakSizeIntoProjection(const double fraction) override;
  /// Get the peaks size onto the current projection
  double getPeakSizeOnProjection() const override;
  /// Get the peaks size into the current projection
  double getPeakSizeIntoProjection() const override;
  /// Enter peak edit mode.
  void peakEditMode(EditMode mode) override;
  /// Change the foreground representation for the peaks of this workspace
  void
  setForegroundColour(boost::shared_ptr<const Mantid::API::IPeaksWorkspace> ws,
                      const QColor);
  /// Change the background representation for the peaks of this workspace
  void
  setBackgroundColour(boost::shared_ptr<const Mantid::API::IPeaksWorkspace> ws,
                      const QColor);
  /// Get the foreground colour corresponding to the workspace
  QColor getForegroundColour(
      boost::shared_ptr<const Mantid::API::IPeaksWorkspace> ws) const;
  /// Get the background colour corresponding to the workspace
  QColor getBackgroundColour(
      boost::shared_ptr<const Mantid::API::IPeaksWorkspace> ws) const;
  /// Determine if the background is shown or not.
  bool getShowBackground(
      boost::shared_ptr<const Mantid::API::IPeaksWorkspace> ws) const;
  /// Get a copy of the palette in its current state.
  PeakPalette getPalette() const;
  /// Setter for indicating whether the background radius will be shown.
  void setBackgroundRadiusShown(
      boost::shared_ptr<const Mantid::API::IPeaksWorkspace> ws,
      const bool shown);
  /// Remove the workspace and corresponding presenter.
  void remove(boost::shared_ptr<const Mantid::API::IPeaksWorkspace> peaksWS);
  /// Hide these peaks in the plot.
  void setShown(boost::shared_ptr<const Mantid::API::IPeaksWorkspace> peaksWS,
                const bool shown);
  /// zoom in on a peak.
  void zoomToPeak(boost::shared_ptr<const Mantid::API::IPeaksWorkspace> peaksWS,
                  const int peakIndex);
  /// Sort the peaks workspace.
  void sortPeaksWorkspace(
      boost::shared_ptr<const Mantid::API::IPeaksWorkspace> peaksWS,
      const std::string &columnToSortBy, const bool sortedAscending);
  /// Get the named peaks presenter.
  PeaksPresenter *getPeaksPresenter(const QString &name);
  /// Register any owning presenter
  void registerOwningPresenter(UpdateableOnDemand *owner) override;
  /// Is the presenter hidden.
  bool getIsHidden(
      boost::shared_ptr<const Mantid::API::IPeaksWorkspace> peaksWS) const;
  /// Perform update on demand
  void performUpdate() override;
  /// Zoom to the rectangle
  void zoomToPeak(PeaksPresenter *const presenter,
                  const int peakIndex) override;
  /// Forget zoom
  void resetZoom() override;
  /// Get optional zoomed peak presenter.
  boost::optional<PeaksPresenter_sptr> getZoomedPeakPresenter() const;
  /// Get optional zoomed peak index.
  int getZoomedPeakIndex() const;
  /// Make notification that some workspace has been changed.
  void notifyWorkspaceChanged(
      const std::string &wsName,
      boost::shared_ptr<Mantid::API::IPeaksWorkspace> &changedPeaksWS);
  /// Determine if the presenter contents are different.
  bool contentsDifferent(PeaksPresenter const *other) const override;
  /// Enter the requested edit mode for the peaks workspace.
  void editCommand(EditMode editMode, boost::weak_ptr<const Mantid::API::IPeaksWorkspace> target);
  /// Can we add peaks to this peaks workspace.
  bool hasPeakAddModeFor(boost::weak_ptr<const Mantid::API::IPeaksWorkspace> target);
  /// Can we add peaks
  bool hasPeakAddMode() const override;

private:
  /// Updateable on demand method.
  void
  updatePeaksWorkspace(const std::string &toName,
                       boost::shared_ptr<const Mantid::API::IPeaksWorkspace>
                           toWorkspace) override;
  /// Alias for container of subjects type.
  typedef std::vector<PeaksPresenter_sptr> SubjectContainer;
  /// Subject presenters.
  SubjectContainer m_subjects;
  /// Use default
  bool useDefault() const { return m_subjects.size() == 0; }
  /// Get the presenter for a given workspace.
  SubjectContainer::iterator getPresenterIteratorFromWorkspace(
      boost::shared_ptr<const Mantid::API::IPeaksWorkspace> ws);
  /// Get the presenter for a given workspace.
  SubjectContainer::const_iterator getPresenterIteratorFromWorkspace(
      boost::shared_ptr<const Mantid::API::IPeaksWorkspace> ws) const;
  /// Get the presenter from a workspace name.
  SubjectContainer::iterator getPresenterIteratorFromName(const QString &name);
  /// Colour pallette.
  PeakPalette m_palette;
  /// Zoomable peaks view.
  ZoomablePeaksView *const m_zoomablePlottingWidget;
  /// Default behaviour
  PeaksPresenter_sptr m_default;
  /// Owning presenter
  UpdateableOnDemand *m_owner;
  /// Presenter zoomed in on.
  boost::optional<PeaksPresenter_sptr> m_zoomedPresenter;
  /// index of peak zoomed in on.
  int m_zoomedPeakIndex;
};

}
}

#endif
