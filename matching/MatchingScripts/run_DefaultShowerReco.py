import sys

if len(sys.argv) < 2:
    msg  = '\n'
    msg += "Usage 1: %s $INPUT_ROOT_FILE\n" % sys.argv[0]
    msg += '\n'
    sys.stderr.write(msg)
    sys.exit(1)


# from ROOT import gSystem,TMath
from larlite import larlite as fmwk
from larlite import larutil
# from recotool import argomatch, showerreco
# from ROOT import calo
from ROOT import *
# from recotool.showerDef import DefaultShowerReco3D



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
  alg.AddShowerRecoModule(showerreco.Axis3DModule()        )
  alg.AddShowerRecoModule(showerreco.StartPoint2DModule()  )
  alg.AddShowerRecoModule(showerreco.StartPoint3DModule()  )
  alg.AddShowerRecoModule(showerreco.ShowerChargeModule()  )

  alg.PrintModuleList()

  return alg

def DefaultMatch():

    palgo_array = argomatch.CPAlgoArray()
    
    palgo1 = argomatch.CPAlgoNHits()
    palgo1.SetMinHits(60)
        
    palgo_array.AddAlgo(palgo1)

    algo_array = argomatch.CFAlgoArray()
    wireAlg    = argomatch.CFAlgoShowerWireMatch()
    timeAlg = argomatch.CFAlgoTimeOverlap()
    timeAlg.RequireThreePlanes(False)
    algo_array.AddAlgo(wireAlg)
    algo_array.AddAlgo(timeAlg)

    return palgo_array, algo_array

def DefaultShowerReco3D():
    # Create analysis unit
    ana_unit = fmwk.ShowerReco3D()
    
    # Attach shower reco alg
    sralg = getShowerRecoAlgModular()
    sralg.SetDebug()
    # sralg.Verbose(False)
    # sralg.Verbose(True)
    # sralg.SetUseArea(True)
    # Attach calo alg
    # calg = calo.CalorimetryAlg()
    # sralg.CaloAlgo(calg)
    #sralg.SetUseModBox(True)
    ana_unit.AddShowerAlgo(sralg)

    # 
    # Attach Matching algorithm
    #
    palgo_array, algo_array = DefaultMatch()
    ana_unit.GetManager().AddPriorityAlgo(palgo_array)
    ana_unit.GetManager().AddMatchAlgo(algo_array)

    return ana_unit




# Create ana_processor instance
my_proc = fmwk.ana_processor()

larutil.LArUtilManager.Reconfigure(fmwk.geo.kArgoNeuT)

# Set input root file
for x in xrange(len(sys.argv)-1):
    my_proc.add_input_file(sys.argv[x+1])

# Specify IO mode
my_proc.set_io_mode(fmwk.storage_manager.kBOTH)

# Specify analysis output root file name
my_proc.set_ana_output_file("");

# Specify data output root file name
my_proc.set_output_file("elecShowers.root")

ana_unit=DefaultShowerReco3D()

ana_unit.SetInputProducer("ccMergedFinal")
ana_unit.SetOutputProducer("showerreco")

# my_proc.set_data_to_write(fmwk.data.kShower,"showerreco")
my_proc.add_process(ana_unit)

print
print  "Finished configuring ana_processor. Start event loop!"
print

#my_proc.run(0,5)
my_proc.process_event(2)

sys.exit()

