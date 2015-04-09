#ifndef RECOTOOL_CBALGOMERGESINGLETOBIG_CXX
#define RECOTOOL_CBALGOMERGESINGLETOBIG_CXX

#include "CBAlgoMergeSingleToBig.h"

namespace cmtool {

  CBAlgoMergeSingleToBig::CBAlgoMergeSingleToBig() : CBoolAlgoBase()
  {
    // SetDebug(false);

    // SetMinHits(20);
    // SetMinModHitDens(1.4);
    // SetMinMHitWires(3.5);
    // SetMinCharge(100);
    // //"fPrincipal" is log(1-eigenvalue_principal)
    // // >-7 means EP > 0.99908
    // SetMinPrincipal(-6.);

    _max_distance = 0.5;
  }


  
  bool CBAlgoMergeSingleToBig::Bool(
      const ::cluster::ClusterParamsAlg &cluster1,
      const ::cluster::ClusterParamsAlg &cluster2)
  {

    // Determine if this combination is exactly one cluster with 
    // multiple hits and one cluster with a single hit:
    if (cluster1.GetNHits() != 1 && cluster2.GetNHits() != 1){
      return false;
    }
    if (cluster1.GetNHits() == 1 && cluster2.GetNHits() == 1){
      return false;
    }
    // Now left with one cluster that's one hit, and one that isn't.

    if (cluster1.GetNHits() == 1){
      if (isContainedBy(cluster1, cluster2))
        return true;
    }
    else{
      if (isContainedBy(cluster2, cluster1))
        return true;      
    }


    return false;
  }

  bool CBAlgoMergeSingleToBig::isContainedBy(
      const ::cluster::ClusterParamsAlg &cluster1,
      const ::cluster::ClusterParamsAlg &cluster2)
  {


    larutil::PxHit single = cluster1.GetHitVector().front();
    larutil::PxHit closestHit = cluster2.GetHitVector().front();
    float minDist(9999.0);

    for (auto & hit : cluster2. GetHitVector()){
      float thisDist = pow(hit.w - single.w,2 ) + pow(hit.t - single.t, 2);
      if ( thisDist < minDist){
        closestHit = hit;
        minDist = thisDist;
      }
    }
    if (minDist < _max_distance){
      // The first cluster is always the one with exactly 1 hit.
      // std::cout << "\nThe cooridinates of the one hit wonder are: "
                // << cluster1.GetHitVector().front().w << ", "
                // << cluster1.GetHitVector().front().t << ".\n";
      // std::cout << "Closest Hit is (dist = " << minDist << "): "
                  // << closestHit.w << ", " << closestHit.t << ".\n";
      return true;
    }

    return false;


  }


}

#endif
