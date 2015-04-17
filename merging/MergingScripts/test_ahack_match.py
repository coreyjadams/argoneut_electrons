import ROOT,sys
#ROOT.gSystem.Load("libCMTool")
from ROOT import *
from ROOT import larlite as fmwk

mgr = fmwk.ana_processor()


mgr.add_input_file(sys.argv[1])

larlite.storage_manager.get().set_in_rootdir("scanner")
#mgr.set_input_rootdir("scanner")

mgr.set_output_file("")

mgr.set_io_mode(fmwk.storage_manager.kREAD)

mgr.set_ana_output_file("")

mgr.set_output_file("nue_matched.root");

larutil.LArUtilManager.Reconfigure(larlite.geo.kArgoNeuT)

producer = "ccMergedStT4"
proc = fmwk.ClusterMatcher()
proc.SetClusterProducer(producer)

#priority_algo = cmtool.CPAlgoPolyArea()
#priority_algo.SetMinArea(1e9)

priority_algo = cmtool.CPAlgoNHits()
priority_algo.SetMinHits(20)
proc.GetManager().AddPriorityAlgo(priority_algo)

timeAlg = cmtool.CFAlgoTimeOverlap()
timeAlg.RequireThreePlanes(False)
timeAlg.SetDebug(False)


proc.GetManager().AddMatchAlgo(timeAlg)

#proc.GetManager().DebugMode(cmtool.CMatchManager.kPerIteration)
#proc.GetManager().DebugMode(cmtool.CMatchManager.kPerAlgoSet)
#proc.GetManager().DebugMode(cmtool.CMatchManager.kPerMerging)

mgr.add_process(proc)

print "Start..."

while mgr.process_event():

    print 

    bk=proc.GetManager().GetBookKeeper()

    res = bk.GetResult()

    print "Found %d matched pairs ... listing each pair below..." % res.size()

    for x in xrange(res.size()):

        for y in xrange(res.at(x).size()):

            print res.at(x).at(y),
        print

    print "Waiting user hitting an enter..."
    sys.stdin.readline()



