import mantid
from ANSTO.bilby_Q1D import BilbyQ1D
from mantid.simpleapi import CreateSampleWorkspace, ConvertUnits, DeleteWorkspace, Rebin
import unittest


class BilbyQ1DTest(unittest.TestCase):

    def test_conversion(self):

        ws = CreateSampleWorkspace(NumBanks=1)
        ws = ConvertUnits(ws, Target='Wavelength')
        ws = Rebin(ws, Params=[1, 0.1, 15])

        step = BilbyQ1D(q_binning=[0,0.1,1])
        ws_in_q = step.execute(ws)

        # Check output units are in momentum transfer
        unit = ws_in_q.getAxis(0).getUnit()
        self.assertEquals("MomentumTransfer", unit.unitID())

        DeleteWorkspace(ws)


if __name__ == '__main__':
    unittest.main()
