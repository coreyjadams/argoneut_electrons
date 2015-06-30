#!/usr/bin/env python

# Load libraries
import ROOT, sys, os
from ROOT import *
from clusterCrawlerShowerArgo import *
import argparse
import time

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

  # #################################
  # Stage 1:
  # #################################

  # Module to take all of the poorly done, vertical tracks and destroy them
  htc = larlite.DropBadVertClusters()
  htc.SetInputProducer(prevProducer)
  htc.SetOutputProducer("ccNoVert")
  prevProducer = "ccNoVert"
  my_proc.add_process(htc)

  # Module to turn all the single hits into one hit clusters
  htc = larlite.HitToCluster()
  htc.SetInputProducer(prevProducer)
  htc.SetOutputProducer("ccclusterWithSingles")
  prevProducer="ccclusterWithSingles"
  my_proc.add_process(htc)


  # Trying an iterative merging approach:
  maxClosestDistances = [0.5,   0.8,  1.0,  0.8 ] # 
  maxAverageDistances = [9999,  9999, 9999, 9999] # 
  maxHitsSmall        = [1,     1,    3,    7   ] # 
  maxHitsProhibit     = [1,     1,    3,    7   ] # 
  minHitsInCluster    = [1,     1,    1,    3   ] # 


  for i in range(0, len(maxClosestDistances)):
    mergers.append(getSmallClustMerger(
        maxHitsSmall  = maxHitsSmall[i],
        maxHitsProhib = maxHitsProhibit[i],
        maxDist       = maxClosestDistances[i],
        maxDistAv     = maxAverageDistances[i],
        minHits       = minHitsInCluster[i]))
    mergers[-1].SetInputProducer(prevProducer)
    mergers[-1].SetOutputProducer("ccMergedSmall" + str(i))
    mergers[-1].SaveOutputCluster()
    prevProducer = "ccMergedSmall" + str(i)
    my_proc.add_process(mergers[-1])

  mergers.append(getExtendBlobMerger(True))
  mergers[-1].SetInputProducer(prevProducer)
  mergers[-1].SetOutputProducer("ccMergedExtendBlobNoBig")
  prevProducer = "ccMergedExtendBlobNoBig"
  mergers[-1].SaveOutputCluster()
  my_proc.add_process(mergers[-1])

  # Add the inline merger:
  inlineMerger = getInlineMerger(maxInlineDist=0.5)
  inlineMerger.SetInputProducer(prevProducer)
  inlineMerger.SetOutputProducer("ccMergedInline")
  prevProducer = "ccMergedInline"
  inlineMerger.SaveOutputCluster()
  my_proc.add_process(inlineMerger)


  maxClosestDistances = [0.6, 0.8, 1.2, 1.3,]
  maxClusterSizes     = [20,  20,  20,  20, ]

  for i in range(0, len(maxClosestDistances)):
    mergers.append(getMergeToTrunk(
      shortestDist = maxClosestDistances[i],
      maxClusterSize = maxClusterSizes[i],
      prohibitBig=True))
    mergers[-1].SetInputProducer(prevProducer)
    mergers[-1].SetOutputProducer("ccMergedSDnoBig" + str(i))
    mergers[-1].SaveOutputCluster()
    prevProducer = "ccMergedSDnoBig" + str(i)
    my_proc.add_process(mergers[-1])




  # # _maxInlineDist   =  [0.8, 1.0, ]
  # # _hitFraction     =  [.50, 0.4, ]

  # # # Add the inline and diffuse merger:
  # # for i in range(0, len(_maxInlineDist)):
  # #   mergers.append(getInlineMerger(
  # #     maxInlineDist=_maxInlineDist[i], 
  # #     useAllHits=False,
  # #     hitFraction=_hitFraction[i]))
  # #   mergers[-1].SetInputProducer(prevProducer)
  # #   mergers[-1].SetOutputProducer("ccMergedInline" + str(i))
  # #   prevProducer = "ccMergedInline" + str(i)
  # #   mergers[-1].SaveOutputCluster()
  # #   my_proc.add_process(mergers[-1])



  merger = getStartTrackMerger()
  merger.SetInputProducer(prevProducer)
  merger.SetOutputProducer("ccMergedStartTrack")
  prevProducer = "ccMergedStartTrack"
  merger.SaveOutputCluster()
  my_proc.add_process(merger)


  # # mergers.append(getInlineMerger(
  # #   maxInlineDist=1.0, 
  # #   useAllHits=False,
  # #   hitFraction=0.25,
  # #   bignessProhibit=999,
  # #   minHits=10))
  # # mergers[-1].SetInputProducer(prevProducer)
  # # mergers[-1].SetOutputProducer("ccMergedInlineFinal")
  # # prevProducer = "ccMergedInlineFinal"
  # # mergers[-1].SaveOutputCluster()
  # # my_proc.add_process(mergers[-1])


  # # # Add one more inline merger iteration:
  # # prevProducer = "ccMergedStartTrack"

  # new function called
  # mergers.append(mergeIfClose())
  # mergers[-1].SetInputProducer(prevProducer)
  # mergers[-1].SetOutputProducer("close")
  # prevProducer = "close"
  # mergers[-1].SaveOutputCluster()
  # my_proc.add_process(mergers[-1])


  mergers.append(getExtendBlobMerger(False, 50,1))
  mergers[-1].SetInputProducer(prevProducer)
  mergers[-1].SetOutputProducer("ccMergedExtendBlob")
  prevProducer = "ccMergedExtendBlob"
  mergers[-1].SaveOutputCluster()
  my_proc.add_process(mergers[-1])

  mergers.append(getExtendBlobMerger(False,100,1))
  mergers[-1].SetInputProducer(prevProducer)
  mergers[-1].SetOutputProducer("ccMergedExtendBlob2")
  prevProducer = "ccMergedExtendBlob2"
  mergers[-1].SaveOutputCluster()
  my_proc.add_process(mergers[-1])




  # Add a DropSingles module:
  drop = larlite.DropSingles()
  drop.SetInputProducer(prevProducer)
  drop.SetOutputProducer("ccMergedFinal")
  my_proc.add_process(drop)

  start = time.clock()

  # my_proc.process_event(0)
  nevents = 200
  my_proc.run(0,nevents)

  end = time.clock()

  print "Processed ", nevents, " events in ", end-start, "seconds."
  print "Average per event: ", (end-start)/nevents, "seconds."
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
