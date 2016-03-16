import sys

if len(sys.argv) < 2:
    msg  = '\n'
    msg += "Usage 1: %s $INPUT_ROOT_FILE(s)\n" % sys.argv[0]
    msg += '\n'
    sys.stderr.write(msg)
    sys.exit(1)

from larlite import larlite as fmwk
from ROOT import argoutils

# Create ana_processor instance
my_proc = fmwk.ana_processor()
my_proc.enable_event_alignment(False)

# Set input root file
for x in xrange(len(sys.argv)-1):
    my_proc.add_input_file(sys.argv[x+1])

# Specify IO mode
my_proc.set_io_mode(fmwk.storage_manager.kBOTH)
my_proc.enable_filter(True)

# Specify output root file name
# my_proc.set_ana_output_file("from_test_ana_you_can_remove_me.root");
my_proc.set_output_file("anu_electrons_bootleg.root");
# my_proc.set_output_file("jasaadi_photons_antineutrino_bootleg.root");

# Attach an analysis unit ... here we use a base class which does nothing.
# Replace with your analysis unit if you wish.

bgcg = argoutils.BGCGenerator()
# bgcg.setBGCFile("/home/cadams/larlite/UserDev/argoneut_electrons/ScanFilejasaadi_neutrino.root")
# bgcg.setBGCFile("/home/cadams/larlite/UserDev/argoneut_electrons/ScanFilejasaadi_antineutrino.root")
bgcg.setBGCFile("/home/cadams/larlite/UserDev/argoneut_electrons/ScanFileandrzejs.root")

my_proc.add_process(bgcg)

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
