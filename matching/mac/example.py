import sys
from ROOT import gSystem
gSystem.Load("libArgoneut_matching")
from ROOT import sample

try:

    print "PyROOT recognized your class %s" % str(sample)

except NameError:

    print "Failed importing matching..."

sys.exit(0)

