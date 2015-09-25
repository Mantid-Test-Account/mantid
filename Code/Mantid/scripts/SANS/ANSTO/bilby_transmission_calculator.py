from mantid.api import AlgorithmManager

class BilbyTransmissionCalculator(object):

    def __init__(self, transmission_ws, detector_ids, direct_ws, incident_beam_monitor_det_id, rebin_params=[]):
        self._transmission_ws = transmission_ws
        self._detector_ids = detector_ids
        self._direct_ws = direct_ws
        self._incident_beam_monitor_det_id = incident_beam_monitor_det_id
        self._rebin_params = rebin_params

    def execute(self, sample_ws):
        calculator = AlgorithmManager.create("CalculateTransmission")
        calculator.setChild(True)
        calculator.initialize()
        calculator.setPropertyValue("OutputWorkspace", "dummy_value")
        calculator.setProperty("SampleRunWorkspace", self._transmission_ws)
        calculator.setProperty("DirectRunWorkspace", self._direct_ws)
        calculator.setProperty("IncidentBeamMonitor", self._incident_beam_monitor_det_id)
        calculator.setProperty("RebinParams", self._rebin_params)
        calculator.setProperty("TransmissionROI", self._detector_ids)
        calculator.execute()
        fraction_ws = calculator.getProperty("OutputWorkspace").value

        multiply = AlgorithmManager.create("Multiply")
        multiply.setChild(True)
        multiply.initialize()
        multiply.setProperty("LHSWorkspace", sample_ws)
        multiply.setProperty("RHSWorkspace", fraction_ws)
        multiply.setPropertyValue("OutputWorkspace", "dummy_value")
        multiply.execute()
        scaled_ws = multiply.getProperty("OutputWorkspace").value
        return scaled_ws


