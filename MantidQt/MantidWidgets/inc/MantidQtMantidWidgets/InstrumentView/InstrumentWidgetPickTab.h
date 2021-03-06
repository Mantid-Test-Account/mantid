#ifndef INSTRUMENTWIDGETPICKTAB_H_
#define INSTRUMENTWIDGETPICKTAB_H_

#include <MantidQtMantidWidgets/WidgetDllOption.h>
#include "MantidQtMantidWidgets/InstrumentView/InstrumentWidgetTab.h"
#include "MantidQtMantidWidgets/InstrumentView/MantidGLWidget.h"

#include "MantidAPI/MatrixWorkspace_fwd.h"
#include "MantidGeometry/ICompAssembly.h"
#include "MantidGeometry/IDTypes.h"

class Instrument3DWidget;

class QPushButton;
class QTextEdit;
class QComboBox;
class QCheckBox;
class QLabel;
class QActionGroup;
class QSignalMapper;
class QMenu;

namespace MantidQt
{
	namespace MantidWidgets
	{
		class InstrumentActor;
		class CollapsiblePanel;
		class OneCurvePlot;
		class ProjectionSurface;
		class ComponentInfoController;
		class DetectorPlotController;

		/**
		* Implements the Pick tab in InstrumentWidget.
		* Contains a set of tools which allow one to:
		*
		*  - pick a detector or a tube and display the data in it and some info
		*  - add a peak to a peaks workspace and display an overlay of markers
		*  - select and remove peaks
		*
		*/
		class EXPORT_OPT_MANTIDQT_MANTIDWIDGETS InstrumentWidgetPickTab : public InstrumentWidgetTab
		{
			Q_OBJECT
		public:
			/// Activity type the tab can be in:
			///   Single:  select and display info for a single detector when pointed by the mouse
			///   Tube:    select and display info for a tube of detectors. The immediate parent
			///            of a detector is considered a tube
			///   AddPeak: Click on a detector and then on the miniplot to add a peak marker and
			///            a peak to the attached peaks workspace
			///   SelectPeak: click on a peak marker or draw a rubber-band selector to select peak
			///               markers. Selected peaks can be deleted by pressing the Delete key.
			enum SelectionType { Single = 0, AddPeak, ErasePeak, SingleDetectorSelection, Tube, Draw };
			enum ToolType { Zoom, PixelSelect, TubeSelect, PeakSelect, PeakErase, DrawEllipse, DrawRectangle, DrawFree, EditShape };

			explicit InstrumentWidgetPickTab(InstrumentWidget *instrWidget);
			bool canUpdateTouchedDetector()const;
                        void initSurface() override;
                        void saveSettings(QSettings &settings) const override;
                        void loadSettings(const QSettings &settings) override;
                        bool addToDisplayContextMenu(QMenu &) const override;
                        void selectTool(const ToolType tool);
			boost::shared_ptr<ProjectionSurface> getSurface() const;

			public slots:
			void setTubeXUnits(int units);
			void changedIntegrationRange(double, double);
			private slots:
			void plotContextMenu();
			void sumDetectors();
			void integrateTimeBins();
			void setPlotCaption();
			void setSelectionType();
			void storeCurve();
			void removeCurve(const QString &);
			void singleComponentTouched(size_t pickID);
			void singleComponentPicked(size_t pickID);
			void updateSelectionInfoDisplay();
			void shapeCreated();
			void updatePlotMultipleDetectors();
			void savePlotToWorkspace();
		private:
                  void showEvent(QShowEvent *) override;
                        QColor getShapeBorderColor() const;

			/* Pick tab controls */
			OneCurvePlot* m_plot; ///< Miniplot to display data in the detectors
			QLabel *m_activeTool; ///< Displays a tip on which tool is currently selected
			QPushButton *m_zoom;  ///< Button switching on navigation mode
			QPushButton *m_one;   ///< Button switching on single detector selection mode
			QPushButton *m_tube;  ///< Button switching on detector's parent selection mode
			QPushButton *m_peak;  ///< Button switching on peak creation mode
			QPushButton *m_peakSelect;   ///< Button switching on peak selection mode
			QPushButton *m_rectangle;    ///< Button switching on drawing a rectangular selection region
			QPushButton *m_ellipse;      ///< Button switching on drawing a elliptical selection region
			QPushButton *m_ring_ellipse; ///< Button switching on drawing a elliptical ring selection region
			QPushButton *m_ring_rectangle; ///< Button switching on drawing a rectangular ring selection region
			QPushButton *m_free_draw;      ///< Button switching on drawing a region of arbitrary shape
			QPushButton *m_edit;           ///< Button switching on edditing the selection region
			bool m_plotSum;

