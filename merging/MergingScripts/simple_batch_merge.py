# Load libraries
import ROOT, sys, os
from ROOT import *



my_proc = larlite.ana_processor()
my_proc.set_verbosity(larlite.msg.kDEBUG)

my_proc.set_io_mode(larlite.storage_manager.kBOTH)

for x in xrange(len(sys.argv)-1):
    print "adding ", sys.argv[x+1]
    my_proc.add_input_file(sys.argv[x+1])

my_proc.set_output_file("out2.root")
larutil.LArUtilManager.Reconfigure(larlite.geo.kArgoNeuT)

merger = larlite.ClusterMerger()

########################################
# attach merge algos here
########################################


########################################
# Remove tracks with priority algo!
########################################
# priority_alg = cmtool.CPAlgoIgnoreTracks()
# priority_alg.SetMinCharge(10000)
# priority_alg.SetMinHits(10)
# merge_viewer.GetManager().AddPriorityAlgo(priority_alg)



########################################
# PROHIBIT ALGORITHMS
########################################
prohib_array = cmtool.CBAlgoArray()

# tracksep_prohibit = cmtool.CBAlgoTrackSeparate()
# tracksep_prohibit.SetDebug(False)
# tracksep_prohibit.SetVerbose(False)
# tracksep_prohibit.SetUseEP(True)
# prohib_array.AddAlgo(tracksep_prohibit,False)

# outofcone_prohibit = cmtool.CBAlgoOutOfConeSeparate()
# outofcone_prohibit.SetDebug(False)
# outofcone_prohibit.SetVerbose(False)
# outofcone_prohibit.SetMaxAngleSep(20.)
# prohib_array.AddAlgo(outofcone_prohibit,False)

angle_prohibit = cmtool.CBAlgoProhibitBigToBig()
#this only applies if both clusters have >15 hits
# angle_prohibit.SetMinHits(10)
# angle_prohibit.SetAllow180Ambig(True)
# angle_prohibit.SetUseOpeningAngle(False)
# #this prohibits clusters w/ angles different than 10 degrees
# angle_prohibit.SetAngleCut(5.)
# angle_prohibit.SetMinLength(10.)
# angle_prohibit.SetDebug(False)
prohib_array.AddAlgo(angle_prohibit,False)

# merge_viewer.GetManager().AddSeparateAlgo(prohib_array)

########################################
# MERGE ALGORITHMS
########################################
algo_array = cmtool.CBAlgoArray()

# COM_algo = cmtool.CBAlgoCenterOfMassSmall()
# COM_algo.SetDebug(False)
# COM_algo.SetVerbose(False)
# COM_algo.UseCOMInPoly(True)
# COM_algo.UseCOMClose(True)
# COM_algo.UseCOMNearClus(True)
# COM_algo.SetMaxDistance(20.)
# COM_algo.SetMaxCOMDistance(20.)
# COM_algo.SetMaxHitsSmallClus(40)
# algo_array.AddAlgo(COM_algo,False)

# ALL_algo = cmtool.CBAlgoMergeSmallToTrack()
# algo_array.AddAlgo(ALL_algo)
# POLY_algo = cmtool.CBAlgoPolyOverlap()
# algo_array.AddAlgo(POLY_algo)
algo_array.AddAlgo(cmtool.CBAlgoMergeSingleToBig())

merger.GetManager().AddMergeAlgo(algo_array)
merger.GetManager().AddSeparateAlgo(prohib_array)

# done attaching merge algos
########################################
# merger.GetManager().MergeTillConverge(True)



my_proc.add_process(merger)

merger.SetInputProducer("ccclusterWithSingles")
merger.SetOutputProducer("ccclusterMerged1")
merger.SaveOutputCluster()


my_proc.run()

# my_proc.process_event(0)

# gStyle.SetOptStat(0)

# #start on first event always
# user_input_evt_no = -1;

# while true:

#     try:
#         user_input_evt_no = input('Hit Enter to continue to next evt, or type in an event number to jump to that event:')
#     except SyntaxError:
#         user_input_evt_no = user_input_evt_no + 1

#     my_proc.process_event(user_input_evt_no)

#     raw_viewer.DrawAllClusters();

#     merge_viewer.DrawAllClusters();


# #    for plane in xrange(larutil.Geometry.GetME().Nplanes()):
# #
# #        print "    Plane:", plane
# #        
# #        for cindex in xrange(merge_viewer.ClusterCount(plane)):
# #
# #            print "        Cluster:",cindex
# #            merge_viewer.DrawOneCluster(plane,cindex)
# #            sys.stdin.readline()
# #    

#     print "    Hit enter to go next event..."
#     sys.stdin.readline()


# done!
