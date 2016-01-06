

# This class defines the matching algorithms, their parameters, and returns
# a configuration to use.  It replaces the old files "run_DefaultShowerReco.py" 
# and "run_matcher.py"
#

# Import the libs needed:
from cpp_classes import argomatch
from ROOT import cmtool


def ArgoMatch():

    palgo_array = cmtool.CPAlgoArray()
    
    palgo1 = cmtool.CPAlgoNHits()
    palgo1.SetMinHits(45)
        
    palgo_array.AddAlgo(palgo1)

    algo_array = cmtool.CFAlgoArray()
    wireAlg    = argomatch.CFAlgoShowerWireMatch()
    timeAlg    = cmtool.CFAlgoTimeOverlap()
    timeAlg.RequireThreePlanes(False)
    algo_array.AddAlgo(wireAlg)
    algo_array.AddAlgo(timeAlg)

    return palgo_array, algo_array