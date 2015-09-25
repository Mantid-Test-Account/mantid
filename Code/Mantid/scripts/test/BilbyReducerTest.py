import mantid
from mantid.api import WorkspaceFactory, AnalysisDataService
from ANSTO.bilby_reducer import BilbyReducer
from reducer_singleton import Reducer
import unittest

class BilbyReducerTest(unittest.TestCase):

    def test_setup(self):
        reducer = BilbyReducer(out_workspace_name="test", sample_file="BBY0000014.tar", can_file="BBY0000014.tar", sample_trans_file="BBY0000014.tar", can_trans_file="BBY0000014.tar", direct_file="BBY0000014.tar")
        reducer.reduce()

if __name__ == '__main__':
    unittest.main()
