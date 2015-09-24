from mantid.api import AlgorithmManager

class BilbyUnitsConvert(object):

    def __init__(self, min=None, max=None, step=None):
        self._min = min
        self._max = max
        self._step = step
        if min and max:
            if min >= max:
                raise ValueError("Min must be < Max")
            if step and step > (max - min):
                raise ValueError("Step is larger than range")

    def execute(self, ws):
        converter = AlgorithmManager.create("ConvertUnits")
        converter.initialize()
        converter.setChild(True)
        converter.setProperty("InputWorkspace", ws)
        converter.setProperty("Target", "Wavelength")
        converter.setProperty("OutputWorkspace", "dummy_value")
        converter.execute()
        converted = converter.getProperty("OutputWorkspace").value

        if self._min and self._max and self._step:
            rebin = AlgorithmManager.create("Rebin")
            rebin.setChild(True)
            rebin.initialize()
            rebin.setProperty("InputWorkspace", converted)
            rebin.setProperty("Params", [self._min, self._step, self._max])
            rebin.setProperty("OutputWorkspace", "dummy_value")
            rebin.execute()
            converted = rebin.getProperty("OutputWorkspace").value

        return converted




