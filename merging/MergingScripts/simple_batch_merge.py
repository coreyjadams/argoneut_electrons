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

  # Module to turn all the single hits into one hit clusters
  htc = larlite.HitToCluster()
  htc.SetInputProducer("cccluster")
  htc.SetOutputProducer("ccclusterWithSingles")
  my_proc.add_process(htc)

  # Trying an iterative merging approach:

  maxClosestDistances = [0.5,   0.65, 1.0, 1.5]
  maxAverageDistances = [100,   1.5,  1.5, 2.5]
  maxHitsSmall        = [1,     3,    5,   10 ]
  maxHitsProhibit     = [1,     3,    5,   10 ]
  minHitsInCluster    = [1,     1,    2,   5  ]

  prevProducer="ccclusterWithSingles"

  mergers = []

  for i in range(0, 4):
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

  # Add a DropSingles module:
  # drop = larlite.DropSingles()
  # drop.SetInputProducer(prevProducer)
  # drop.SetOutputProducer("ccMergedNoSingles")
  # my_proc.add_process(drop)


  # Peter, add your algorithm here!
  merger3 = getSmallToTrackMerger(0.5)
  merger3.SetInputProducer(prevProducer)
  merger3.SetOutputProducer("ccMergedStage21")
  merger3.SaveOutputCluster()
  my_proc.add_process(merger3)

  # # Second iteration
  # merger4 = getSmallToTrackMerger(1.0)
  # merger4.SetInputProducer("ccMergedStage21")
  # merger4.SetOutputProducer("ccMergedStage22")
  # merger4.SaveOutputCluster()
  # my_proc.add_process(merger4)

  # #Third iteration
  # merger5 = getSmallToTrackMerger(1.5)
  # merger5.SetInputProducer("ccMergedStage22")
  # merger5.SetOutputProducer("ccMergedStage23")
  # merger5.SaveOutputCluster()
  # my_proc.add_process(merger5)

  # #Fourth iteration
  # merger6 = getSmallToTrackMerger(2.5)
  # merger6.SetInputProducer("ccMergedStage23")
  # merger6.SetOutputProducer("ccMergedStage24")
  # merger6.SaveOutputCluster()
  # my_proc.add_process(merger6)

  # my_proc.process_event(0)
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
