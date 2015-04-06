#ifndef RECOTOOL_CBALGOMERGESMALLTOTRACK_CXX
#define RECOTOOL_CBALGOMERGESMALLTOTRACK_CXX

#include "CBAlgoMergeSmallToTrack.h"

namespace cmtool {

  CBAlgoMergeSmallToTrack::CBAlgoMergeSmallToTrack() : CBoolAlgoBase()
  {
    // Nothing to be done in the base class
    SetDebug(false);

    SetMinHits(20);
    SetMinModHitDens(1.4);
    SetMinMHitWires(3.5);
    SetMinCharge(100);
    //"fPrincipal" is log(1-eigenvalue_principal)
    // >-7 means EP > 0.99908
    SetMinPrincipal(-6.);

  }


  
  bool CBAlgoMergeSmallToTrack::Bool(
      const ::cluster::ClusterParamsAlg &cluster1,
      const ::cluster::ClusterParamsAlg &cluster2)
  {

    // Figure out which cluster is the track and which is the small
    // If both are track or both are small, return false

    return true;

    const ::cluster::ClusterParamsAlg * track;
    const ::cluster::ClusterParamsAlg * small;

    if (isTrack(cluster1)){
      if (isTrack(cluster2)){
        // both are tracks, bail
        return false;
      }
      track = &cluster1;
    }
    else if (isTrack(cluster2)){
      track =  &cluster2;
    }
    // else neither are tracks, bail
    else return false;

    // Figure out which is small
    if (isSmall(cluster1)){
      if (isSmall(cluster2)){
        // both are small, bail
        return false;
      }
      // verify that track and small aren't the same
      if (track == &cluster1) return false;
      small = &cluster1;
    }
    else if (isSmall(cluster2)){
      if (track == &cluster2) return false;
      small =  &cluster2;
    }
    // else neither are small, bail
    else return false;

    // Now make the final decision based on distances:
    // Must have the closest approach be less than the max
    // Must have the distance to start and end be greater than the min

    if (closestApproach(*track, *small) < _max_closest_dist){
      // In this space, fill in the computation 
      // to get the distance to the start and end points
      return true;
    }

    return false;
  }

  bool CBAlgoMergeSmallToTrack::isTrack(
    const ::cluster::ClusterParamsAlg &cluster)
  {
    return false;
  }
  bool CBAlgoMergeSmallToTrack::isSmall(
    const ::cluster::ClusterParamsAlg &cluster)
  {
    return false;
  }

  float CBAlgoMergeSmallToTrack::closestApproach(
    const ::cluster::ClusterParamsAlg &cluster1,
    const ::cluster::ClusterParamsAlg &cluster2)
  {
    return 99999.9;
  }
}

#endif
