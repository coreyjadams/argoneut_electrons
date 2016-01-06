import ROOT
import larlite
from colored_msg import error, warning
orig_level = ROOT.gErrorIgnoreLevel
ROOT.gErrorIgnoreLevel = ROOT.kFatal

libs = ['/home/cadams/larlite/lib/libargoneut_electrons_analysis.so',
        '/home/cadams/larlite/lib/libargoneut_electrons_filter.so',
        '/home/cadams/larlite/lib/libargoneut_electrons_matching.so',
        '/home/cadams/larlite/lib/libargoneut_electrons_merging.so',
        '/home/cadams/larlite/lib/libargoneut_electrons_minos.so',
        '/home/cadams/larlite/lib/libargoneut_electrons_vertex.so',
        '/home/cadams/larlite/lib/libargoneut_electrons_utils.so']


for l in libs:
    # continue
    val = ROOT.gSystem.Load(l)
    if val < 0:
        warning('Skipping %s' % l)
    # else:
    #    info('%s %s' % (val,l))
ROOT.gErrorIgnoreLevel = orig_level

try:
    ROOT
    ROOT.argoutils
    ROOT.argomerge
    ROOT.argomatch
    ROOT.argofilter
    ROOT.argovertex
except AttributeError:
    error('No Argoneut library loaded!')
    raise
from ROOT import argoutils, argomerge, argomatch, argofilter, argovertex

