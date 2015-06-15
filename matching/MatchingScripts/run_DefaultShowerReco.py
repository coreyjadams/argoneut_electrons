import sys

if len(sys.argv) < 2:
    msg  = '\n'
    msg += "Usage 1: %s $INPUT_ROOT_FILE\n" % sys.argv[0]
    msg += '\n'
    sys.stderr.write(msg)
    sys.exit(1)


from ROOT import gSystem,TMath
from larlite import larlite as fmwk
from larlite import larutil
from recotool import cmtool, showerreco
from ROOT import calo
# from recotool.showerDef import DefaultShowerReco3D



def DefaultMatch():

    palgo_array = cmtool.CPAlgoArray()
    
    palgo1 = cmtool.CPAlgoNHits()
    palgo1.SetMinHits(25)
    
    # palgo2 = cmtool.CPAlgoIgnoreTracks()
    
    palgo_array.AddAlgo(palgo1)

    algo_array = cmtool.CFAlgoArray()
    showerAlg  = cmtool.CFAlgoShowerTimeMatch()
    wireAlg    = cmtool.CFAlgoShowerWireMatch()
    qualityAlg = cmtool.CFAlgoMatchStart()
    algo_array.AddAlgo(showerAlg)
    algo_array.AddAlgo(wireAlg)
    algo_array.AddAlgo(qualityAlg)

    return palgo_array, algo_array


def DefaultShowerReco3D():
    # Create analysis unit
    ana_unit = fmwk.ShowerReco3D()
    
    # Attach shower reco alg
    sralg = showerreco.ShowerRecoAlg()
    sralg.Verbose(False)
    # sralg.Verbose(True)
    sralg.SetUseArea(True)
    # Attach calo alg
    calg = calo.CalorimetryAlg()
    sralg.CaloAlgo(calg)
    #sralg.SetUseModBox(True)
    ana_unit.SetShowerAlgo(sralg)

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

my_proc.set_data_to_write(fmwk.data.kShower,"showerreco")
my_proc.add_process(ana_unit)

print
print  "Finished configuring ana_processor. Start event loop!"
print

#my_proc.run(0,5)
my_proc.run()

sys.exit()

