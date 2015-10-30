import unittest
import mantid
import isis_instrument as instruments
import ISISCommandInterface as command_iface
from reducer_singleton import ReductionSingleton
import isis_reduction_steps as reduction_steps
from mantid.simpleapi import *
from mantid.kernel import DateAndTime
import random
import math
class SANSCommandInterfaceGetAndSetTransmissionSettings(unittest.TestCase):
    def test_that_gets_transmission_monitor(self):
        # Arrange
        trans_spectrum = 4
        command_iface.Clean()
        command_iface.SANS2D()
        ReductionSingleton().transmission_calculator.trans_mon = trans_spectrum
        # Act
        result = command_iface.GetTransmissionMonitorSpectrum()
        # Assert
        self.assertEqual(trans_spectrum, result, 'The transmission spectrum should be set to 4.')

    def test_setting_transmission_monitor_to_valid_input(self):
        # Arrange
        trans_spectrum = 4
        command_iface.Clean()
        command_iface.SANS2D()
        # Act
        command_iface.SetTransmissionMonitorSpectrum(trans_mon = trans_spectrum)
        # Assert
        self.assertEqual(trans_spectrum, command_iface.GetTransmissionMonitorSpectrum(), 'The transmission spectrum should be set to 4.')

    def test_setting_transmission_monitor_to_invalid_input_does_not_set(self):
        # Arrange
        trans_spectrum = 4
        trans_spectrum_invalid = '23434_yh'
        command_iface.Clean()
        command_iface.SANS2D()
        # Act
        command_iface.SetTransmissionMonitorSpectrum(trans_mon = trans_spectrum)
        command_iface.SetTransmissionMonitorSpectrum(trans_mon = trans_spectrum_invalid)
        # Assert
        self.assertEqual(trans_spectrum, command_iface.GetTransmissionMonitorSpectrum(), 'The transmission spectrum should be set to 4.')

    def test_that_gets_transmission_monitor_shift(self):
        # Arrange
        trans_spectrum_shift = -55
        command_iface.Clean()
        command_iface.SANS2D()
        ReductionSingleton().get_instrument().monitor_4_offset = trans_spectrum_shift
        # Act
        result = command_iface.GetTransmissionMonitorSpectrumShift()
        # Assert
        self.assertEqual(trans_spectrum_shift, result, 'The transmission monitor shift should be set to -55.')

    def test_setting_shift_to_valid_value(self):
        # Arrange
        trans_spectrum_shift = -55.0
        command_iface.Clean()
        command_iface.SANS2D()
        # Act
        command_iface.SetTransmissionMonitorSpectrumShift(trans_mon_shift = trans_spectrum_shift)
        # Assert
        self.assertEqual(trans_spectrum_shift, command_iface.GetTransmissionMonitorSpectrumShift(), 'The transmission monitor shift should be set to -55.')

    def test_setting_shift_with_invalid_input(self):
        # Arrange
        trans_spectrum_shift = '-55_thg'
        command_iface.Clean()
        command_iface.SANS2D()
        # Act
        command_iface.SetTransmissionMonitorSpectrumShift(trans_mon_shift = trans_spectrum_shift)
        # Assert
        self.assertEqual(None, command_iface.GetTransmissionMonitorSpectrumShift(), 'The transmission monitor shift should be None.')

    def test_that_gets_transmission_radius(self):
        # Arrange
        trans_radius = 23/1000
        command_iface.Clean()
        command_iface.SANS2D()
        ReductionSingleton().transmission_calculator.radius = trans_radius
        # Act
        result = command_iface.GetTransmissionRadiusInMM()
        # Assert
        self.assertEqual(trans_radius*1000, result, 'The transmission radius should be set to 23 mm.')

    def test_setting_radius_to_valid_value(self):
        # Arrange
        trans_radius = 23
        command_iface.Clean()
        command_iface.SANS2D()
        # Act
        command_iface.SetTransmissionRadiusInMM(trans_radius = trans_radius)
        # Assert
        self.assertEqual(trans_radius, command_iface.GetTransmissionRadiusInMM(), 'The transmission radius should be set to 23.')

    def test_setting_radius_with_invalid_input(self):
        # Arrange
        trans_radius = '23_yh'
        command_iface.Clean()
        command_iface.SANS2D()
        # Act
        command_iface.SetTransmissionRadiusInMM(trans_radius = trans_radius)
        # Assert
        self.assertEqual(None, command_iface.GetTransmissionRadiusInMM(), 'The transmission radius should be None.')



    def test_that_gets_non_empty_roi_files(self):
        # Arrange
        trans_roi_files = ['roi_file1.xml', 'roi_file2.xml']
        command_iface.Clean()
        command_iface.SANS2D()
        ReductionSingleton().transmission_calculator.roi_files = trans_roi_files
        # Act
        result = command_iface.GetTransmissionROI()
        # Assert
        self.assertEqual(trans_roi_files, result, 'The transmission roi should have two entries')

    def test_that_gets_None_for_empty_roi_files(self):
         # Arrange
        command_iface.Clean()
        command_iface.SANS2D()
        # Act
        result = command_iface.GetTransmissionROI()
        # Assert
        self.assertEqual(None, result, 'The transmission roi should be None')

    def test_setting_roi_file_for_valid_input(self):
         # Arrange
        trans_roi_files = ['file1.xml', 'file2.xml']
        command_iface.Clean()
        command_iface.SANS2D()
        # Act
        command_iface.SetTransmissionROI(trans_roi_files = trans_roi_files)
        # Assert
        result = ReductionSingleton().transmission_calculator.roi_files
        self.assertEqual(2, len(result), 'The transmission roi list should have two entries')
        self.assertEqual("file1.xml", result[0], 'The first file should be file1.xml')
        self.assertEqual("file2.xml", result[1], 'The second file should be file2.xml')

    def test_setting_roi_file_for_invalid_input(self):
         # Arrange
        trans_roi_files = ['file1g', 'file2.xml']
        command_iface.Clean()
        command_iface.SANS2D()
        # Act
        command_iface.SetTransmissionROI(trans_roi_files = trans_roi_files)
        # Assert
        self.assertEqual(0, len(ReductionSingleton().transmission_calculator.roi_files), 'The transmission roi list should be empty.')



    def test_that_gets_non_empty_mask_files(self):
        # Arrange
        trans_mask_files = ['mask_file1.xml', 'mask_file2.xml']
        command_iface.Clean()
        command_iface.SANS2D()
        ReductionSingleton().transmission_calculator.mask_files = trans_mask_files
        # Act
        result = command_iface.GetTransmissionMask()
        # Assert
        self.assertEqual(trans_mask_files, result, 'The transmission mask should have two entries')

    def test_that_gets_None_for_empty_mask_files(self):
         # Arrange
        command_iface.Clean()
        command_iface.SANS2D()
        # Act
        result = command_iface.GetTransmissionMask()
        # Assert
        self.assertEqual(None, result, 'The transmission mask should be None')

    def test_setting_mask_file_for_valid_input(self):
         # Arrange
        trans_mask_files = ['file1.xml', 'file2.xml']
        command_iface.Clean()
        command_iface.SANS2D()
        # Act
        command_iface.SetTransmissionMask(trans_mask_files = trans_mask_files)
        # Assert
        result = ReductionSingleton().transmission_calculator.mask_files
        self.assertEqual(2, len(result), 'The transmission mask list should have two entries')
        self.assertEqual("file1.xml", result[0], 'The first file should be file1.xml')
        self.assertEqual("file2.xml", result[1], 'The second file should be file2.xml')

    def test_setting_mask_file_for_invalid_input(self):
         # Arrange
        trans_mask_files = " file1g,  file2.xml "
        command_iface.Clean()
        command_iface.SANS2D()
        # Act
        command_iface.SetTransmissionMask(trans_mask_files = trans_mask_files)
        # Assert
        self.assertEqual(0, len(ReductionSingleton().transmission_calculator.mask_files), 'The transmission mask list should be empty.')

