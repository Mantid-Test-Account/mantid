#pylint: disable=no-init,invalid-name
from mantid.kernel import *
from mantid.api import *

import math

class EnggFitPeaks(PythonAlgorithm):
    EXPECTED_DIM_TYPE = 'Time-of-flight'
    PEAK_TYPE = 'BackToBackExponential'

    def category(self):
        return "Diffraction\\Engineering"

    def name(self):
        return "EnggFitPeaks"

    def summary(self):
        return ("The algorithm fits an expected diffraction pattern to a workpace spectrum by "
                "performing single peak fits.")

    def PyInit(self):
        self.declareProperty(MatrixWorkspaceProperty("InputWorkspace", "", Direction.Input),\
                             "Workspace to fit peaks in. ToF is expected X unit.")

        self.declareProperty("WorkspaceIndex", 0,\
                             "Index of the spectra to fit peaks in")

        self.declareProperty(FloatArrayProperty("ExpectedPeaks", (self._get_default_peaks())),\
                             "A list of dSpacing values to be translated into TOF to find expected peaks.")

        self.declareProperty(FileProperty(name="ExpectedPeaksFromFile",defaultValue="",
                                          action=FileAction.OptionalLoad,extensions = [".csv"]),
                             "Load from file a list of dSpacing values to be translated into TOF to "
                             "find expected peaks. This takes precedence over 'ExpectedPeaks' if both "
                             "options are given.")

        peaks_grp = 'Peaks to fit'
        self.setPropertyGroup('ExpectedPeaks', peaks_grp)
        self.setPropertyGroup('ExpectedPeaksFromFile', peaks_grp)

        self.declareProperty('OutParametersTable', '', direction=Direction.Input,
                             doc = 'Name for a table workspace with the fitted values calculated by '
                             'this algorithm (Difc and Zero parameters) for GSAS. '
                             'These two parameters are added as two columns in a single row. If not given, '
                             'the table workspace is not created.')

        self.declareProperty('OutFittedPeaksTable', '', direction=Direction.Input,
                             doc = 'Name for a table workspace with the parameters of the peaks found and '
                             'fitted. If not given, the table workspace is not created.')

        self.declareProperty("Difc", 0.0, direction = Direction.Output,\
    		doc = "Fitted Difc value")

        self.declareProperty("Zero", 0.0, direction = Direction.Output,\
    		doc = "Fitted Zero value")

        self.declareProperty(ITableWorkspaceProperty("FittedPeaks", "", Direction.Output),
                             doc = "Information on fitted peaks. The table contains, for every peak fitted "
                             "the expected peak value (in d-spacing), and the parameters fitted. The expected "
                             "values are given in the column labelled 'dSpacing'. When fitting "
                             "back-to-back exponential functions, the 'X0' column has the fitted peak center.")

    def PyExec(self):

        import EnggUtils

        # Get peaks in dSpacing from file
        expected_peaks_dsp = EnggUtils.read_in_expected_peaks(self.getPropertyValue("ExpectedPeaksFromFile"),
                                                              self.getProperty('ExpectedPeaks').value)

        if len(expected_peaks_dsp) < 1:
            raise ValueError("Cannot run this algorithm without any input expected peaks")

        # Get expected peaks in TOF for the detector
        in_wks = self.getProperty("InputWorkspace").value
        dimType = in_wks.getXDimension().getName()
        if self.EXPECTED_DIM_TYPE != dimType:
            raise ValueError("This algorithm expects a workspace with %s X dimension, but "
                             "the X dimension of the input workspace is: '%s'" % (self.EXPECTED_DIM_TYPE, dimType))

        wks_index = self.getProperty("WorkspaceIndex").value

        # FindPeaks will return a list of peaks sorted by the centre found. Sort the peaks as well,
        # so we can match them with fitted centres later.
        expected_peaks_ToF = sorted(self._expected_peaks_in_ToF(expected_peaks_dsp, in_wks, wks_index))

        found_peaks = self._peaks_from_find_peaks(in_wks, expected_peaks_ToF, wks_index)
        if found_peaks.rowCount() < len(expected_peaks_ToF):
            txt = "Peaks effectively found: " + str(found_peaks)[1:-1]
            self.log().warning("Some peaks from the list of expected peaks were not found by the algorithm "
                               "FindPeaks which this algorithm uses to check that the data has the the "
                               "expected peaks. " + txt)

        peaks_table_name = self.getPropertyValue("OutFittedPeaksTable")
        difc, zero, fitted_peaks = self._fit_all_peaks(in_wks, wks_index, (found_peaks, expected_peaks_dsp), peaks_table_name)
        self._produce_outputs(difc, zero, fitted_peaks)

    def _get_default_peaks(self):
        """
        Gets default peaks for Engg algorithms. Values from CeO2
        """
        import EnggUtils

        return EnggUtils.default_ceria_expected_peaks()

    def _produce_outputs(self, difc, zero, fitted_peaks):
        """
        Fills in the output properties as requested via the input properties. Sets the output difc, and zero
        values, and a table workspace with peaks information. It can also produces a table with the difc and
        zero parameters if this is required in the inputs.

        @param difc :: the difc GSAS parameter as fitted here
        @param zero :: the zero GSAS parameter as fitted here
        @param fitted_peaks :: table workspace with peak parameters (one peak per row)
        """

        import EnggUtils

        # mandatory outputs
        self.setProperty('Difc', difc)
        self.setProperty('Zero', zero)
        self.setProperty('FittedPeaks', fitted_peaks)

        # optional outputs
        tbl_name = self.getPropertyValue("OutParametersTable")
        if '' != tbl_name:
            EnggUtils.generateOutputParTable(tbl_name, difc, zero)
            self.log().information("Output parameters added into a table workspace: %s" % tbl_name)


    def _estimate_start_end_fitting_range(self, center, width):
        """
        Try to predict a fit window for the peak (using magic numbers). The heuristic
        +-COEF_LEFT/RIGHT sometimes produces ranges that are too narrow and contain too few
        samples (one or a handful) for the fitting to run correctly. A minimum is enforced.

        @Returns :: a tuple with the range (start and end values) for fitting a peak.
        """
        # Magic numbers, approx. represanting the shape/proportions of a B2BExponential peak
        COEF_LEFT = 2
        COEF_RIGHT = 3
        MIN_RANGE_WIDTH = 175

        startx = center - (width * COEF_LEFT)
        endx = center + (width * COEF_RIGHT)
        # force startx-endx > 175, etc
        x_diff = endx-startx
        min_width = MIN_RANGE_WIDTH
        if x_diff < min_width:
            inc = (min_width-x_diff)/2
            endx = endx + inc
            startx = startx - inc

        return (startx, endx)

    def _fit_all_peaks(self, in_wks, wks_index, peaks, peaks_table_name):
        """
        This method is the core of EnggFitPeaks. It tries to fit as many peaks as there are in the list of
        expected peaks passed to the algorithm. This is a single peak fitting, in the sense that peaks
        are fitted separately, one at a time.

        The parameters from the (Gaussian) peaks fitted by FindPeaks elsewhere (before calling this method)
        are used as initial guesses.

        @param in_wks :: input workspace with spectra for fitting
        @param wks_index :: workspace index of the spectrum where the given peaks should be fitted
        @param peaks :: tuple made of two lists: found_peaks (peaks found by FindPeaks or similar
                        algorithm), and expected_peaks_dsp (expected peaks given as input to this algorithm
                        (in dSpacing units)
        @param peaks_table_name :: name of an (output) table with peaks parameters. If empty, the table is anonymous

        @returns difc and zero parameters and a table with parameters for every fitted peak. The difc and zero
        parameters are obtained from fitting a linear background (in _fit_dSpacing_to_ToF) to the peaks fitted
        here individually

        """
        if 2 != len(peaks):
            raise RuntimeError("Unexpected inconsistency found. This method requires a tuple with the list "
                               "of found peaks and the list of expected peaks.")
        found_peaks = peaks[0]
        fitted_peaks = self._create_fitted_peaks_table(peaks_table_name)

        prog = Progress(self, start=0, end=1, nreports=found_peaks.rowCount())

        for i in range(found_peaks.rowCount()):
            prog.report('Fitting peak number ' + str(i+1))

            row = found_peaks.row(i)
            # Peak parameters estimated by FindPeaks
            initial_params = (row['centre'], row['width'], row['height'])
            # Oh oh, this actually happens sometimes for some spectra of the system test dataset
            # and it should be clarified when the role of FindPeaks etc. is fixed (trac ticket #10907)
            width = initial_params[2]
            if width <= 0.:
                detailTxt = ("Cannot fit a peak with these initial parameters from FindPeaks, center: %s "
                             ", width: %s, height: %s"%(initial_params[0], width, initial_params[1]))
                self.log().notice('For workspace index ' + str(wks_index) + ', a peak that is in the list of '
                                  'expected peaks and was found by FindPeaks has not been fitted correctly. '
                                  'It will be ignored. Details: ' + detailTxt)
                continue


            param_table, chi_over_dof = self._fit_single_peak(peaks[1][i], initial_params, in_wks, wks_index)

            fitted_params = {}
            fitted_params['dSpacing'] = peaks[1][i]
            fitted_params['Chi'] = chi_over_dof
            self._add_parameters_to_map(fitted_params, param_table)

            fitted_peaks.addRow(fitted_params)

        # Check if we were able to really fit any peak
        if 0 == fitted_peaks.rowCount():
            detailTxt = ("Could find " + str(len(found_peaks)) + " peaks using the algorithm FindPeaks but " +
                         "then it was not possible to fit any peak starting from these peaks found and using '" +
                         self.PEAK_TYPE + "' as peak function.")
            self.log().warning('Could not fit any peak. Please check the list of expected peaks, as it does not '
                               'seem to be appropriate for the workspace given. More details: ' +
                               detailTxt)

            raise RuntimeError('Could not fit any peak.  Failed to fit peaks with peak type ' +
                               self.PEAK_TYPE + ' even though FindPeaks found ' + str(found_peaks.rowCount()) +
                               ' peaks in principle. See the logs for further details.')
        # Better than failing to fit the linear function
        if 1 == fitted_peaks.rowCount():
            raise RuntimeError('Could find only one peak. This is not enough to fit the output parameters '
                               'difc and zero. Please check the list of expected peaks given and if it is '
                               'appropriate for the workspace')

        difc, zero = self._fit_dSpacing_to_ToF(fitted_peaks)

        return (difc, zero, fitted_peaks)

    def _fit_single_peak(self, expected_center, initial_params, wks, wks_index):
        """
        Fits one peak, given an initial guess of parameters (center, width, height).

        @param expected_center :: expected peak position
        @param initial_params :: tuple with initial guess of the peak center, width and height
        @param wks :: workspace with data (spectra) to fit
        @param wks_index :: index of the spectrum to fit

        @return parameters from Fit, and the goodness of fit estimation from Fit (as Chi^2/DoF)
        """

        center, width, height = initial_params
        # Sigma value of the peak, assuming Gaussian shape
        sigma = width / (2 * math.sqrt(2 * math.log(2)))

        # Approximate peak intensity, assuming Gaussian shape
        intensity = height * sigma * math.sqrt(2 * math.pi)

        peak = FunctionFactory.createFunction(self.PEAK_TYPE)
        peak.setParameter('X0', center)
        peak.setParameter('S', sigma)
        peak.setParameter('I', intensity)

        # Fit using predicted window and a proper function with approximated initital values
        fit_alg = self.createChildAlgorithm('Fit')
        fit_function = 'name=LinearBackground;{0}'.format(peak)
        fit_alg.setProperty('Function', fit_function)
        fit_alg.setProperty('InputWorkspace', wks)
        fit_alg.setProperty('WorkspaceIndex', wks_index)
        fit_alg.setProperty('CreateOutput', True)

        (startx, endx) = self._estimate_start_end_fitting_range(center, width)
        fit_alg.setProperty('StartX', startx)
        fit_alg.setProperty('EndX', endx)
        self.log().debug("Fitting for peak expectd in (d-spacing): {0}, Fitting peak function: "
                         "{1}, with startx: {2}, endx: {3}".
                         format(expected_center, fit_function, startx, endx))
        fit_alg.execute()
        param_table = fit_alg.getProperty('OutputParameters').value
        chi_over_dof = fit_alg.getProperty('OutputChi2overDoF').value

        return (param_table, chi_over_dof)

    def _peaks_from_find_peaks(self, in_wks, expected_peaks_ToF, wks_index):
        """
        Use the algorithm FindPeaks to check that the expected peaks are there.

        @param in_wks data workspace
        @param expected_peaks_ToF vector/list of expected peak values
        @param wks_index workspace index

        @return list of peaks found by FindPeaks. If there are no issues, the length
        of this list should be the same as the number of expected peaks received.
        """

        # Find approximate peak positions, asumming Gaussian shapes
        find_peaks_alg = self.createChildAlgorithm('FindPeaks')
        find_peaks_alg.setProperty('InputWorkspace', in_wks)
        find_peaks_alg.setProperty('PeakPositions', expected_peaks_ToF)
        find_peaks_alg.setProperty('PeakFunction', 'Gaussian')
        find_peaks_alg.setProperty('WorkspaceIndex', wks_index)
        find_peaks_alg.execute()
        found_peaks = find_peaks_alg.getProperty('PeaksList').value
        return found_peaks

    def _fit_dSpacing_to_ToF(self, fitted_peaksTable):
        """
        Fits a linear background to the dSpacing <-> TOF relationship and returns fitted difc
        and zero values. If the table passed has less than 2 peaks this raises an exception, as it
        is not possible to fit the difc, zero parameters.

        @param fitted_peaksTable :: table with one row per fitted peak, expecting column 'dSpacing'
        as x values and column 'X0' as y values.

        @return the pair of difc and zero values fitted to the peaks.
    	"""

        num_peaks = fitted_peaksTable.rowCount()
        if num_peaks < 2:
            raise ValueError('Cannot fit a linear function with less than two peaks. Got a table of ' +
                             'peaks with ' + str(num_peaks) + ' peaks')

        convert_tbl_alg = self.createChildAlgorithm('ConvertTableToMatrixWorkspace')
        convert_tbl_alg.setProperty('InputWorkspace', fitted_peaksTable)
        convert_tbl_alg.setProperty('ColumnX', 'dSpacing')
        convert_tbl_alg.setProperty('ColumnY', 'X0')
        convert_tbl_alg.execute()
        dSpacingVsTof = convert_tbl_alg.getProperty('OutputWorkspace').value

    	# Fit the curve to get linear coefficients of TOF <-> dSpacing relationship for the detector
        fit_alg = self.createChildAlgorithm('Fit')
        fit_alg.setProperty('Function', 'name=LinearBackground')
        fit_alg.setProperty('InputWorkspace', dSpacingVsTof)
        fit_alg.setProperty('WorkspaceIndex', 0)
        fit_alg.setProperty('CreateOutput', True)
        fit_alg.execute()
        param_table = fit_alg.getProperty('OutputParameters').value

        zero = param_table.cell('Value', 0) # A0
        difc = param_table.cell('Value', 1) # A1

        return (difc, zero)

    def _expected_peaks_in_ToF(self, expectedPeaks, in_wks, wks_index):
        """
        Converts expected peak dSpacing values to TOF values for the
        detector. Implemented by using the Mantid algorithm ConvertUnits. A
        simple user script to do what this function does would be
        as follows:

        import mantid.simpleapi as sapi

        yVals = [1] * (len(expectedPeaks) - 1)
        ws_from = sapi.CreateWorkspace(UnitX='dSpacing', DataX=expectedPeaks, DataY=yVals,
                                      ParentWorkspace=in_wks)
        target_units = 'TOF'
        wsTo = sapi.ConvertUnits(InputWorkspace=ws_from, Target=target_units)
        peaks_ToF = wsTo.dataX(0)
        values = [peaks_ToF[i] for i in range(0,len(peaks_ToF))]

        @param expectedPeaks :: vector of expected peaks, in dSpacing units
        @param in_wks :: input workspace with the relevant instrument/geometry
        @param wks_index spectrum index

        Returns:
            a vector of ToF values converted from the input (dSpacing) vector.

        """

        # When receiving a (for example) focussed workspace we still do not know how
        # to properly deal with it. CreateWorkspace won't copy the instrument sample
        # and source even if given the option ParentWorkspace. Resort to old style
        # hard-coded calculation.
        # The present behavior of 'ConvertUnits' is to show an information log:
        # "Unable to calculate sample-detector distance for 1 spectra. Masking spectrum"
        # and silently produce a wrong output workspace. That might need revisiting.
        if 1 == in_wks.getNumberHistograms():
            return self._do_approx_hard_coded_convert_units_to_ToF(expectedPeaks, in_wks, wks_index)

        # Create workspace just to convert dSpacing -> ToF, yVals are irrelevant
        # this used to be calculated with:
        # lambda d: 252.816 * 2 * (50 + detL2) * math.sin(detTwoTheta / 2.0) * d
        # which is approximately what ConverUnits will do
        # remember the -1, we must produce a histogram data workspace, which is what
        # for example EnggCalibrate expects
        yVals = [1] * (len(expectedPeaks) - 1)
        # Do like: ws_from = sapi.CreateWorkspace(UnitX='dSpacing', DataX=expectedPeaks, DataY=yVals,
        #                                        ParentWorkspace=self.getProperty("InputWorkspace").value)
        create_alg = self.createChildAlgorithm("CreateWorkspace")
        create_alg.setProperty("UnitX", 'dSpacing')
        create_alg.setProperty("DataX", expectedPeaks)
        create_alg.setProperty("DataY", yVals)
        create_alg.setProperty("ParentWorkspace", in_wks)
        create_alg.execute()

        ws_from = create_alg.getProperty("OutputWorkspace").value

        # finally convert units, like: sapi.ConvertUnits(InputWorkspace=ws_from, Target=target_units)
        conv_alg = self.createChildAlgorithm("ConvertUnits")
        conv_alg.setProperty("InputWorkspace", ws_from)
        target_units = 'TOF'
        conv_alg.setProperty("Target", target_units)
        # note: this implicitly uses default property "EMode" value 'Elastic'
        goodExec = conv_alg.execute()

        if not goodExec:
            raise RuntimeError("Conversion of units went wrong. Failed to run ConvertUnits for %d peaks: %s"
                               % (len(expectedPeaks), expectedPeaks))

        wsTo = conv_alg.getProperty('OutputWorkspace').value
        peaks_ToF = wsTo.readX(0)
        if len(peaks_ToF) != len(expectedPeaks):
            raise RuntimeError("Conversion of units went wrong. Converted %d peaks from the original "
                               "list of %d peaks. The instrument definition might be incomplete for the "
                               "original workspace / file."% (len(peaks_ToF), len(expectedPeaks)))

        tof_values = [peaks_ToF[i] for i in range(0,len(peaks_ToF))]
        # catch potential failures because of geometry issues, etc.
        if tof_values == expectedPeaks:
            vals = self._do_approx_hard_coded_convert_units_to_ToF(expectedPeaks, in_wks, wks_index)
            return vals

        return tof_values

    def _do_approx_hard_coded_convert_units_to_ToF(self, dsp_values, ws, wks_index):
        """
        Converts from dSpacing to Time-of-flight, for one spectrum/detector. This method
        is here for exceptional cases that presently need clarification / further work,
        here and elsewhere in Mantid, and should ideally be removed in favor of the more
        general method that uses the algorithm ConvertUnits.

        @param dsp_values to convert from dSpacing
        @param ws workspace with the appropriate instrument / geometry definition
        @param wks_index index of the spectrum

        Return:
        input values converted from dSpacing to ToF
        """
        det = ws.getDetector(wks_index)

        # Current detector parameters
        detL2 = det.getDistance(ws.getInstrument().getSample())
        detTwoTheta = ws.detectorTwoTheta(det)

        # hard coded equation to convert dSpacing -> TOF for the single detector
        dSpacingToTof = lambda d: 252.816 * 2 * (50 + detL2) * math.sin(detTwoTheta / 2.0) * d

        # Values (in principle, expected peak positions) in TOF for the detector
        tof_values = [dSpacingToTof(ep) for ep in dsp_values]
        return tof_values

    def _create_fitted_peaks_table(self, tbl_name):
        """
        Creates a table where to put peak fitting results to

        @param tbl_name :: name of the table workspace (can be empty)
    	"""
        table = None
        if not tbl_name:
            alg = self.createChildAlgorithm('CreateEmptyTableWorkspace')
            alg.execute()
            table = alg.getProperty('OutputWorkspace').value
        else:
            import mantid.simpleapi as sapi
            table = sapi.CreateEmptyTableWorkspace(OutputWorkspace=tbl_name)

        table.addColumn('double', 'dSpacing')

        for param in ['A0', 'A1', 'X0', 'A', 'B', 'S', 'I']:
            table.addColumn('double', param)
            table.addColumn('double', param + '_Err')

        table.addColumn('double', 'Chi')

        return table

    def _add_parameters_to_map(self, param_map, param_table):
        """
        Takes parameters from a table that contains output parameters from a Fit run, and adds
        them as name:value and name_Err:error_value pairs to the map.

        @param param_map :: map where to add the fitting parameters
        @param param_table :: table with parameters from a Fit algorithm run
        """
        for i in range(param_table.rowCount() - 1): # Skip the last (fit goodness) row
            row = param_table.row(i)

            # Get local func. param name. E.g., not f1.A0, but just A0
            name = (row['Name'].rpartition('.'))[2]

            param_map[name] = row['Value']
            param_map[name + '_Err'] = row['Error']


AlgorithmFactory.subscribe(EnggFitPeaks)
