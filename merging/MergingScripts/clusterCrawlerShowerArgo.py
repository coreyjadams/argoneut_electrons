# Load libraries
import ROOT, sys, os
from ROOT import *


# Define all of the steps to merging for ClusterCrawlerShower
# each function returns an instance of ClusterMerger()

def getOneHitMerger():
  'This merger takes single hits and merges them into nearby clusters'
  merger = larlite.ClusterMerger()
  ########################################
  # PROHIBIT ALGORITHMS
  ########################################
  prohib_array = cmtool.CBAlgoArray()
  big_prohibit = cmtool.CBAlgoProhibitBigToBig()
  big_prohibit.SetMaxHits(1)
  prohib_array.AddAlgo(big_prohibit,False)
  
  ########################################
  # MERGE ALGORITHMS
  ########################################
  algo_array = cmtool.CBAlgoArray()
  singleToBig = cmtool.CBAlgoMergeSingleToBig()
  singleToBig.SetMaxDistance(0.45)
  algo_array.AddAlgo(singleToBig)
  
  merger.GetManager().AddMergeAlgo(algo_array)
  merger.GetManager().AddSeparateAlgo(prohib_array)
  return merger

def getMedClustMerger():
  merger = larlite.ClusterMerger()
  ########################################
  # PROHIBIT ALGORITHMS
  ########################################
  prohib_array = cmtool.CBAlgoArray()
  big_prohibit = cmtool.CBAlgoProhibitBigToBig()
  big_prohibit.SetMaxHits(10)
  prohib_array.AddAlgo(big_prohibit,False)
  
  ########################################
  # MERGE ALGORITHMS
  ########################################
  algo_array = cmtool.CBAlgoArray()
  singleToBig = cmtool.CBAlgoMergeSingleToBig()
  singleToBig.SetMaxDistance(1.5)
  singleToBig.SetMaxAverageDistance(2.5)
  singleToBig.SetMaxSmallClustHits(5)

  algo_array.AddAlgo(singleToBig)

  
  merger.GetManager().AddMergeAlgo(algo_array)
  merger.GetManager().AddSeparateAlgo(prohib_array)
  merger.GetManager().MergeTillConverge(True)
  merger.GetManager().SetMinNHits(1)
  return merger

def getSmallToTrackMerger(dist):
  merger = larlite.ClusterMerger()
  ########################################
  # PROHIBIT ALGORITHMS
  ########################################
  prohib_array = cmtool.CBAlgoArray()
  big_prohibit = cmtool.CBAlgoProhibitBigToBig()
  big_prohibit.SetMaxHits(10)
  prohib_array.AddAlgo(big_prohibit,False)

  # tracksep_prohibit = cmtool.CBAlgoTrackSeparate()
  # tracksep_prohibit.SetDebug(False)
  # tracksep_prohibit.SetVerbose(False)
  # tracksep_prohibit.SetMinAngleDiff(5)
  # tracksep_prohibit.SetUseEP(False)
  # prohib_array.AddAlgo(tracksep_prohibit,False)


  ########################################
  # MERGE ALGORITHMS
  ########################################
  algo_array = cmtool.CBAlgoArray()
  smallToTrack = cmtool.CBAlgoMergeSmallToTrack()
  smallToTrack.SetDebug(False)
  # smallToTrack.SetMinHits()
  # smallToTrack.SetMinModHitDens(0.5)
  # smallToTrack.SetMinMHitWires()
  # smallToTrack.SetMinPrincipal(10)
  # smallToTrack.SetMinCharge()
  # smallToTrack.SetMinLength()
  # smallToTrack.SetMaxHit(8)
  # smallToTrack.SetMaxCharge()
  # smallToTrack.SetMaxLength()
  # smallToTrack.SetMaxWidth(5)
  smallToTrack.SetMaxClosestDist(dist)
  # smallToTrack.SetMinDistToStart()
  # smallToTrack.SetMinDistToEnd()
  algo_array.AddAlgo(smallToTrack)

  merger.GetManager().AddMergeAlgo(algo_array)
  merger.GetManager().AddSeparateAlgo(prohib_array)
  return merger