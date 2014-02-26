import unittest
import numpy
from pbd import set_trace as tr
from mantid.kernel import logger
from mantid.simpleapi import CreateWorkspace, Fit, mtd
from mantid.api import AnalysisDataService

class DSFinterp1DTestTest(unittest.TestCase):

  def generateWorkspaces(self, nf, startf, df, e=False):
    '''Helper function. Generates input workspaces for testing 
    
    Generates a set of one-histogram workspaces, each containing a Lorentzian. Also
    generate a target lorentzian agains which we will fit
    
    Arguments:
      nf: number of workspaces
      startf: first theoretical HWHM
      df: separation between consecutive theoretical HWHM
      [e]: if true, the theoretical HWHM and the actual HWHM used to construct
        the lorentzian are different by a random amount.
    Returns:
      fvalues: list of theoretical HWHM
      workspaces: names of the generated workspaces
      xvalues: energy domains
      HWHM: the HWHM of the target lorentzian against which we fit, stored
        in workspace of name "targetW"
    '''
    from random import random
    n=200
    dE = 0.004 #typical spacing for QENS experiments in BASIS, in meV
    xvalues = dE * numpy.arange(-n, n)
    fvalues = ''
    workspaces = ''
    for iif in range(nf):
      f = startf + iif*df
      HWHM = f# half-width at half maximum
      # Impose uncertainty in the Lorentzian by making its actual HWHM different than the theoretical one
      if e:
        HWHM += 0.5*df - df*ramdom() # add uncertainty
      yvalues = 1/numpy.pi * HWHM / (HWHM*HWHM + xvalues*xvalues)
      evalues = yvalues*0.1*numpy.random.rand(2*n) # errors
      CreateWorkspace(OutputWorkspace='sim{0}'.format(iif), DataX=evalues, DataY=yvalues, DataE=evalues)
      fvalues += ' {0}'.format(f) # theoretical HWHM, only coincides with real HWHM when no uncertainty
      workspaces += 'sim{0}'.format(iif)
    # Target workspace against which we will fit
    HWHM = startf + (nf/2)*df
    yvalues = 1/numpy.pi * HWHM / (HWHM*HWHM + xvalues*xvalues)
    evalues = yvalues*0.1*numpy.random.rand(2*n) # errors
    CreateWorkspace(OutputWorkspace='targetW', DataX=evalues, DataY=yvalues, DataE=evalues)
    return fvalues, workspaces, xvalues, HWHM

  def cleanup(self, nf):
    for iif in range(nf):
      AnalysisDataService.remove('sim{0}'.format(iif))
    AnalysisDataService.remove('targetW')
    # Remove the fitting results, if present
    for suffix in 'NormalisedCovarianceMatrix Parameters Workspace':
      if 'targetW_{0}'.format(suffix) in mtd.keys():
        AnalysisDataService.remove('targetW_{0}'.format(suffix))

  def isthere_dsfinterp(self):
    try:
      import dsfinterp
    except:
      logger.debug('Python package dsfinterp is missing (https://pypi.python.org/pypi/dsfinterp)')
      return False
    return True

  def test_input_exceptions(self):
    ''' Test exceptions thrown upon wrong input '''
    if not isthere_dsfinterp():
      return
    import dsfinterp
    nf = 9
    fvalues, workspaces, xvalues, HWHM = self.generateWorkspaces(nf, 0.01, 0.01) # workspaces sim0 to sim8 (nine workpaces)
    # Try passing different number of workspaces and parameter values
    try:
      fvalueswrong = fvalues[1:] # one less value
      func_string = 'name=DSFinterp1DFit,Workspaces="{0}",ParameterValues="{1}",'.format(workspaces,fvalueswrong) +\
      'LoadErrors=0,LocalRegression=1,RegressionType=quadratic,RegressionWindow=6,' +\
      'WorkspaceIndex=0,Intensity=1.0,TargetParameter=0.01;'
      Fit( Function=func_string, InputWorkspace= 'targetW', WorkspaceIndex=0, StartX=xvalues[0], EndX=xvalues[-1], CreateOutput=1, MaxIterations=0 )
    except Exception as e:
      self.assertTrue('Number of Workspaces and ParameterValues should be the same' in str(e))
    else:
      assert False, "Didn't raise any exception"
    # Try passing the wrong workspace index
    try:
      func_string = 'name=DSFinterp1DFit,Workspaces="{0}",ParameterValues="{1}",'.format(workspaces,fvalues) +\
      'LoadErrors=0,LocalRegression=1,RegressionType=quadratic,RegressionWindow=6,' +\
      'WorkspaceIndex=1,Intensity=1.0,TargetParameter=0.01;'
      Fit( Function=func_string, InputWorkspace= 'targetW', WorkspaceIndex=0, StartX=xvalues[0], EndX=xvalues[-1], CreateOutput=1, MaxIterations=0 )
    except Exception as e:
      self.assertTrue('Numer of histograms in Workspace sim0 does not allow for workspace index 1' in str(e))
    else:
      assert False, "Didn't raise any exception"
    #Try passing the wrong type of regression
    try:
      func_string = 'name=DSFinterp1DFit,Workspaces="{0}",ParameterValues="{1}",'.format(workspaces,fvalues) +\
      'LoadErrors=0,LocalRegression=1,RegressionType=baloney,RegressionWindow=6,' +\
      'WorkspaceIndex=1,Intensity=1.0,TargetParameter=0.0;'
      Fit( Function=func_string, InputWorkspace= 'targetW', WorkspaceIndex=0, StartX=xvalues[0], EndX=xvalues[-1], CreateOutput=1, MaxIterations=0 )
    except Exception as e:
      self.assertTrue('Regression type baloney not implemented' in str(e))
    else:
      assert False, "Didn't raise any exception"
    # Try passing an unappropriate regression window for the regression type selected
    try:
      func_string = 'name=DSFinterp1DFit,Workspaces="{0}",ParameterValues="{1}",'.format(workspaces,fvalues) +\
      'LoadErrors=0,LocalRegression=1,RegressionType=quadratic,RegressionWindow=3,' +\
      'WorkspaceIndex=1,Intensity=1.0,TargetParameter=0.0;'
      Fit( Function=func_string, InputWorkspace= 'targetW', WorkspaceIndex=0, StartX=xvalues[0], EndX=xvalues[-1], CreateOutput=1, MaxIterations=0 )
    except Exception as e:
      self.assertTrue('RegressionWindow must be equal or bigger than 4 for regression type quadratic' in str(e))
    else:
      assert False, "Didn't raise any exception"
    self.cleanup(nf)

  def test_LorentzianFit(self):
    ''' Fit a set or Lorentzians with "noisy" HWHM against a reference lorentzian '''
    if not isthere_dsfinterp():
      return
    import dsfinterp
    nf=20
    startf=0.01
    df=0.01
    fvalues, workspaces, xvalues, HWHM = self.generateWorkspaces(nf, startf, df, e=True)
    # Do the fit starting from different initial guesses
    for iif in range(0,nf,3):
      guess = startf + iif*df
      func_string = 'name=DSFinterp1DFit,Workspaces="{0}",ParameterValues="{1}",'.format(workspaces,fvalues) +\
      'LoadErrors=0,LocalRegression=1,RegressionType=quadratic,RegressionWindow=6,' +\
      'WorkspaceIndex=0,Intensity=1.0,TargetParameter={0};'.format(guess)
      Fit( Function=func_string, InputWorkspace= 'targetW', WorkspaceIndex=0, StartX=xvalues[0], EndX=xvalues[-1], CreateOutput=1, MaxIterations=20 )
      ws = mtd['targetW_Parameters']
      tr()
      optimizedf = ws.row(0)[ 'Value' ]
      self.assertTrue( abs(1.0 - optimizedf/HWHM) < 0.01) #one percent error
    self.cleanup(nf)

if __name__=="__main__":
	unittest.main()
