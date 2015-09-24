from reducer_singleton import Reducer
from mantid.api import AnalysisDataService

class BilbyReducer(Reducer):

    _steps = list()
    _output_workspace_name = ""

    def __init__(self, out_workspace_name):
        """
            Constructor
        """
        super(BilbyReducer, self).__init__()
        self._output_workspace_name = out_workspace_name

    def set_sample(self, sample):
        raise RuntimeError("Not implemented")

    def set_can(self, can):
        raise RuntimeError("Not implemented")

    def set_trans_sample(self, trans, direct):
        raise RuntimeError("Not implemented")

    def set_trans_can(self, trans, direct):
        raise RuntimeError("Not implemented")

    def _to_steps(self):
        """
            Defines the steps that are run and their order
        """
        raise RuntimeError("Not implemented")

    def _init_steps(self):
        """
            Create individual steps to run
        """
        raise RuntimeError("Not implemented")

    def pre_process(self):
        """
            Pre processing steps
        """
        raise RuntimeError("Not implemented")

    def post_process(self):
        """
            Post processing steps
        """
        raise RuntimeError("Not implemented")

    def _reduce(self, init=True, post=True, steps=None):
        """
            Execute the list of all reduction steps
            @param init: if False it assumes that the reducer is fully setup, default=True
            @param post: if to run the post run commands, default True
            @param steps: the list of ReductionSteps to execute, defaults to _reduction_steps if not set
            @return: name of the output workspace
        """

        # Do initial setup of steps, beam center etc.
        if init:
            self.pre_process()

        # Overwrite steps if demanded
        if steps:
            self._steps = steps

        # Execute all individual steps
        current_ws = None
        for step in self._steps:
            current_ws = step.execute(current_ws)

        # Make the output public
        AnalysisDataService.addOrReplace(self._output_workspace_name, current_ws)


        # Do post processing steps. Clean-up etc.
        if post:
            self.post_process()