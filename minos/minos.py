import ROOT
import larlite
from ROOT import larlite, larutil
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
    