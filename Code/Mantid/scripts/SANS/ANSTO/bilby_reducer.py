from reducer_singleton import Reducer
from mantid.api import AnalysisDataService

# import steps
from bilby_load_run import BilbyLoadRun
from bilby_transmission_calculator import BilbyTransmissionCalculator
from bilby_units_convert import BilbyUnitsConvert


class BilbyReducer(Reducer):
    def __init__(self,
                 out_workspace_name,
                 sample_file,
                 can_file,
                 sample_trans_file,
                 can_trans_file,
                 direct_file):
        """
            Constructor
        """
        super(BilbyReducer, self).__init__()

        self._output_workspace_name = out_workspace_name
        self._steps = list()

        self._sample_trans_file = sample_trans_file
        self._can_trans_file = can_trans_file

        self._sample_file = sample_file
        self._can_file = can_file

        self._direct_file = direct_file

        self._init_steps()

    def set_sample(self, sample_file):
        self._sample_file = sample_file

    def set_can(self, can_file):
        self._can_file = can_file

    def set_trans_sample(self, trans_file, direct_file):
        self._sample_trans_file = trans_file
        self._direct_file = direct_file

    def set_trans_can(self, trans_file, direct_file):
        self._can_trans_file = trans_file
        self._direct_file = direct_file

    def _init_steps(self):
        """
            Create individual steps to run
        """


    def reduce(self):


        step_load_sample = BilbyLoadRun(self._sample_file)
        step_load_sample_trans = BilbyLoadRun(self._sample_trans_file)
        step_load_can = BilbyLoadRun(self._can_file)
        step_load_can_trans = BilbyLoadRun(self._can_trans_file)
        step_load_direct = BilbyLoadRun(self._direct_file)

        # Get the input workspace to use
        sample_ws = step_load_sample.execute()

        # Hack
        out_ws = sample_ws

        # Make the output public
        AnalysisDataService.addOrReplace(self._output_workspace_name, out_ws)
