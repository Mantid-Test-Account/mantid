import mantid
from mantid.api import AlgorithmManager
from ANSTO.bilby_units_convert import BilbyUnitsConvert
import unittest


class BilbyUnitsConvertTest(unittest.TestCase):

    _ws = None

    def __init__(self, *args, **kwargs):
        super(BilbyUnitsConvertTest, self).__init__(*args, **kwargs)

        if not self._ws:
            loader = AlgorithmManager.create("LoadBBY")
            loader.setChild(True)
            loader.initialize()
            loader.setProperty("Filename", "BBY0000014.tar")
            loader.setPropertyValue("OutputWorkspace", "dummy_entry")
            loader.execute()
            self._ws = loader.getProperty("OutputWorkspace").value

    def test_bad_min_max_throws(self):
        self.assertRaises(ValueError, BilbyUnitsConvert, min=2, max=2, step=1)

    def test_bad_step_throws(self):
        self.assertRaises(ValueError, BilbyUnitsConvert, min=1, max=2, step=10)

    def test_units_conversion_no_rebin(self):

        wavelength_converter = BilbyUnitsConvert()
        out_ws = wavelength_converter.execute(self._ws)

        unit = out_ws.getAxis(0).getUnit()
        self.assertEquals("Wavelength", unit.unitID())


    def test_units_conversion_with_rebin(self):

        minimum = 2
        maximum = 10
        step = 1
        wavelength_converter = BilbyUnitsConvert(min=minimum, max=maximum, step=step)
        out_ws = wavelength_converter.execute(self._ws)

        x_output = out_ws.readX(0)
        self.assertEquals(minimum, x_output[0])
        self.assertEquals(maximum, x_output[-1])
        self.assertEquals(step, x_output[1] - x_output[0])

if __name__ == '__main__':
    unittest.main()
