#ifndef RECOTOOL_CBALGOMERGESMALLTOTRACK_CXX
#define RECOTOOL_CBALGOMERGESMALLTOTRACK_CXX

#include "CBAlgoMergeSmallToTrack.h"

namespace cmtool {

  CBAlgoMergeSmallToTrack::CBAlgoMergeSmallToTrack() : CBoolAlgoBase()
  {
    // Nothing to be done in the base class
    SetDebug(false);

    // Set parameters for track-like clusters
    SetMinHits(20);
    SetMinModHitDens(1.4);
    SetMinMHitWires(3.5);
    SetMaxWidth(5);
    SetMinLength(10);
    
    // Set parameters for small clusters
    SetMaxCharge(100);
    SetMaxLength(20);
    SetMaxWidth(20);
    SetMaxHits(100);

    // Set merging criteria
    SetMaxClosestDist(50);

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

    unsigned int npoints1 = cluster1.GetParams().PolyObject.Size();
    unsigned int npoints2 = cluster2.GetParams().PolyObject.Size();

    //loop over points on first polygon
    for(unsigned int i = 0; i < npoints1; ++i){

      float pt1w = cluster1.GetParams().PolyObject.Point(i).first;
      float pt1t = cluster1.GetParams().PolyObject.Point(i).second;
      
      //loop over points on second polygon
      for(unsigned int j = 0; j < npoints2; ++j){

        float pt2w = cluster2.GetParams().PolyObject.Point(j).first;
        float pt2t = cluster2.GetParams().PolyObject.Point(j).second;
        double distsqrd = pow(pt2w-pt1w,2)+pow(pt2t-pt1t,2);
        
        if (distsqrd < _max_closest_dist) return true;
      }

    }
    // if (closestApproach(*track, *small) < _max_closest_dist){
    //   // In this space, fill in the computation 
    //   // to get the distance to the start and end points
    //   return true;
    // }

    return false;
  }

/*---------------------------------------------------------------------*/
// Determine is a cluster is a track based on some parameters.

  bool CBAlgoMergeSmallToTrack::isTrack(
    const ::cluster::ClusterParamsAlg &cluster)
  {

      size_t N_Hits = cluster.GetHitVector().size();
      auto start_point = cluster.GetParams().start_point;
      double angle_2d = cluster.GetParams().angle_2d;
      double opening_angle = cluster.GetParams().opening_angle;
      Polygon2D PolyObject = cluster.GetParams().PolyObject;
      double length = cluster.GetParams().length;
      double width = cluster.GetParams().width;

      if( (N_Hits > _min_hits) &&
          (width < _max_width) &&
          (length < _min_length) ){

          return true;
      }

    return false;
  }

/*---------------------------------------------------------------------*/
// Determine if a cluster is small enough

  bool CBAlgoMergeSmallToTrack::isSmall(
    const ::cluster::ClusterParamsAlg &cluster)
  {

      size_t N_Hits = cluster.GetHitVector().size();
      auto start_point = cluster.GetParams().start_point;
      double angle_2d = cluster.GetParams().angle_2d;
      double opening_angle = cluster.GetParams().opening_angle;
      Polygon2D PolyObject = cluster.GetParams().PolyObject;
      double length = cluster.GetParams().length;
      double width = cluster.GetParams().width;
      double charge = cluster.GetParams().sum_charge;

      if( (N_Hits < _max_hits) &&
          (charge < _max_charge) &&
          (length < _max_length) &&
          (width < _max_width)      ){

            return true;
      }

    return false;
  }

  float CBAlgoMergeSmallToTrack::closestApproach(
    const ::cluster::ClusterParamsAlg &cluster1,
    const ::cluster::ClusterParamsAlg &cluster2)
  {



  }
}

#endif
