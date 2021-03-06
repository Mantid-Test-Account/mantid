#pylint: disable=no-init,invalid-name
from mantid.kernel import *
from mantid.api import *

class EnggCalibrate(PythonAlgorithm):
    INDICES_PROP_NAME = 'SpectrumNumbers'

    def category(self):
        return "Diffraction\\Engineering"

    def name(self):
        return "EnggCalibrate"

    def summary(self):
        return ("Calibrates one or more detector banks (or group(s) of detectors) by performing single peak "
                "fitting.")

    def PyInit(self):
        self.declareProperty(MatrixWorkspaceProperty("InputWorkspace", "", Direction.Input),\
                             doc="Workspace with the calibration run to use.")

        import EnggUtils
        self.declareProperty(FloatArrayProperty("ExpectedPeaks",
                                                values=EnggUtils.default_ceria_expected_peaks(),
                                                direction=Direction.Input),
                             doc="A list of dSpacing values where peaks are expected.")

        self.declareProperty(FileProperty(name="ExpectedPeaksFromFile",defaultValue="",
                                          action=FileAction.OptionalLoad,extensions = [".csv"]),
                             doc="Load from file a list of dSpacing values to be translated into TOF to "
                             "find expected peaks. This takes precedence over 'ExpectedPeaks' if both "
                             "options are given.")

        peaks_grp = 'Peaks to fit'
        self.setPropertyGroup('ExpectedPeaks', peaks_grp)
        self.setPropertyGroup('ExpectedPeaksFromFile', peaks_grp)


        self.declareProperty(MatrixWorkspaceProperty("VanadiumWorkspace", "", Direction.Input,
                                                     PropertyMode.Optional),
                             doc='Workspace with the Vanadium (correction and calibration) run. '
                             'Alternatively, when the Vanadium run has been already processed, '
                             'the properties can be used')

        self.declareProperty(ITableWorkspaceProperty("VanIntegrationWorkspace", "",
                                                     Direction.Input, PropertyMode.Optional),
                             doc='Results of integrating the spectra of a Vanadium run, with one column '
                             '(integration result) and one row per spectrum. This can be used in '
                             'combination with OutVanadiumCurveFits from a previous execution and '
                             'VanadiumWorkspace to provide pre-calculated values for Vanadium correction.')

        self.declareProperty(MatrixWorkspaceProperty('VanCurvesWorkspace', '', Direction.Input,
                                                     PropertyMode.Optional),
                             doc = 'A workspace2D with the fitting workspaces corresponding to '
                             'the instrument banks. This workspace has three spectra per bank, as produced '
                             'by the algorithm Fit. This is meant to be used as an alternative input '
                             'VanadiumWorkspace for testing and performance reasons. If not given, no '
                             'workspace is generated.')

        vana_grp = 'Vanadium (open beam) properties'
        self.setPropertyGroup('VanadiumWorkspace', vana_grp)
        self.setPropertyGroup('VanIntegrationWorkspace', vana_grp)
        self.setPropertyGroup('VanCurvesWorkspace', vana_grp)

        self.declareProperty("Bank", '', StringListValidator(EnggUtils.ENGINX_BANKS),
                             direction=Direction.Input,
                             doc = "Which bank to calibrate. It can be specified as 1 or 2, or "
                             "equivalently, North or South. See also " + self.INDICES_PROP_NAME + " "
                             "for a more flexible alternative to select specific detectors")

        self.declareProperty(self.INDICES_PROP_NAME, '', direction=Direction.Input,
                             doc = 'Sets the spectrum numbers for the detectors '
                             'that should be considered in the calibration (all others will be '
                             'ignored). This option cannot be used together with Bank, as they overlap. '
                             'You can give multiple ranges, for example: "0-99", or "0-9, 50-59, 100-109".')

        banks_grp = 'Banks / spectra'
        self.setPropertyGroup('Bank', banks_grp)
        self.setPropertyGroup(self.INDICES_PROP_NAME, banks_grp)

        self.declareProperty(ITableWorkspaceProperty("DetectorPositions", "",
                                                     Direction.Input, PropertyMode.Optional),
                             "Calibrated detector positions. If not specified, default ones (from the "
                             "current instrument definition) are used.")

        self.declareProperty('OutputParametersTableName', '', direction=Direction.Input,
                             doc = 'Name for a table workspace with the calibration parameters calculated '
                             'from this algorithm: difc and zero parameters for GSAS. these two parameters '
                             'are added as two columns in a single row. If not given, no table is '
                             'generated.')

        self.declareProperty("Difc", 0.0, direction = Direction.Output,
                             doc = "Calibrated Difc value for the bank or range of pixels/detectors given")

        self.declareProperty("Zero", 0.0, direction = Direction.Output,
                             doc = "Calibrated Zero value for the bank or range of pixels/detectors given")

        self.declareProperty(ITableWorkspaceProperty("FittedPeaks", "", Direction.Output),
                             doc = "Information on fitted peaks as produced by the (child) algorithm "
                             "EnggFitPeaks.")

        out_grp = 'Outputs'
        self.setPropertyGroup('DetectorPositions', out_grp)
        self.setPropertyGroup('OutputParametersTableName', out_grp)
        self.setPropertyGroup('Difc', out_grp)
        self.setPropertyGroup('Zero', out_grp)
        self.setPropertyGroup('FittedPeaks', out_grp)

    def PyExec(self):

        import EnggUtils

        # Get peaks in dSpacing from file
        expectedPeaksD = EnggUtils.read_in_expected_peaks(self.getPropertyValue("ExpectedPeaksFromFile"),
                                                          self.getProperty('ExpectedPeaks').value)

        prog = Progress(self, start=0, end=1, nreports=2)

        prog.report('Focusing the input workspace')
        focussed_ws = self._focus_run(self.getProperty('InputWorkspace').value,
                                      self.getProperty("VanadiumWorkspace").value,
                                      self.getProperty('Bank').value,
                                      self.getProperty(self.INDICES_PROP_NAME).value)

        if len(expectedPeaksD) < 1:
            raise ValueError("Cannot run this algorithm without any input expected peaks")

        prog.report('Fitting parameters for the focused run')
        difc, zero, fitted_peaks = self._fit_params(focussed_ws, expectedPeaksD)

        self.log().information("Fitted {0} peaks. Resulting difc: {1}, tzero: {2}".
                               format(fitted_peaks.rowCount(), difc, zero))
        self.log().information("Peaks fitted: {0}, centers in ToF: {1}".
                               format(fitted_peaks.column("dSpacing"),
                                      fitted_peaks.column("X0")))

        self._produce_outputs(difc, zero, fitted_peaks)

    def _fit_params(self, focused_ws, expected_peaks_d):
        """
        Fit the GSAS parameters that this algorithm produces: difc and zero. Fits a
        number of peaks starting from the expected peak positions. Then it fits a line
        on the peak positions to produce the DIFC and TZERO as used in GSAS.

        @param focused_ws :: focused workspace to do the fitting on
        @param expected_peaks_d :: expected peaks, used as intial peak positions for the
        fitting, in d-spacing units

        @returns a tuple of parameters: difc, zero, and a list of peak centers as fitted
        """

        fit_alg = self.createChildAlgorithm('EnggFitPeaks')
        fit_alg.setProperty('InputWorkspace', focused_ws)
        fit_alg.setProperty('WorkspaceIndex', 0) # There should be only one index anyway
        fit_alg.setProperty('ExpectedPeaks', expected_peaks_d)
        # we could also pass raw 'ExpectedPeaks' and 'ExpectedPeaksFromFile' to
        # EnggFitPaks, but better to check inputs early, before this
        fit_alg.execute()

        difc = fit_alg.getProperty('Difc').value
        zero = fit_alg.getProperty('Zero').value
        fitted_peaks = fit_alg.getProperty('FittedPeaks').value

        return difc, zero, fitted_peaks

    def _focus_run(self, ws, vanWS, bank, indices):
        """
        Focuses the input workspace by running EnggFocus as a child algorithm, which will produce a
        single spectrum workspace.

        @param ws :: workspace to focus
        @param vanWS :: workspace with Vanadium run for corrections
        @param bank :: the focussing will be applied on the detectors of this bank
        @param indices :: list of indices to consider, as an alternative to bank (bank and indices are
        mutually exclusive)

        @return focussed (summed) workspace
        """
        alg = self.createChildAlgorithm('EnggFocus')
        alg.setProperty('InputWorkspace', ws)

        alg.setProperty('Bank', bank)
        alg.setProperty(self.INDICES_PROP_NAME, indices)

        detPos = self.getProperty('DetectorPositions').value
        if detPos:
            alg.setProperty('DetectorPositions', detPos)

        if vanWS:
            alg.setProperty('VanadiumWorkspace', vanWS)

        integWS = self.getProperty('VanIntegrationWorkspace').value
        if integWS:
            alg.setProperty('VanIntegrationWorkspace', integWS)

        curvesWS = self.getProperty('VanCurvesWorkspace').value
        if curvesWS:
            alg.setProperty('VanCurvesWorkspace', curvesWS)

        alg.execute()

        return alg.getProperty('OutputWorkspace').value

    def _produce_outputs(self, difc, zero, fitted_peaks):
        """
        Just fills in the output properties as requested

        @param difc :: the difc GSAS parameter as fitted here
        @param zero :: the zero GSAS parameter as fitted here
        @param fitted_peaks :: table workspace with peak parameters (one peak per row)
        """

        import EnggUtils

        self.setProperty('Difc', difc)
        self.setProperty('Zero', zero)
        self.setProperty('FittedPeaks', fitted_peaks)

        # make output table if requested
        tblName = self.getPropertyValue("OutputParametersTableName")
        if '' != tblName:
            EnggUtils.generateOutputParTable(tblName, difc, zero)
            self.log().information("Output parameters added into a table workspace: %s" % tblName)


AlgorithmFactory.subscribe(EnggCalibrate)
