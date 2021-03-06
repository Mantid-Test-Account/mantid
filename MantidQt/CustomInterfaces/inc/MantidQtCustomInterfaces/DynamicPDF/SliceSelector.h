#ifndef MANTIDQTCUSTOMINTERFACES_DYNAMICPDF_SLICESELECTOR_H_
#define MANTIDQTCUSTOMINTERFACES_DYNAMICPDF_SLICESELECTOR_H_
#include "MantidAPI/MatrixWorkspace.h"
#include "MantidQtCustomInterfaces/DllConfig.h"
#include "MantidQtAPI/UserSubWindow.h"
#include "ui_SliceSelector.h"
#include <qwt_plot_spectrogram.h>

namespace MantidQt {
namespace MantidWidgets {
  class MWView;
}
namespace CustomInterfaces {
namespace DynamicPDF {

// Forward Declarations
//class BackgroundRemover;

/// Helper class containing pointer and some metadata for the loaded workspace
class WorkspaceRecord{

public:
  WorkspaceRecord(const std::string &workspaceName);
  void updateMetadata(const size_t &newIndex);

  boost::shared_ptr<Mantid::API::MatrixWorkspace> m_ws;
  const std::string m_name;
  double m_energy;
  std::string m_label;
};

class MANTIDQT_CUSTOMINTERFACES_DLL SliceSelector
    : public MantidQt::API::UserSubWindow {
  Q_OBJECT

public:
  /// The name of the interface as registered into the factory
  static std::string name() { return "Dynamic PDF Slice Selector"; }
  // This interface's categories.
  static QString categoryInfo() { return "DynamicPDF"; }

public:
  SliceSelector(QWidget *parent = nullptr);
  ~SliceSelector();

private slots:
  void showHelp();
  void loadSlices(const QString &workspaceName);
  void updateSelectedSlice(const int &newSelectedIndex);
  void updatePlotSelectedSlice();
  void launchBackgroundRemover();

private:
  void initLayout();
  /// The form generated by Qt Designer
  Ui::SliceSelector m_uiForm;
  boost::shared_ptr<WorkspaceRecord> m_loadedWorkspace;
  size_t m_selectedWorkspaceIndex;
  /// The child dialog to remove the multiphonon background
  //boost::shared_ptr<BackgroundRemover> m_BackgroundRemover;
};

}
}
}
#endif //MANTIDQTCUSTOMINTERFACES_DYNAMICPDF_SLICESELECTOR_H_
