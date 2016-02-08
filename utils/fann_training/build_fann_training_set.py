

import sys

if len(sys.argv) < 2:
    msg  = '\n'
    msg += "Usage 1: %s $INPUT_ROOT_FILE(s)\n" % sys.argv[0]
    msg += '\n'
    sys.stderr.write(msg)
    sys.exit(1)

from larlite import larlite
from ROOT import larutil, argoutils

# Create ana_processor instance
my_proc = larlite.ana_processor()
larutil.LArUtilManager.Reconfigure(larlite.geo.kArgoNeuT)

# Set input root file
for x in xrange(len(sys.argv)-1):
    my_proc.add_input_file(sys.argv[x+1])

# Specify IO mode
my_proc.set_io_mode(larlite.storage_manager.kREAD)

# Specify output root file name
my_proc.set_ana_output_file("testingParameters.root");

# Attach an analysis unit ... here we use a base class which does nothing.
# Replace with your analysis unit if you wish.

# Configure the process:
proc = argoutils.GenFANNTrainingData()
proc.setOutputFile("testingFannData.txt")
proc.setClusterProducer("ccMergedFinal");

my_proc.add_process(proc)


print
print  "Finished configuring ana_processor. Start event loop!"
print

# Let's run it.
my_proc.run(0);

# done!
print
print "Finished running ana_processor event loop!"
print

sys.exit(0)