			// Actions to set integration option for the detector's parent selection mode
			QAction *m_sumDetectors;      ///< Sets summation over detectors (m_plotSum = true)
			QAction *m_integrateTimeBins; ///< Sets integration over time bins (m_plotSum = false)
			QActionGroup *m_summationType;
			QAction *m_logY;
			QAction *m_linearY;
			QActionGroup *m_yScale;
			QActionGroup* m_unitsGroup;
			QAction *m_detidUnits, *m_lengthUnits, *m_phiUnits, *m_outOfPlaneAngleUnits;
			QSignalMapper *m_unitsMapper;

			// Instrument display context menu actions
			QAction *m_storeCurve; ///< add the current curve to the list of permanently displayed curves
			QAction *m_savePlotToWorkspace; ///< Save data plotted on the miniplot into a MatrixWorkspace

			CollapsiblePanel* m_plotPanel;
			QTextEdit* m_selectionInfoDisplay; ///< Text control for displaying selection information
			CollapsiblePanel* m_infoPanel;
			SelectionType m_selectionType;
			mutable bool m_freezePlot;

			/// Controller responsible for the info display.
			ComponentInfoController* m_infoController;
			/// Controller responsible for the plot.
			DetectorPlotController* m_plotController;

			// Temporary caches for values from settings
			int m_tubeXUnitsCache;
			int m_plotTypeCache;
		};

		/**
		* Class containing the logic of displaying info on the selected
		* component(s) in the info text widget.
		*/
		class ComponentInfoController : public QObject
		{
			Q_OBJECT
		public:
			/// Constructor.
			ComponentInfoController(InstrumentWidgetPickTab *tab, InstrumentActor* instrActor, QTextEdit* infoDisplay);
			public slots:
			void displayInfo(size_t pickID);
			void clear();
		private:
			QString displayDetectorInfo(Mantid::detid_t detid);
			QString displayNonDetectorInfo(Mantid::Geometry::ComponentID compID);
			QString getParameterInfo(Mantid::Geometry::IComponent_const_sptr comp);
			QString getPeakOverlayInfo();

			InstrumentWidgetPickTab* m_tab;
			InstrumentActor* m_instrActor;
			QTextEdit* m_selectionInfoDisplay;

			bool m_freezePlot;
			bool m_instrWidgetBlocked;
			size_t m_currentPickID;
			QString m_xUnits;

		};

		/**
		* Class contining the logic of plotting the data in detectors/tubes.
		*/
		class DetectorPlotController : public QObject
		{
			Q_OBJECT

		public:

			enum PlotType { Single = 0, DetectorSum, TubeSum, TubeIntegral };
			enum TubeXUnits { DETECTOR_ID = 0, LENGTH, PHI, OUT_OF_PLANE_ANGLE, NUMBER_OF_UNITS };

			DetectorPlotController(InstrumentWidgetPickTab *tab, InstrumentActor* instrActor, OneCurvePlot* plot);
			void setEnabled(bool on) { m_enabled = on; }
			void setPlotData(size_t pickID);
			void setPlotData(QList<int> detIDs);
			void updatePlot();
			void clear();
			void savePlotToWorkspace();

			void setPlotType(PlotType type) { m_plotType = type; }
			PlotType getPlotType() const { return m_plotType; }
			void setTubeXUnits(TubeXUnits units) { m_tubeXUnits = units; }
			TubeXUnits getTubeXUnits() const { return m_tubeXUnits; }
			QString getTubeXUnitsName() const;
			QString getTubeXUnitsUnits() const;
			QString getPlotCaption() const;

			private slots:

			void addPeak(double x, double y);

		private:

			void plotSingle(int detid);
			void plotTube(int detid);
			void plotTubeSums(int detid);
			void plotTubeIntegrals(int detid);
			void prepareDataForSinglePlot(
				int detid,
				std::vector<double>&x,
				std::vector<double>&y,
				std::vector<double>* err = NULL);
			void prepareDataForSumsPlot(
				int detid,
				std::vector<double>&x,
				std::vector<double>&y,
				std::vector<double>* err = NULL);
			void prepareDataForIntegralsPlot(
				int detid,
				std::vector<double>&x,
				std::vector<double>&y,
				std::vector<double>* err = NULL);
			static double getOutOfPlaneAngle(const Mantid::Kernel::V3D& pos, const Mantid::Kernel::V3D& origin, const Mantid::Kernel::V3D& normal);

			InstrumentWidgetPickTab* m_tab;
			InstrumentActor* m_instrActor;
			OneCurvePlot* m_plot;

			PlotType m_plotType;
			bool m_enabled;
			TubeXUnits m_tubeXUnits; ///< quantity the time bin integrals to be plotted against
			int m_currentDetID;

		};

	}//MantidWidgets
}//MantidQt

#endif /*INSTRUMENTWIDGETPICKTAB_H_*/