class TestEventWorkspaceCheck(unittest.TestCase):
    def _create_file_name(self, name):
        temp_save_dir = config['defaultsave.directory']
        if (temp_save_dir == ''):
            temp_save_dir = os.getcwd()
        return os.path.join(temp_save_dir, name + '.nxs')

    def addSampleLogEntry(self, log_name, ws, start_time, extra_time_shift):
        number_of_times = 10
        for i in range(0, number_of_times):

            val = random.randrange(0, 10, 1)
            date = DateAndTime(start_time)
            date +=  int(i*1e9)
            date += int(extra_time_shift*1e9)
            AddTimeSeriesLog(ws, Name=log_name, Time=date.__str__().strip(), Value=val)

    def _clean_up(self, file_name):
        if os.path.exists(file_name):
            os.remove(file_name)

    def test_that_histogram_workspace_is_detected(self):
        # Arrange
        ws = CreateSampleWorkspace()
        self.addSampleLogEntry('proton_charge', ws, "2010-01-01T00:00:00", 0.0)
        file_name = self._create_file_name('dummy')
        SaveNexus(Filename= file_name, InputWorkspace=ws)
        # Act
        result = command_iface.check_if_event_workspace(file_name)
        self.assertFalse(result)
        # Clean Up
        self._clean_up(file_name)
        DeleteWorkspace(ws)

