# Load libraries
import ROOT, sys, os
from ROOT import *


# Define all of the steps to merging for ClusterCrawlerShower
# each function returns an instance of ClusterMerger()

def getOneHitMerger(maxHits=1, maxDist=0.5):
  'This merger takes single hits and merges them into nearby clusters'
  merger = larlite.ClusterMerger()
  ########################################
  # PROHIBIT ALGORITHMS
  ########################################
  prohib_array = cmtool.CBAlgoArray()
  big_prohibit = cmtool.CBAlgoProhibitBigToBig()
  big_prohibit.SetMaxHits(maxHits)
  prohib_array.AddAlgo(big_prohibit,False)
  
  ########################################
  # MERGE ALGORITHMS
  ########################################
  algo_array = cmtool.CBAlgoArray()
  singleToBig = cmtool.CBAlgoMergeSingleToBig()
  singleToBig.SetMaxDistance(maxDist)
  algo_array.AddAlgo(singleToBig)
  
  merger.GetManager().AddMergeAlgo(algo_array)
  merger.GetManager().AddSeparateAlgo(prohib_array)
  return merger

def getSmallClustMerger(maxHitsProhib=5, maxHitsSmall=1, maxDist=0.5,maxDistAv=2.4, minHits = 1):
  merger = larlite.ClusterMerger()
  ########################################
  # PROHIBIT ALGORITHMS
  ########################################
  prohib_array = cmtool.CBAlgoArray()
  big_prohibit = cmtool.CBAlgoProhibitBigToBig()
  big_prohibit.SetMaxHits(maxHitsProhib)
  prohib_array.AddAlgo(big_prohibit,False)
  
  ########################################
  # MERGE ALGORITHMS
  ########################################
  algo_array = cmtool.CBAlgoArray()
  singleToBig = cmtool.CBAlgoMergeSingleToBig()
  singleToBig.SetMaxDistance(maxDist)
  singleToBig.SetMaxAverageDistance(maxDistAv)
  singleToBig.SetMaxSmallClustHits(maxHitsSmall)

  algo_array.AddAlgo(singleToBig)

  
  merger.GetManager().AddMergeAlgo(algo_array)
  merger.GetManager().AddSeparateAlgo(prohib_array)
  merger.GetManager().MergeTillConverge(False)
  merger.GetManager().SetMinNHits(minHits)
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
  singleToBig.SetMaxSmallClustHits(10)

  algo_array.AddAlgo(singleToBig)

  
  merger.GetManager().AddMergeAlgo(algo_array)
  merger.GetManager().AddSeparateAlgo(prohib_array)
  merger.GetManager().MergeTillConverge(False)
  merger.GetManager().SetMinNHits(1)
  return merger

def getBigClustMerger():
  merger = larlite.ClusterMerger()
  ########################################
  # PROHIBIT ALGORITHMS
  ########################################
  prohib_array = cmtool.CBAlgoArray()
  # big_prohibit = cmtool.CBAlgoProhibitBigToBig()
  # big_prohibit.SetMaxHits(1)
  # prohib_array.AddAlgo(big_prohibit,False)
  
  # tracksep_prohibit = cmtool.CBAlgoTrackSeparate()
  # tracksep_prohibit.SetDebug(False)
  # tracksep_prohibit.SetVerbose(False)
  # tracksep_prohibit.SetMinAngleDiff(1)
  # tracksep_prohibit.SetUseEP(False)
  # prohib_array.AddAlgo(tracksep_prohibit,True)

  ########################################
  # MERGE ALGORITHMS
  ########################################
  algo_array = cmtool.CBAlgoArray()
  # polySD = cmtool.CBAlgoPolyShortestDist()
  # polySD.SetMinDistSquared(0.5)
  # polySD.SetMinNumHits(10)
  # polySD.SetMaxNumHits(10)


  # algo_array.AddAlgo(polySD)

  
  merger.GetManager().AddMergeAlgo(algo_array)
  merger.GetManager().AddSeparateAlgo(prohib_array)
  merger.GetManager().MergeTillConverge(False)
  merger.GetManager().SetMinNHits(50)
  return merger


def getSmallToTrackMerger():
  merger = larlite.ClusterMerger()
  ########################################
  # PROHIBIT ALGORITHMS
  ########################################
  prohib_array = cmtool.CBAlgoArray()
  big_prohibit = cmtool.CBAlgoProhibitBigToBig()
  big_prohibit.SetMaxHits(15)
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
  # Setters for track-like parameters
  # smallToTrack.SetMinHits()
  smallToTrack.SetMinModHitDens(0.8)
  # smallToTrack.SetMinMHitWires()
  smallToTrack.SetMinPrincipal(10)
  # smallToTrack.SetMinCharge()
  # smallToTrack.SetMinLength()

  # Setter for small like parameters
  smallToTrack.SetMaxHit(8)
  # smallToTrack.SetMaxCharge()
  # smallToTrack.SetMaxLength()
  smallToTrack.SetMaxWidth(5)

  # Setter for merging parameters
  smallToTrack.SetMaxClosestDist(3)
  # smallToTrack.SetMinDistToStart()
  # smallToTrack.SetMinDistToEnd()
  algo_array.AddAlgo(smallToTrack)

  merger.GetManager().AddMergeAlgo(algo_array)
  merger.GetManager().AddSeparateAlgo(prohib_array)
  return merger