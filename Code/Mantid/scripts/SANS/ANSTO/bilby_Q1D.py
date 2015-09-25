from mantid.api import AlgorithmManager

class BilbyQ1D(object):

    def __init__(self, q_binning):
        self._q_binning = q_binning

    def execute(self, sample_ws):
        q_alg = AlgorithmManager.create("Q1D")
        q_alg.setChild(True)
        q_alg.initialize()
        q_alg.setProperty("DetBankWorkspace", sample_ws)
        q_alg.setProperty("OutputBinning", self._q_binning)
        q_alg.setProperty("AccountForGravity", True)
        q_alg.setProperty("SolidAngleWeighting", True)
        q_alg.setPropertyValue("OutputWorkspace", "dummy_value")
        q_alg.execute()
        q_ws = q_alg.getProperty("OutputWorkspace").value
        return q_ws

