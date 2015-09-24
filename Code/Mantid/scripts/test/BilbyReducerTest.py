import mantid
from mantid.api import WorkspaceFactory, AnalysisDataService
from ANSTO.bilby_reducer import BilbyReducer
from reducer_singleton import Reducer
import unittest

class BilbyReducerTest(unittest.TestCase):

    def test_construction(self):
        reducer = BilbyReducer('Q_output')
        self.assertTrue(isinstance(reducer, Reducer))

    def test_executes_everything(self):

        class FakeStep:
            self.__executed = False
            def execute(self, in_ws):
                self._executed = True
                return WorkspaceFactory.createTable()

        reducer = BilbyReducer('Q_output')
        steps = [FakeStep(), FakeStep()] # Provide our own steps
        reducer._reduce(init=False, post=False, steps=steps)
        for step in reducer._steps:
            self.assertTrue(step._executed)

        AnalysisDataService.remove('Q_output')


if __name__ == '__main__':
    unittest.main()
