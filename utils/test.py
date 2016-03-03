import ROOT
import larlite

from ROOT import argoutils

a = argoutils.BGCFileReader()

fileName="/home/cadams/larlite/UserDev/argoneut_electrons/ScanFileandrzejs.root"

a.setInputFile(fileName)
a.readData()

# Play around with the data:

b = a.getData()

totalEvents=0

for run in b:
  print run.first, ", ", run.second.size(), " events"
  for event in run.second:
    print "\tEvent ", event.first, ", ", event.second.size(), " clusters."
  totalEvents += run.second.size()

print totalEvents