import unittest
from mantid.simpleapi import *
from IndirectImport import *
from mantid.api import MatrixWorkspace, WorkspaceGroup

if is_supported_f2py_platform():
    class QLinesTest(unittest.TestCase):

        _sample_ws = None
        _res_ws = None
        _num_bins = None
        _num_hists = None

        def setUp(self):
            self._sample_ws = Load(Filename='irs26176_graphite002_red.nxs')
            self._res_ws = Load(Filename='irs26173_graphite002_res.nxs')
            self._num_bins = self._sample_ws.blocksize()
            self._num_hists = self._sample_ws.getNumberHistograms()


        def _validate_shape(self, result, prob, group):
            """
            Validates that the output workspaces from QLines are of the correct type, units and shape.

            @param result Result workspace from QLines
            @param prob Probability workspace from QLines
            @param group Group workspace of fitted spectra from QLines
            """

        def _validate_values(self, result, prob, group):
            """
            Validates that the output workspaces from QLines have expected values

            @param result Result workspace from QLines
            @param prob Probability workspace from QLines
            @param group Group workspace of fitted spectra from QLines
            """

            # Test values of result
            self.assertEquals(round(result.dataY(0)[0], ), )
            self.assertEquals(round(result.dataY(1)[0], ), )
            self.assertEquals(round(result.dataY(2)[0], ), )

            # Test values of probability
            self.assertEquals(round(prob.dataY(0)[0], ), )
            self.assertEquals(round(prob.dataY(1)[0], ), )
            self.assertEquals(round(prob.dataY(2)[0], ), )

            # Test values of fit group
            sub_ws = group.getItem(0)
            self.assertEquals(round(sub_ws.dataY(0)[0], ), )
            self.assertEquals(round(sub_ws.dataY(1)[0], ), )
            self.assertEquals(round(sub_ws.dataY(2)[0], ), )


        def testQLines(self):
            """
            Test QLines algorithm
            """
            QLines(SamNumber='26176',
                    ResNumber='26173',
                    InputType='Workspace',
                    ResInputType='Workspace',
                    Instrument='irs',
                    Analyser='graphite002',
                    Plot='None')
            result_ws = mtd['irs26176_graphite002_']
            prob_ws = mtd['irs26176_graphite002_']
            fit_ws_group = mtd['irs26176_graphite002_']

    if __name__=="__main__":
        unittest.main()