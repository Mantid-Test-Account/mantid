import sys
sys.path.append("C:\\Mantid\\Git\\code\\mantid\\scripts\\sans") # HACK
sys.path.append( "C:\\Mantid\\Git\\code\\mantid\\scripts") # HACK

from ANSTO.bilby_reducer import BilbyReducer
from reducer_singleton import Reducer
import unittest


class BilbyReducerTest(unittest.TestCase):

    def test_construction(self):
        reducer = BilbyReducer()
        self.assertTrue(isinstance(reducer, Reducer))

    def test_executes_everything(self):

        class FakeStep:
            self.__executed = False
            def execute(self, parent, output_workspace_name):
                self._executed = True

        reducer = BilbyReducer()
        steps = [FakeStep(), FakeStep()] # Provide our own steps
        reducer._reduce(init=False, post=False, steps=steps)
        for step in reducer._steps:
            self.assertTrue(step._executed)




if __name__ == '__main__':
    print sys.path
    unittest.main()