class TestFitRescaleAndShift(unittest.TestCase):
    def _create_workspace(self, ws_name):
        CreateSampleWorkspace(OutputWorkspace = ws_name)
    def test_that_2D_workspace_is_caught(self):
        #Arrange
        ws_name1 = "ws_fit_and_shift_test_1"
        ws_name2 = "ws_fit_and_shift_test_2"
        self._create_workspace(ws_name1)
        self._create_workspace(ws_name2)
        ws1 = mtd[ws_name1]
        ws2 = mtd[ws_name2]

        scale = 2.4
        shift = 1.1
        rAnds = instruments.DetectorBank._RescaleAndShift(scale, shift)

        # Act
        ret_scale, ret_shift = command_iface._fitRescaleAndShift(rAnds,ws1,ws2)

        # Assert
        self.assertTrue(ret_scale == scale)
        self.assertTrue(ret_shift == shift)

        # Clean up
        DeleteWorkspace(ws1)
        DeleteWorkspace(ws2)



class SANSCommandInterfaceGetAndSetQResolutionSettings(unittest.TestCase):
    '''
    Test the input and output mechanims for the QResolution settings
    '''
    def test_full_setup_for_circular_apertures(self):
        # Arrange
        command_iface.Clean()
        command_iface.SANS2D()
        a1 = 2 # in mm
        a2 = 3 # in mm
        delta_r = 4 # in mm
        collimation_length = 10 # in m
        norm = reduction_steps.CalculateNormISIS()
        ReductionSingleton().to_Q = reduction_steps.ConvertToQISIS(norm)

        # Act
        command_iface.set_q_resolution_a1(a1 = a1)
        command_iface.set_q_resolution_a2(a2 = a2)
        command_iface.set_q_resolution_delta_r(delta_r = delta_r)
        command_iface.set_q_resolution_collimation_length(collimation_length = collimation_length)
        command_iface.set_q_resolution_use(use = True)
        ReductionSingleton().to_Q._set_up_q_resolution_parameters()

        # Assert
        a1_stored = ReductionSingleton().to_Q.get_q_resolution_a1() # in m 
        a1_expected = a1/1000.
        self.assertEqual(a1_stored, a1_expected)

        a2_stored = ReductionSingleton().to_Q.get_q_resolution_a2() # in m
        a2_expected = a2/1000.
        self.assertEqual(a2_stored, a2_expected)

        collimation_length_stored = ReductionSingleton().to_Q.get_q_resolution_collimation_length() # in m
        collimation_length_expected = collimation_length
        self.assertEqual(collimation_length_stored, collimation_length_expected)

        delta_r_stored = ReductionSingleton().to_Q.get_q_resolution_delta_r() # in m
        delta_r_expected = delta_r/1000.
        self.assertEqual(delta_r_stored, delta_r_expected)

    def test_full_setup_for_rectangular_apertures(self):
        # Arrange
        command_iface.Clean()
        command_iface.SANS2D()
        a1 = 2 # in mm
        a2 = 3 # in mm
        delta_r = 4 # in mm
        collimation_length = 10 # in m
        h1 = 9 # in mm
        w1 = 8 # in mm
        h2 = 7 # in mm
        w2 = 5 # in mm
        norm = reduction_steps.CalculateNormISIS()
        ReductionSingleton().to_Q = reduction_steps.ConvertToQISIS(norm)

        # Act
        command_iface.set_q_resolution_a1(a1 = a1)
        command_iface.set_q_resolution_a2(a2 = a2)
        command_iface.set_q_resolution_delta_r(delta_r = delta_r)
        command_iface.set_q_resolution_h1(h1 = h1)
        command_iface.set_q_resolution_w1(w1 = w1)
        command_iface.set_q_resolution_h2(h2 = h2)
        command_iface.set_q_resolution_w2(w2 = w2)
        command_iface.set_q_resolution_collimation_length(collimation_length = collimation_length)
        command_iface.set_q_resolution_use(use = True)
        ReductionSingleton().to_Q._set_up_q_resolution_parameters()

        # Assert
        a1_stored = ReductionSingleton().to_Q.get_q_resolution_a1() # in m 
        a1_expected = 2*math.sqrt((h1/1000.*h1/1000. + w1/1000.*w1/1000.)/6)
        self.assertEqual(a1_stored, a1_expected)

        a2_stored = ReductionSingleton().to_Q.get_q_resolution_a2() # in m
        a2_expected = 2*math.sqrt((h2/1000.*h2/1000. + w2/1000.*w2/1000.)/6)
        self.assertEqual(a2_stored, a2_expected)

        collimation_length_stored = ReductionSingleton().to_Q.get_q_resolution_collimation_length() # in m
        collimation_length_expected = collimation_length
        self.assertEqual(collimation_length_stored, collimation_length_expected)

        delta_r_stored = ReductionSingleton().to_Q.get_q_resolution_delta_r() # in m
        delta_r_expected = delta_r/1000.
        self.assertEqual(delta_r_stored, delta_r_expected)


    def test_full_setup_for_rectangular_apertures_which_are_only_partially_specified(self):
        # Arrange
        command_iface.Clean()
        command_iface.SANS2D()
        a1 = 2 # in mm
        a2 = 3 # in mm
        delta_r = 4 # in mm
        collimation_length = 10 # in m
        h1 = 9 # in mm
        w1 = 8 # in mm
        h2 = 7 # in mm
        # We take out w2, hence we don't have a full rectangular spec
        norm = reduction_steps.CalculateNormISIS()
        ReductionSingleton().to_Q = reduction_steps.ConvertToQISIS(norm)

        # Act
        command_iface.set_q_resolution_a1(a1 = a1)
        command_iface.set_q_resolution_a2(a2 = a2)
        command_iface.set_q_resolution_delta_r(delta_r = delta_r)
        command_iface.set_q_resolution_h1(h1 = h1)
        command_iface.set_q_resolution_w1(w1 = w1)
        command_iface.set_q_resolution_h2(h2 = h2)

        command_iface.set_q_resolution_collimation_length(collimation_length = collimation_length)
        command_iface.set_q_resolution_use(use = True)
        ReductionSingleton().to_Q._set_up_q_resolution_parameters()

        # Assert
        a1_stored = ReductionSingleton().to_Q.get_q_resolution_a1() # in m 
        a1_expected = a1/1000.
        self.assertEqual(a1_stored, a1_expected)

        a2_stored = ReductionSingleton().to_Q.get_q_resolution_a2() # in m
        a2_expected = a2/1000.
        self.assertEqual(a2_stored, a2_expected)

        collimation_length_stored = ReductionSingleton().to_Q.get_q_resolution_collimation_length() # in m
        collimation_length_expected = collimation_length
        self.assertEqual(collimation_length_stored, collimation_length_expected)

        delta_r_stored = ReductionSingleton().to_Q.get_q_resolution_delta_r() # in m
        delta_r_expected = delta_r/1000.
        self.assertEqual(delta_r_stored, delta_r_expected)


if __name__ == "__main__":
    unittest.main()
