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

  if args['num_events'] != None:
    nevents = int(args['num_events'])

  my_proc.set_io_mode(larlite.storage_manager.kBOTH)

  for x in args['source']:
      my_proc.add_input_file(x)

  my_proc.set_output_file(args['data_output'])
  larutil.LArUtilManager.Reconfigure(larlite.geo.kArgoNeuT)

  mergers = []
  mergerNames = []
  prevProducer = "cccluster"

  # Module to take all of the poorly done, vertical tracks and destroy them
  drop = larlite.DropBadVertClusters()
  drop.SetInputProducer(prevProducer)
  drop.SetOutputProducer("ccNoVert")
  prevProducer = "ccNoVert"
  mergerNames.append(prevProducer)
  my_proc.add_process(drop)

  # Module to turn all the single hits into one hit clusters
  htc = larlite.HitToCluster()
  htc.SetInputProducer(prevProducer)
  htc.SetOutputProducer("ccclusterWithSingles")
  prevProducer="ccclusterWithSingles"
  mergerNames.append(prevProducer)
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
    mergerNames.append(prevProducer)
    my_proc.add_process(mergers[-1])

  mergers.append(getExtendBlobMerger(True))
  mergers[-1].SetInputProducer(prevProducer)
  mergers[-1].SetOutputProducer("ccMergedExtendBlobNoBig")
  prevProducer = "ccMergedExtendBlobNoBig"
  mergers[-1].SaveOutputCluster()
  mergerNames.append(prevProducer)
  my_proc.add_process(mergers[-1])

  # Add the inline merger:
  inlineMerger = getInlineMerger(maxInlineDist=0.5,useAllHits=False,hitFraction=0.25)
  inlineMerger.SetInputProducer(prevProducer)
  inlineMerger.SetOutputProducer("ccMergedInline")
  prevProducer = "ccMergedInline"
  mergerNames.append(prevProducer)
  inlineMerger.SaveOutputCluster()
  my_proc.add_process(inlineMerger)


  maxClosestDistances = [0.6, 0.65, 0.7,]
  maxClusterSizes     = [20,  20,   20, ]

  for i in range(0, len(maxClosestDistances)):
    mergers.append(getMergeToTrunk(
      shortestDist = maxClosestDistances[i],
      maxClusterSize = maxClusterSizes[i],
      prohibitBig=True))
    mergers[-1].SetInputProducer(prevProducer)
    mergers[-1].SetOutputProducer("ccMergedSDnoBig" + str(i))
    mergers[-1].SaveOutputCluster()
    prevProducer = "ccMergedSDnoBig" + str(i)
    mergerNames.append(prevProducer)
    my_proc.add_process(mergers[-1])

  # mergers.append(getMergeToTrunk(shortestDist = 0.6,
  #                                maxClusterSize = 20, 
  #                                prohibitBig=True))
  # mergers[-1].SetInputProducer(prevProducer)
  # mergers[-1].SetOutputProducer("ccMergedSDnoBig")
  # mergers[-1].SaveOutputCluster()
  # prevProducer = "ccMergedSDnoBig"
  # mergerNames.append(prevProducer)
  # my_proc.add_process(mergers[-1])


  merger = getStartTrackMerger()
  merger.SetInputProducer(prevProducer)
  merger.SetOutputProducer("ccMergedStartTrack")
  prevProducer = "ccMergedStartTrack"
  mergerNames.append(prevProducer)
  merger.SaveOutputCluster()
  my_proc.add_process(merger)

  # new function called
  mergers.append(mergeIfClose())
  mergers[-1].SetInputProducer(prevProducer)
  mergers[-1].SetOutputProducer("close")
  prevProducer = "close"
  mergerNames.append(prevProducer)
  mergers[-1].SaveOutputCluster()
  my_proc.add_process(mergers[-1])

  mergers.append(getExtendBlobMerger(False, 50,1))
  mergers[-1].SetInputProducer(prevProducer)
  mergers[-1].SetOutputProducer("ccMergedExtendBlob")
  prevProducer = "ccMergedExtendBlob"
  mergerNames.append(prevProducer)
  mergers[-1].SaveOutputCluster()
  my_proc.add_process(mergers[-1])

  mergers.append(getExtendBlobMerger(False,999,1))
  mergers[-1].SetInputProducer(prevProducer)
  mergers[-1].SetOutputProducer("ccMergedExtendBlob2")
  prevProducer = "ccMergedExtendBlob2"
  mergerNames.append(prevProducer)
  mergers[-1].SaveOutputCluster()
  my_proc.add_process(mergers[-1])


  # Add a DropSingles module:
  drop = larlite.DropSingles()
  drop.SetInputProducer(prevProducer)
  mergerNames.append("ccMergedFinal")
  drop.SetOutputProducer("ccMergedFinal")
  my_proc.add_process(drop)


  # my_proc.process_event(0)\
  if args['num_events'] != None:
    start = time.clock()
    my_proc.run(0,nevents)
    end = time.clock()
    print "Processed ", nevents, " events in ", end-start, "seconds."
    print "Average per event: ", (end-start)/nevents, "seconds."
  else:
    my_proc.run()




  print "mergers are: ", mergerNames




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
  parser.add_argument("-n","--num-events",help="Number of events to run over")

  args = parser.parse_args()

  if len(sys.argv) == 1:
      print "\n-------You forgot to include a source file!-------\n"
      parser.print_help()

  main(**vars(args))
