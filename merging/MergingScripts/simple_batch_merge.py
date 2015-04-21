#!/usr/bin/env python

# Load libraries
import ROOT, sys, os
from ROOT import *
from clusterCrawlerShowerArgo import *
import argparse


def main(**args):
  

  my_proc = larlite.ana_processor()


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

  ##################################
  # Stage 1:
  ##################################

  # Module to turn all the single hits into one hit clusters
  htc = larlite.HitToCluster()
  htc.SetInputProducer(prevProducer)
  htc.SetOutputProducer("ccclusterWithSingles")
  my_proc.add_process(htc)

  # Trying an iterative merging approach:

  maxClosestDistances = [0.5,   0.8,  1.0,  1.2 ]
  maxAverageDistances = [9999,  9999, 9999, 9999]
  maxHitsSmall        = [1,     1,    3,    3   ]
  maxHitsProhibit     = [1,     1,    3,    3   ]
  minHitsInCluster    = [1,     1,    1,    3   ]

  prevProducer="ccclusterWithSingles"

  for i in range(0, len(maxClosestDistances)):
    mergers.append(getSmallClustMerger(
        maxHitsSmall  = maxHitsSmall[i],
        maxHitsProhib = maxHitsProhibit[i],
        maxDist       = maxClosestDistances[i],
        maxDistAv     = maxAverageDistances[i],
        minHits       = minHitsInCluster[i]))
    mergers[-1].SetInputProducer(prevProducer)
    mergers[-1].SetOutputProducer("ccMerged" + str(i))
    mergers[-1].SaveOutputCluster()
    prevProducer = "ccMerged" + str(i)
    my_proc.add_process(mergers[-1])

  # Add the inline merger:
  inlineMerger = getInlineMerger()
  inlineMerger.SetInputProducer(prevProducer)
  inlineMerger.SetOutputProducer("ccMergedInline")
  prevProducer = "ccMergedInline"
  inlineMerger.SaveOutputCluster()
  my_proc.add_process(inlineMerger)

  ##################################
  # Stage 3:
  ##################################


  # # Add a DropSingles module:
  # drop = larlite.DropSingles()
  # drop.SetInputProducer(prevProducer)
  # drop.SetOutputProducer("ccMergedNoSingles")
  # my_proc.add_process(drop)


  # Peter, add your algorithm here!

  maxClosestDistances = [0.5, 1.0, 1.5, 2.0, 2.5]
  maxSmallHits = [8.0, 20.0, 40.0, 60.0, 60.0]
  maxSmallLength = [10, 30, 50, 70, 100]

  stt_mergers = []

  for i in range(0,len(maxClosestDistances)):
    stt_mergers.append(getSmallToTrackMerger(
      maxClosestDistances[i],
      maxSmallHits[i],
      maxSmallLength[i]
      ))
    stt_mergers[-1].SetInputProducer(prevProducer)
    stt_mergers[-1].SetOutputProducer("ccMergedStT"+str(i))
    stt_mergers[-1].SaveOutputCluster()
    prevProducer = "ccMergedStT"+str(i)
    my_proc.add_process(stt_mergers[-1])


  ##################################
  # Stage 2:
  ##################################

  overlapFraction    = [0.9, 0.8, 0.7, 0.5 ]
  minHitsForConsid   = [5,   10,  15,  15  ]
  maxHitsProhibit    = [15,  30,  50,  50 ]

  for i in range(0, len(overlapFraction)):
    mergers.append(getOverlapMerger(
        overlapFrac = overlapFraction[i],
        minHits     = minHitsForConsid[i],
        maxHits     = maxHitsProhibit[i]))
    mergers[-1].SetInputProducer(prevProducer)
    mergers[-1].SetOutputProducer("ccMergedPoly" + str(i))
    mergers[-1].SaveOutputCluster()
    prevProducer = "ccMergedPoly" + str(i)
    my_proc.add_process(mergers[-1])

  withinMerger = getWithinMerger()
  withinMerger.SetInputProducer(prevProducer)
  withinMerger.SetOutputProducer("ccMergedWithinBoundary")
  withinMerger.SaveOutputCluster()
  my_proc.add_process(withinMerger)

  my_proc.run()
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
