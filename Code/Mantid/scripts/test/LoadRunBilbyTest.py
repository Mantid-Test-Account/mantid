import mantid
import unittest

from ANSTO.LoadRunBilby import LoadRunBilby

class LoadRunBilbyTest(unittest.TestCase):

    def test_load(self):
        step = LoadRunBilby("BBY0000014.tar")
        ws = step.execute()
        self.assertEquals("BILBY", ws.getInstrument().getName())


if __name__ == '__main__':
    unittest.main()
