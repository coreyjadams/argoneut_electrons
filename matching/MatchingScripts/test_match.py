import ROOT,sys
ROOT.gSystem.Load("libCMTool")
from ROOT import *
from ROOT import larlight as fmwk

mgr = fmwk.ana_processor()

mgr.add_input_file(sys.argv[1])

mgr.set_input_rootdir("scanner")

mgr.set_output_file("")

mgr.set_io_mode(fmwk.storage_manager.READ)

mgr.set_ana_output_file("")

proc = fmwk.ClusterMatcher()

#priority_algo = argomatch.CPAlgoPolyArea()
#priority_algo.SetMinArea(1e9)

#proc.GetManager().AddPriorityAlgo(priority_algo)

proc.GetManager().AddMatchAlgo(argomatch.CFAlgoZOverlap())

#proc.GetManager().DebugMode(argomatch.CMatchManager.kPerIteration)
#proc.GetManager().DebugMode(argomatch.CMatchManager.kPerAlgoSet)
proc.GetManager().DebugMode(argomatch.CMatchManager.kPerMerging)

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



