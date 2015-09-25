import mantid
from ANSTO.bilby_transmission_calculator import BilbyTransmissionCalculator
import unittest
from mantid.api import MatrixWorkspace
from mantid.simpleapi import CreateSampleWorkspace, DeleteWorkspace, ConvertUnits, Rebin


class BilbyTransmissionCalculatorTest(unittest.TestCase):

    def _create_workspace(self, name):
        ws = CreateSampleWorkspace(OutputWorkspace=name, NumBanks=1)
        ws = ConvertUnits(ws, Target='Wavelength', OutputWorkspace=name)
        ws = Rebin(ws, Params=[5,0.1,15], OutputWorkspace=name)
        return ws

    def test_executes_transmission_calculator(self):
        transmission_ws = self._create_workspace('transmission')
        direct_ws = self._create_workspace('direct')
        sample_ws = self._create_workspace('sample')

        incident_beam_monitor_det_id=100
        detector_ids=[101, 102, 103]

        calculator = BilbyTransmissionCalculator(transmission_ws, detector_ids, direct_ws, incident_beam_monitor_det_id)
        out_ws = calculator.execute(sample_ws)

        # Basic test that things ran and are wired-up correctly. Assume CalculateTransmission does what it's supposed to.
        self.assertTrue(isinstance(out_ws, MatrixWorkspace))

        DeleteWorkspace(transmission_ws)
        DeleteWorkspace(sample_ws)
        DeleteWorkspace(direct_ws)


if __name__ == '__main__':
    unittest.main()
