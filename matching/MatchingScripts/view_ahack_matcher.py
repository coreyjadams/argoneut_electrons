# Load libraries
import ROOT, sys, os
from ROOT import *

# Now import ana_processor & your class. For this example, ana_base.
#gSystem.Load("libCMTool")
from ROOT import *
if len(sys.argv) != 2:
    print
    print "*** Improper usage. Usage: python viewclusters.py /path/to/input/file.root ***"
    print


filename = sys.argv[1]
#filename = "/Users/davidkaleko/Data/ShowerStudy/PDGID_11/shower_larlight_11.root"
my_proc = larlite.ana_processor()
#my_proc.set_verbosity(larlite.MSG.kDEBUG)

my_proc.set_io_mode(larlite.storage_manager.kREAD)

my_proc.add_input_file(filename)

larutil.LArUtilManager.Reconfigure(larlite.geo.kArgoNeuT)

larlite.storage_manager.get().set_in_rootdir("scanner")

my_proc.set_ana_output_file("")

raw_viewer   = larlite.ClusterViewer()
match_viewer = larlite.MatchViewer()
#mc_viewer    = larlite.MCShowerClusterViewer()
clusterz = cluster.CRUHelper()

match_viewer.SetPrintClusterInfo(True)
#match_viewer.ShowShowers(False)
#match_viewer.SetDrawShowers(True)
########################################
# attach match algos here
########################################


priority_algo = cmtool.CPAlgoNHits()
priority_algo.SetMinHits(20)
match_viewer.GetManager().AddPriorityAlgo(priority_algo)

algo_array = cmtool.CFAlgoArray()

#timeAlg = cmtool.CFAlgoTimeOverlap()
#timeAlg.SetDebug(False)
#timeAlg.RequireThreePlanes(False)

showerAlg = cmtool.CFAlgoShowerTimeMatch()
wireAlg = cmtool.CFAlgoShowerWireMatch()
wireAlg.SetDebug(False)

qualityAlg = cmtool.CFAlgoMatchStart()

algo_array.AddAlgo(showerAlg)
#algo_array.AddAlgo(wireAlg)
#algo_array.AddAlgo(qualityAlg)

match_viewer.GetManager().AddMatchAlgo(algo_array)

########################################
# done attaching match algos
########################################

my_proc.add_process(raw_viewer)

my_proc.add_process(match_viewer)

#my_proc.add_process(mc_viewer)

#producer="ccMergedCone"
producer="ccMergedFinal"
#producer="ccMergedPoly3"

raw_viewer.SetClusterProducer(producer) #larlite.DATA.Cluster)
#raw_viewer.SetClusterProducer(larlite.DATA.MCShowerCluster)

match_viewer.SetClusterProducer(producer) #larlite.DATA.Cluster)
#match_viewer.SetClusterProducer(larlite.DATA.MCShowerCluster)

gStyle.SetOptStat(0)

####
# want to mark begin points
####
#algo = cluster.ClusterParamsExecutor()
#result = algo.GetParams()



#start on first event always
user_input_evt_no = -1;

counter = 0

while true:

    try:
        user_input_evt_no = input('Hit Enter to continue to next evt, or type in an event number to jump to that event:')
    except SyntaxError:
        user_input_evt_no = user_input_evt_no + 1

    print 'Next Event: '
    my_proc.process_event(user_input_evt_no)

    raw_viewer.DrawAllClusters();

    match_viewer.DrawAllClusters();

    #mc_viewer.DrawAllClusters();

    print "Event ID is: " ,counter
    counter+=1
    print "    Hit enter to go next event..."
    sys.stdin.readline()


# done!
