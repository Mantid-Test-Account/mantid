import mantid
import unittest

from ANSTO.bilby_load_run import BilbyLoadRun

class LoadRunBilbyTest(unittest.TestCase):


    def test_load(self):
        step = BilbyLoadRun("BBY0000014.tar")
        ws = step.execute()
        self.assertEquals("BILBY", ws.getInstrument().getName())


if __name__ == '__main__':
    unittest.main()
