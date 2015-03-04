#!/usr/bin/python

import sys
import numpy as np
import ctypes
import matplotlib.pyplot as plt

if len(sys.argv) < 2:
    msg  = '\n'
    msg += "Usage 1: %s $INPUT_ROOT_FILE(s)\n" % sys.argv[0]
    msg += '\n'
    sys.stderr.write(msg)
    sys.exit(1)




from larlite import larlite as fmwk

# Create ana_processor instance
my_proc = fmwk.ana_processor()


# Set input root file
for x in xrange(len(sys.argv)-1):
    my_proc.add_input_file(sys.argv[x+1])

# Specify IO mode
my_proc.set_io_mode(fmwk.storage_manager.kREAD)

# Specify output root file name
my_proc.set_ana_output_file("from_test_ana_you_can_remove_me.root");

# Attach an analysis unit ... here we use a base class which does nothing.
# Replace with your analysis unit if you wish.
my_proc.add_process(fmwk.DrawRaw())

print
print  "Finished configuring ana_processor. Start event loop!"
print

# Let's run it.
while my_proc.process_event():
# my_proc.run();    
    # wire = my_proc.get_process(0).getData();
    # print (wire.size())
    # print (wire.at(0).at(0))
    # print (wire.at(0).at(1))
    # print type(wire)
    # print type(wire[0])
    # print type(wire[0][0])
    d = np.array(my_proc.get_process(0).getData())
    print type(d)
    print d.shape
    print d
    
    plt.imshow(d.T)
    plt.show()

    sys.stdin.readline()



# done!
print
print "Finished running ana_processor event loop!"
print

sys.exit(0)
