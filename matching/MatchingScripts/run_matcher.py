import ROOT,sys
ROOT.gSystem.Load("libClusterRecoUtil")
from ROOT import *
from ROOT import larlite as fmwk

mgr = fmwk.ana_processor()

larutil.LArUtilManager.Reconfigure(larlite.geo.kArgoNeuT)

#arg1 should be input file name
mgr.add_input_file(sys.argv[1])

mgr.set_output_file("nue_matched.root");

#larlite.storage_manager.get().set_in_rootdir("scanner")

mgr.set_input_rootdir("scanner")
mgr.set_output_rootdir("scanner")

#arg2 should be output file name
#mgr.set_output_file(sys.argv[2])

mgr.set_io_mode(fmwk.storage_manager.kBOTH)

mgr.set_ana_output_file("")

proc = fmwk.ClusterMatcher()
proc.SetClusterProducer("ccMergedStT4")
proc.SaveOutputCluster(True)

priority_algo = argomatch.CPAlgoNHits()
priority_algo.SetMinHits(20)
proc.GetManager().AddPriorityAlgo(priority_algo)

timeAlg = argomatch.CFAlgoTimeOverlap()
timeAlg.RequireThreePlanes(False)
timeAlg.SetDebug(False)

proc.GetManager().AddMatchAlgo(timeAlg)

########################################
# CMergeManager options
########################################
proc.GetManager().MergeTillConverge(True)

mgr.add_process(proc)

mgr.run()

