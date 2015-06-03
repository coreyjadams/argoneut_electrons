#!/usr/bin/env python

# Load libraries
import ROOT, sys, os
from ROOT import *
from clusterCrawlerShowerArgo import *
import argparse


def main(**args):
  

  my_proc = larlite.ana_processor()
  # my_proc.enable_event_alignment(False)

  if args['verbose']:
      print "Verbose mode turned on."
      if args['source'] != None:
          print "\tSource file is " + args['source']
      if args['data_output'] != None:
          print "\tData output file is " + args['data_output']
      if args['ana_output'] != None:
          print "\tAna output file is " + args['ana_output'] 
      # my_proc.set_verbosity(larlite.msg.kDEBUG)
  # else:
  my_proc.set_verbosity(larlite.msg.kERROR)

  if args['source'] == None:
      print "Error: please specificy an input file with -s or --source."
      quit()

  if args['data_output'] == None:
      args['data_output'] = "default_event_output.root"
      if args['verbose']:
        print "No event output file selected.  If necessary, output will go to:"
        print "\t"+args['data_output']

  if args['ana_output'] == None:
      args['ana_output'] = "default_ana_output.root"
      if  args['verbose']:
        print "No ana output file selected.  If necessary, output will go to:"
        print "\t"+args['ana_output']



  my_proc.set_io_mode(larlite.storage_manager.kBOTH)

  for x in args['source']:
      my_proc.add_input_file(x)

  my_proc.set_output_file(args['data_output'])
  larutil.LArUtilManager.Reconfigure(larlite.geo.kArgoNeuT)

  mergers = []
  prevProducer = "cccluster"

  # ##################################
  # # Stage 1:
  # ##################################

  # # Module to take all of the poorly done, vertical tracks and destroy them
  # htc = larlite.DropBadVertClusters()
  # htc.SetInputProducer(prevProducer)
  # htc.SetOutputProducer("ccNoVert")
  # prevProducer = "ccNoVert"
  # my_proc.add_process(htc)

  # # Module to turn all the single hits into one hit clusters
  # htc = larlite.HitToCluster()
  # htc.SetInputProducer(prevProducer)
  # htc.SetOutputProducer("ccclusterWithSingles")
  # prevProducer="ccclusterWithSingles"
  # my_proc.add_process(htc)

  # # Trying an iterative merging approach:
  # maxClosestDistances = [0.5,   0.8,  1.0,  1.2 ]
  # maxAverageDistances = [9999,  9999, 9999, 9999]
  # maxHitsSmall        = [1,     1,    3,    3   ]
  # maxHitsProhibit     = [1,     1,    3,    3   ]
  # minHitsInCluster    = [1,     1,    1,    3   ]


  # for i in range(0, len(maxClosestDistances)):
  #   mergers.append(getSmallClustMerger(
  #       maxHitsSmall  = maxHitsSmall[i],
  #       maxHitsProhib = maxHitsProhibit[i],
  #       maxDist       = maxClosestDistances[i],
  #       maxDistAv     = maxAverageDistances[i],
  #       minHits       = minHitsInCluster[i]))
  #   mergers[-1].SetInputProducer(prevProducer)
  #   mergers[-1].SetOutputProducer("ccMergedSmall" + str(i))
  #   mergers[-1].SaveOutputCluster()
  #   prevProducer = "ccMergedSmall" + str(i)
  #   my_proc.add_process(mergers[-1])


  # # Add the inline merger:
  # inlineMerger = getInlineMerger(maxInlineDist=0.5)
  # inlineMerger.SetInputProducer(prevProducer)
  # inlineMerger.SetOutputProducer("ccMergedInline")
  # prevProducer = "ccMergedInline"
  # inlineMerger.SaveOutputCluster()
  # my_proc.add_process(inlineMerger)

  # #################################
  # # Stage 2:
  # #################################


  # prevProducer = "ccMergedInline"

  # overlapFraction    = [0.9, 0.8,]
  # minHitsForConsid   = [3,   6,  ]
  # maxHitsProhibit    = [15,  30, ]


  # for i in range(0, len(overlapFraction)):
  #   mergers.append(getOverlapMerger(
  #       overlapFrac = overlapFraction[i],
  #       minHits     = minHitsForConsid[i],
  #       maxHits     = maxHitsProhibit[i]))
  #   mergers[-1].SetInputProducer(prevProducer)
  #   mergers[-1].SetOutputProducer("ccMergedOverlap" + str(i))
  #   mergers[-1].SaveOutputCluster()
  #   prevProducer = "ccMergedOverlap" + str(i)
  #   my_proc.add_process(mergers[-1])




  # ##################################
  # # Stage 3:
  # ##################################


  #   prevProducer = "ccMergedOverlap1"

  #   maxClosestDistances = [0.8, 1.0, 1.2, 0.8, 1.0, 1.2, 0.8, 1.0, 1.2, ]
  #   maxClusterSizes     = [10,  10,  10,  20,  20,  20,  30,  30,  30 ,]
  # # 1.0, 1.2, 1.4, 1.4
  # # 15,  25,  30 , 40 
  #   for i in range(0, len(maxClosestDistances)):
  #     mergers.append(getMergeToTrunk(
  #       shortestDist = maxClosestDistances[i],
  #       maxClusterSize = maxClusterSizes[i],
  #       prohibitBig=True))
  #     mergers[-1].SetInputProducer(prevProducer)
  #     mergers[-1].SetOutputProducer("ccMergedSDnoBig" + str(i))
  #     mergers[-1].SaveOutputCluster()
  #     prevProducer = "ccMergedSDnoBig" + str(i)
  #     my_proc.add_process(mergers[-1])

  # ##################################
  # # Stage 4:
  # ##################################

  # prevProducer = "ccMergedSDnoBig8"


  # _maxInlineDist   =  [0.8, 1.2, ]
  # _hitFraction     =  [.50, 0.5, ]

  # _maxDiffuseDist   = [1.2, 1.2, ]
  # _hitFractionDif   = [.3,  0.2, ]

  # # Add the inline and diffuse merger:
  # for i in range(0, len(_maxInlineDist)):
  #   mergers.append(getInlineMerger(
  #     maxInlineDist=_maxInlineDist[i], 
  #     useAllHits=False,
  #     hitFraction=_hitFraction[i]))
  #   mergers[-1].SetInputProducer(prevProducer)
  #   mergers[-1].SetOutputProducer("ccMergedInline" + str(i))
  #   prevProducer = "ccMergedInline" + str(i)
  #   mergers[-1].SaveOutputCluster()
  #   my_proc.add_process(mergers[-1])
  #   mergers.append(getDiffuseMerger(
  #     maxInlineDist=_maxDiffuseDist[i],
  #     hitFraction=_hitFractionDif[i]))
  #   mergers[-1].SetInputProducer(prevProducer)
  #   mergers[-1].SetOutputProducer("ccMergedDiffuse" + str(i))
  #   prevProducer = "ccMergedDiffuse" + str(i)
  #   mergers[-1].SaveOutputCluster()   
  #   my_proc.add_process(mergers[-1])



  # merger = getStartTrackMerger()
  # merger.SetInputProducer(prevProducer)
  # merger.SetOutputProducer("ccMergedStartTrack")
  # prevProducer = "ccMergedStartTrack"
  # merger.SaveOutputCluster()
  # my_proc.add_process(merger)

  # ##################################
  # # Stage 5:
  # ##################################

