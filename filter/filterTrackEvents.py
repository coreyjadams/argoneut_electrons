import ROOT
import larlite
from ROOT import larlite, larutil, argofilter
import sys

if __name__ == '__main__':

    if len(sys.argv) < 2:
        msg = '\n'
        msg += "Usage 1: %s $INPUT_ROOT_FILE(s)\n" % sys.argv[0]
        msg += '\n'
        sys.stderr.write(msg)
        sys.exit(1)

    f = sys.argv[-1]

    
    # Run minos matching on the file, reject events that are matched to minos:
    

    larutil.LArUtilManager.Reconfigure(larlite.geo.kArgoNeuT)
    
    # Create ana_processor instance
    my_proc = larlite.ana_processor()

    # Set input root file
    my_proc.add_input_file(f)

    # Specify IO mode
    my_proc.set_io_mode(larlite.storage_manager.kREAD)

    # Specify output root file name
    # my_proc.set_ana_output_file()

    # Attach an analysis unit ... here we use a base class which do
    my_proc.add_process(argofilter.TrackFilter())

    my_proc.run(0)
