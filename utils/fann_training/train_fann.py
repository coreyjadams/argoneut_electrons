

import sys

if len(sys.argv) < 2:
    msg  = '\n'
    msg += "Usage 1: %s $INPUT_TRAINING_FILE(s)\n" % sys.argv[0]
    msg += '\n'
    sys.stderr.write(msg)
    sys.exit(1)

from larlite import larlite as fmwk
from ROOT import *

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
proc = cluster.TrainingModule()
# proc.setUseCascade(True)
# proc.addLayer(8)
# proc.addLayer(8)

proc.addLayer(7)


proc.setFeatureVectorLength(7)
proc.setOutputVectorLength(2)
proc.setOutputFileName("trackShowerAnn.dat")

proc.init(sys.argv[-1])

proc.train()
proc.saveFANNToFile()


# my_proc.add_process(proc)


# print
# print  "Finished configuring ana_processor. Start event loop!"
# print

# Let's run it.
# my_proc.run(0);

# # done!
# print
# print "Finished running ana_processor event loop!"
# print

sys.exit(0)
