from reducer_singleton import Reducer


class BilbyReducer(Reducer):

    _steps = list()
    _output_workspace = ""

    def __init__(self):
        """
            Constructor
        """
        super(BilbyReducer, self).__init__()

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
        for step in self._steps:
            step.execute(self, self._output_workspace)

        # Do post processing steps. Clean-up etc.
        if post:
            self.post_process()