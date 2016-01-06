

# This class defines the shower reco algorithms, their parameters, and returns
# a configuration to use.  

# Import the libs needed:
# from cpp_classes import argomerge
from ROOT import larlite, cmtool, showerreco


def getShowerRecoAlgModular():
  # This function returns the default shower reco module
  # If you want to extend, customize, or otherwise alter the default
  # reco (which you should!) it's recommended that you do one of two
  # things:
  #   1) Copy this function, and change the lines you need
  #   2) Use this function as is and replace the modules with the built-in functions
  #       to ShowerRecoAlgModular
  # Then, use the return value (alg) in place of ShowerRecoAlg in your python script

  # It should be noted, however, that this "default" set of modules is not yet developed
  # and if you are developing you ought to be updating it here!

  alg = showerreco.ShowerRecoAlgModular()
  alg.SetDebug(False)

  # 3D Axis Module:
  axis3D = showerreco.Axis3DModule()
  axis3D.setMaxIterations(100)
  axis3D.setNStepsInitial(25)
  axis3D.setTargetError(0.001)
  axis3D.setNormalErrorRange(0.01)
  axis3D.setThetaRangeStart(0.1)
  axis3D.setThetaRangeMin(0.0005)
  axis3D.setNStepsStart(4)
  axis3D.setConvergeRate(0.85)
  axis3D.setVerbosity(True)
  axis3D.setSeedVectorErrorCutoff(0.1)

  energy = showerreco.LinearEnergy()
  energy.SetUseModBox(True)
  energy.setVerbosity(False)

  dqdx = showerreco.dQdxModule()

  dedx = showerreco.dEdxFromdQdx()
  dedx.SetUsePitch(False)
  dedx.setVerbosity(False)

  # alg.AddShowerRecoModule(axis3D)
  alg.AddShowerRecoModule(showerreco.AxisFromTracks())
  # alg.AddShowerRecoModule(showerreco.StartPoint3DModule()  )
  # alg.AddShowerRecoModule(energy)
  # alg.AddShowerRecoModule(dqdx)
  # alg.AddShowerRecoModule(dedx)
  # alg.AddShowerRecoModule(showerreco.StartPoint2DModule()  )
  #alg.AddShowerRecoModule(showerreco.OtherStartPoint3D()  )
  # alg.AddShowerRecoModule(showerreco.ShowerChargeModule()  )

  # alg.AddShowerRecoModule(showerreco.GeoModule())

  alg.PrintModuleList()

  return alg

