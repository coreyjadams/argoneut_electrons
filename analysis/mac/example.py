import sys
from ROOT import gSystem
gSystem.Load("libargoneut_electrons_analysis")
from ROOT import sample

try:

    print "PyROOT recognized your class %s" % str(sample)

except NameError:

    print "Failed importing analysis..."

sys.exit(0)

