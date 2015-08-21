from mantid.api import * # PythonAlgorithm, registerAlgorithm, WorkspaceProperty
from mantid.kernel import *
from mantid.simpleapi import *

#pylint: disable=no-init
class GetNegMuMuonicXRD(PythonAlgorithm):
    #Dictionary of <element>:<peaks> easily extendible by user.
    Muonic_XR={'Au' :[8135.2,8090.6,8105.4,8069.4,5764.89,5594.97,3360.2,
                       3206.8,2474.22,2341.21,2304.44,1436.05,1391.58,1104.9,
                       899.14,869.98,405.654,400.143],
               'Ag': [3184.7,3147.7,901.2,869.2,308.428,304.759],
               'Cu' :[1512.78,1506.61,334.8,330.26],
               'Zn' :[1600.15,1592.97,360.75,354.29],
               'Pb' :[8523.3,8442.11,5966.0,5780.1,2641.8,2499.7,
                        2459.7,1511.63,1214.12,1028.83,972.3,938.4,
                        437.687,431.285],
               'As' : [1866.9,1855.8,436.6,427.5],
               'Sn' : [3457.3,3412.8,1022.6,982.5,349.953,345.226]}

    def PyInit(self):
        self.declareProperty(StringArrayProperty("Elements", values=[],
                             direction=Direction.Input
                             ))
        self.declareProperty(name="YAxisPosition",
                                    defaultValue=-0.001,
                                    doc="Position for Markers on the y-axis")

    def get_Muonic_XR(self, element):
        #retrieve peak values from dictionary Muonic_XR
        peak_values = self.Muonic_XR[element]
        return peak_values

    def Create_MuonicXR_WS(self, element, YPos):
        #retrieve the values from Muonic_XR
        xr_peak_values = self.get_Muonic_XR(element)
        #Calibrate y-axis for workspace
        YPos_WS = [YPos]*len(xr_peak_values)
        xvalues = xr_peak_values
        WS_Muon_XR = CreateWorkspace(xvalues, YPos_WS[:])
        RenameWorkspaces(WS_Muon_XR, WorkspaceNames="MuonXRWorkspace_"+element)
        return WS_Muon_XR


    def category(self):
        return "Muon"

    def PyExec(self):
        elements = self.getProperty("Elements").value
        yposition = self.getProperty("YAxisPosition").value
        workspace_list = [None]*len(elements)

        for idx,element in enumerate(elements):
            curr_workspace = self.Create_MuonicXR_WS(element, yposition)
            workspace_list[idx] = curr_workspace
        if len(elements) == 1:
            MuonXRWorkspace = workspace_list[0]
            self.log().information(str("Created: "+MuonXRWorkspace.name()))
        else:
            MuonicXR_group = GroupWorkspaces(workspace_list)
            self.log().information(str("Created Group: "+MuonicXR_group.name()))

AlgorithmFactory.subscribe(GetNegMuMuonicXRD)