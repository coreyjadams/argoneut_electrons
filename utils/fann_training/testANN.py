

import sys

# if len(sys.argv) < 2:
#     msg  = '\n'
#     msg += "Usage 1: %s $INPUT_TRAINING_FILE(s)\n" % sys.argv[0]
#     msg += '\n'
#     sys.stderr.write(msg)
#     sys.exit(1)

from larlite import larlite as fmwk
import numpy as np
from ROOT import *
import time

# Create Histograms for showing the data:
tracks_as_tracks = TH1F("tracks_as_tracks","Identification of Tracks",30,0,1)
tracks_as_showers = TH1F("tracks_as_showers","Identification of Tracks",30,0,1)
showers_as_tracks = TH1F("showers_as_tracks","Identification of Showers",30,0,1)
showers_as_showers = TH1F("showers_as_showers","Identification of Showers",30,0,1)

total_disc_tracks = TH1F("total_tracks","Net Discrimination",60,-1,1)
total_disc_showers = TH1F("total_showers","Net Discrimination",60,-1,1)

# Create ana_processor instance
# my_proc = fmwk.ana_processor()

# # Set input root file
# for x in xrange(len(sys.argv)-1):
#     my_proc.add_input_file(sys.argv[x+1])

# Specify IO mode
# my_proc.set_io_mode(fmwk.storage_manager.kREAD)

# Specify output root file name
# my_proc.set_ana_output_file("from_test_ana_you_can_remove_me.root");

# Attach an analysis unit ... here we use a base class which does nothing.
# Replace with your analysis unit if you wish.

# Configure the process:
proc = cluster.FANNModule()

proc.LoadFromFile("trackShowerAnn.dat")

with open("testingFannData.txt") as f:
  a = f.readlines()


# get the header information:
head = a[0].split()
n_items = int(head[0])
n_input = int(head[1])
n_output = int(head[2])

inputVec = vector(float)()
inputVec.resize(n_input)
outputVec = vector(float)()
outputVec.resize(n_output)

result = vector(float)()

n_iterations = 393

track_cut = -0.9
shower_cut = 0.9

n_total_showers = 0
n_total_tracks = 0

n_found_showers = 0
n_found_tracks = 0

n_misid_showers = 0
n_misid_tracks = 0


for iteration in range(1,n_iterations+1):

  # read in the data to the vector:
  inStr = a[iteration*2 - 1].split()
  outStr = a[iteration*2].split()


  for i in xrange(len(inStr)):
    inputVec[i] = float(inStr[i])


  for i in xrange(len(outStr)):
    outputVec[i] = float(outStr[i])

  proc.run(inputVec,result)

  # print result[0], result[1]
  # print outputVec[0], outputVec[1]

  if outputVec[0] == 0:
    # This is a track (truth level)
    tracks_as_tracks.Fill(result[0])
    tracks_as_showers.Fill(result[1])
    total_tracks.Fill(result[0] - result[1])
    n_total_tracks += 1
    if result[0] - result[1] < track_cut:
      n_found_tracks += 1

    if result[0] - result[1] > shower_cut:
      n_misid_showers += 1

  if outputVec[0] == 1:
    # This is a shower (truth level)
    showers_as_tracks.Fill(result[0])
    showers_as_showers.Fill(result[1])
    total_showers.Fill(result[0]-result[1])
    n_total_showers += 1
    if result[0] - result[1] > shower_cut:
      n_found_showers += 1
    if result[0] - result[1] < track_cut:
      n_misid_showers += 1

# Print out the efficiencies:
print "Total showers in sample: ", str(n_total_showers)
print "\t Found ", str(n_found_showers), " showers."
print "\t Mis Id ", str(n_misid_tracks), " tracks."
print "Total tracks in sample: ", str(n_total_tracks)
print "\t Found ", str(n_found_tracks), " tracks."
print "\t Mis Id ", str(n_misid_showers), " showers."



tracks = TCanvas("tracks","Tracks",800,600)
showers = TCanvas("showers","Showers",800,600)
total = TCanvas("total","Showers",800,600)


tracks.cd()
tracks_as_tracks.SetLineColor(kBlue)
tracks_as_tracks.Draw()
tracks_as_showers.SetLineColor(kRed)
tracks_as_showers.Draw("same")

showers.cd()
showers_as_showers.SetLineColor(kRed)
showers_as_showers.Draw()
showers_as_tracks.SetLineColor(kBlue)
showers_as_tracks.Draw("same")

total.cd()
total_tracks.SetLineColor(kBlue)
total_tracks.Draw()
total_showers.SetLineColor(kRed)
total_showers.Draw("same")


time.sleep(60)
# sys.exit(0)
