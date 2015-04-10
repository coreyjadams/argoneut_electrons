# Load libraries
import ROOT, sys, os
from ROOT import *
from clusterCrawlerShowerArgo import *
import argparse

parser = argparse.ArgumentParser(description="Python script to process and merge showers.")
group = parser.add_mutually_exclusive_group()
group.add_argument("-v", "--verbose", help="Turn on verbose output",
                    action="store_true")
group.add_argument("-q", "--quiet", help="Turn off most output",
                    action="store_true")
parser.add_argument("-s","--source",nargs='*',help="Name of input file")
parser.add_argument("-o","--data-output",help="Output data file, if event is changed")
parser.add_argument("-a","--ana-output",help="Analysis output file")
parser.add_argument("-n","--num-events",help="Number of events to process")
parser.add_argument("-d","--display",help="Turn on the display to see each view before and after." )
args = parser.parse_args()

my_proc = larlite.ana_processor()


if len(sys.argv) == 1:
    print "\n-------You forgot to include a source file!-------\n"
    parser.print_help()

if args.verbose:
    print "Verbose mode turned on."
    if args.source != None:
        print "\tSource file is " + args.source
    if args.data_output != None:
        print "\tData output file is " + args.data_output
    if args.ana_output != None:
        print "\tAna output file is " + args.ana_output
    my_proc.set_verbosity(larlite.msg.kDEBUG)

if args.source == None:
    print "Error: please specificy an input file with -s or --source."
    quit()

if args.num_events == None:
    args.num_events = -1

if args.data_output == None:
    args.data_output = "default_event_output.root"
    if args.verbose:
      print "No event output file selected.  If necessary, output will go to:"
      print "\t"+args.data_output

if args.ana_output == None:
    args.ana_output = "default_ana_output.root"
    if  args.verbose:
      print "No ana output file selected.  If necessary, output will go to:"
      print "\t"+args.ana_output



my_proc.set_io_mode(larlite.storage_manager.kBOTH)

for x in args.source:
    my_proc.add_input_file(x)

my_proc.set_output_file(args.data_output)
larutil.LArUtilManager.Reconfigure(larlite.geo.kArgoNeuT)

merger1 = getOneHitMerger()
merger1.SetInputProducer("ccclusterWithSingles")
merger1.SetOutputProducer("ccclusterMerged1")
merger1.SaveOutputCluster()

merger2 = getMedClustMerger()
merger2.SetInputProducer("ccclusterMerged1")
merger2.SetOutputProducer("ccclusterMerged2")
merger2.SaveOutputCluster()

my_proc.add_process(merger1)
my_proc.add_process(merger2)

my_proc.run()


# my_proc.process_event(0)

# done!
