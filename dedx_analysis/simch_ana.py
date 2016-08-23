import ROOT
import larlite
from ROOT import argoana, larutil, larlite


_file = "/data_linux/argoneut/argoneut_single_photon/larlite_photon_lowerE_simch_mergeall_endpoint2d_match_shower.root"

_process = larlite.truedEdxAna()

larutil.LArUtilManager.Reconfigure(larlite.geo.kArgoNeuT)

# Create ana_processor instance
my_proc = larlite.ana_processor()

# Set input root file
my_proc.add_input_file(_file)

# Specify IO mode
my_proc.set_io_mode(larlite.storage_manager.kREAD)

# Specify output root file name
# my_proc.set_ana_output_file()

# Attach an analysis unit ... here we use a base class which do
my_proc.add_process(_process)

# # Let's run it.
# if n_events == -1:
#     my_proc.run(0)
# else:
#     if n_skip == -1:
#         my_proc.run(0, n_events)
#     else:
#         my_proc.run(n_skip, n_events)

my_proc.run(0, 5)