#   prevProducer = "ccMergedStartTrack"

#   maxClosestDistances = [0.8, 1.0, 1.2, 0.8, ]
#   maxClusterSizes     = [40,  40,  40,  40,  ]
# # 1.0, 1.2, 1.4, 1.4
# # 15,  25,  30 , 40 
#   for i in range(0, len(maxClosestDistances)):
#     mergers.append(getMergeToTrunk(
#       shortestDist = maxClosestDistances[i],
#       maxClusterSize = maxClusterSizes[i],
#       prohibitBig=True))
#     mergers[-1].SetInputProducer(prevProducer)
#     mergers[-1].SetOutputProducer("ccMergedSDBig" + str(i))
#     mergers[-1].SaveOutputCluster()
#     prevProducer = "ccMergedSDBig" + str(i)
#     my_proc.add_process(mergers[-1])





  # # Add one more inline merger iteration:
  prevProducer = "ccMergedStartTrack"

  # mergers.append(getInlineMerger(
  #   maxInlineDist=0.8, 
  #   useAllHits=False,
  #   hitFraction=0.2))
  # mergers[-1].SetInputProducer(prevProducer)
  # mergers[-1].SetOutputProducer("ccMergedInlineFinal")
  # prevProducer = "ccMergedInlineFinal"
  # mergers[-1].SaveOutputCluster()
  # my_proc.add_process(mergers[-1])

  mergers.append(getExtendBlobMerger())
  mergers[-1].SetInputProducer(prevProducer)
  mergers[-1].SetOutputProducer("ccMergedExtendBlob")
  prevProducer = "ccMergedExtendBlob"
  mergers[-1].SaveOutputCluster()
  my_proc.add_process(mergers[-1])

  # Add a DropSingles module:
  drop = larlite.DropSingles()
  drop.SetInputProducer(prevProducer)
  drop.SetOutputProducer("ccMergedFinal")
  my_proc.add_process(drop)


  # my_proc.process_event(0)
  my_proc.run(0,10)

  # done!


if __name__ == '__main__':
  parser = argparse.ArgumentParser(description="Python script to process and merge showers.")
  group = parser.add_mutually_exclusive_group()
  group.add_argument("-v", "--verbose", help="Turn on verbose output",
                      action="store_true")
  group.add_argument("-q", "--quiet", help="Turn off most output",
                      action="store_true")
  parser.add_argument("-s","--source",nargs='*',help="Name of input file")
  parser.add_argument("-o","--data-output",help="Output data file, if event is changed")
  parser.add_argument("-a","--ana-output",help="Analysis output file")

  args = parser.parse_args()

  if len(sys.argv) == 1:
      print "\n-------You forgot to include a source file!-------\n"
      parser.print_help()

  main(**vars(args))
