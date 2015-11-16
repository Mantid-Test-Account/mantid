from mantid.api import * # PythonAlgorithm, registerAlgorithm, WorkspaceProperty
from mantid.kernel import *
from mantid.simpleapi import *

#pylint: disable=no-init
class GetNegMuMuonicXRD(PythonAlgorithm):
    #Dictionary of <element>:<peaks> easily extendible by user.
    muonic_xr ={
               'Ag': [3184.7,3147.7,901.2,869.2,308.428,304.759],
               'As': [1866.9,1855.8,436.6,427.5],
               'Au': [8135.2,8090.6,8105.4,8069.4,5764.89,5594.97,3360.2,
                       3206.8,2474.22,2341.21,2304.44,1436.05,1391.58,1104.9,
                       899.14,869.98,405.654,400.143],
               'Ba': [3979.8, 3915.4, 1289.2, 1229.2, 436.5],
               'Bi': [6032.2, 5839.7, 2700.2, 2554.8, 997.2, 962.5, 445.6],
               'Ca': [783.8],
               'Cd': [3263.63, 3223.74, 940.58, 905.85, 321.973, 317.977],
               'Ce': [4160.3, 4087.3, 1376.7, 1314.9, 468.1, 214.2],
               'Co': [1341.8],
               'Cr': [1094.4],
               'Cs': [3899.1, 3836.1, 1241.6, 1188.6, 421.2],
               'Cu': [1512.78,1506.61,334.8,330.26],
               'Fe': [1256.4],
               'Hg': [5817.2, 5645.1, 2523.6, 2388.5, 918.1, 888.1, 412.7],
               'I' : [3721.6, 3667.6, 1146.7, 1098.0, 391.7],
               'In': [3366.27, 3322.67, 981.64, 943.39],
               'La': [4071.2, 4001.3, 1328.4, 1266.8, 452.8, 206.8],
               'Mn': [1171.2],
               'Mo': [2732.3, 2706.8, 717.8, 695.0],
               'Nb': [2626.4, 2603.2, 687.17, 665.88],
               'Nd': [4335.0, 4257.2, 1469.8, 1401.1, 498.4, 229.4],
               'Ni': [1427.4, 1422.1, 309.97],
               'Pb': [8523.3,8442.11,5966.0,5780.1,2641.8,2499.7,
                      2459.7,1511.63,1214.12,1028.83,972.3,938.4,
                      437.687,431.285],
               'Pd': [3077],
               'Pr': [4258.8, 4184.3, 1422.6, 1356.7, 486.6, 221.3],
               'Rh': [2982],
               'Sb': [3543.3, 3497.7, 1062.8, 1019.6, 360.3],
               'Sn': [3457.3,3412.8,1022.6,982.5,349.953,345.226],
               'Sr': [2241.5, 2224.0],
               'Te': [3625.6, 3575.5, 1104.3, 1060.0, 275.9],
               'Tl': [5897.9, 5716.6, 2585.0, 2446.6, 947.2, 915.1, 426.6],
               'Ti': [931.57, 191.61],
               'V' : [1011.3, 208.1],
               'Y' : [3038.63, 3033.11, 2439.38, 2420.12, 807.6, 616.38,
                        599.39, 211.5],
               'Zn': [1600.15, 1592.97, 360.75, 354.29],
               'Zr': [2535.9, 2514.9]
               
               }

    def PyInit(self):
        self.declareProperty(StringArrayProperty("Elements", values=[],
                             direction=Direction.Input
                             ))
        self.declareProperty(name="YAxisPosition",
                                    defaultValue=-0.001,
                                    doc="Position for Markers on the y-axis")
        self.declareProperty(WorkspaceGroupProperty('OutputWorkspace', '', direction=Direction.Output),
                            doc='The output workspace will always be a GroupWorkspaces '
                                'that will have the workspaces of each'
                                  ' muonicXR workspace created')

    def get_muonic_xr(self, element):
        #retrieve peak values from dictionary Muonic_XR
        peak_values = self.muonic_xr[element]
        return peak_values
        
    def validateInput(self):
        issues = dict()
        
        elements = self.getProperty('Elements').value()
        if elements == "":
            issues["Elements"] = 'No elements have been selected from the periodic table'
        y_axis_position = self.getProperty('YAxisPosition').value()
        if y_axis_position == "":
            issues["YAxisPosition"] = 'No y-shift value has been entered'
        outputworkspace_str = self.getProperty('OutputWorkspace').value()
        if outputworkspace_str == "":
            issues['OutputWorkspace'] = 'No output workspace name has been specified'
            
        return issues

    def create_muonic_xr_ws(self, element, y_pos):
        #retrieve the values from Muonic_XR
        xr_peak_values = self.get_muonic_xr(element)
        #Calibrate y-axis for workspace
        y_pos_ws = [y_pos]*len(xr_peak_values)
        xvalues = xr_peak_values
        muon_xr_ws = CreateWorkspace(xvalues, y_pos_ws[:])
        RenameWorkspaces(muon_xr_ws, WorkspaceNames="MuonXRWorkspace_"+element)
        return muon_xr_ws

    def category(self):
        return "Muon"

    def PyExec(self):
        elements = self.getProperty("Elements").value
        y_position = self.getProperty("YAxisPosition").value
        workspace_list = [None]*len(elements)
        for idx,element in enumerate(elements):
            curr_workspace = self.create_muonic_xr_ws(element, y_position)
            workspace_list[idx] = curr_workspace

        self.setProperty("OutputWorkspace", GroupWorkspaces(workspace_list))
        self.log().information(str("Created Group: "+ self.getPropertyValue("OutputWorkspace")))

AlgorithmFactory.subscribe(GetNegMuMuonicXRD)
