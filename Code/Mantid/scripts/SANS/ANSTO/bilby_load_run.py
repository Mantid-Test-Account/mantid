from mantid.api import AlgorithmManager

class BilbyLoadRun(object):

    def __init__(self, filename):
        self._filename = filename

    def setFilename(self, filename):
        self._filename = filename

    def execute(self):
        loader = AlgorithmManager.create("LoadBBY")
        loader.setChild(True)
        loader.initialize()
        loader.setProperty("Filename", self._filename)
        loader.setPropertyValue("OutputWorkspace", "dummy_entry")
        loader.execute()
        ws = loader.getProperty("OutputWorkspace").value
        return ws


