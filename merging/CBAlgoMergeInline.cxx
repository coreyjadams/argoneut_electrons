#ifndef RECOTOOL_CBALGOMERGEINLINE_CXX
#define RECOTOOL_CBALGOMERGEINLINE_CXX

#include "CBAlgoMergeInline.h"

namespace cmtool {

  CBAlgoMergeInline::CBAlgoMergeInline() : CBoolAlgoBase()
  {
    _max_average_min_distance = 0.0;
  }


  
  bool CBAlgoMergeInline::Bool(
      const ::cluster::ClusterParamsAlg &cluster1,
      const ::cluster::ClusterParamsAlg &cluster2)
  {


    if (cluster1.GetNHits() <= cluster2.GetNHits()){
      if (getAverageDist(cluster1, cluster2) < _max_average_min_distance)
        return true;
    }
    else {
      if (getAverageDist(cluster2, cluster1) < _max_average_min_distance)
        return true;      
    }


    return false;
  }

  float CBAlgoMergeInline::getAverageDist(
      const ::cluster::ClusterParamsAlg &cluster1,
      const ::cluster::ClusterParamsAlg &cluster2)
  {


    float averageDist(0.0);
    for (auto & hit : cluster1.GetHitVector()){
      float minDist(9999.0);
      for (auto & otherHit : cluster2.GetHitVector()){
        float thisDist = sqrt(pow(hit.w - otherHit.w,2 ) + pow(hit.t - otherHit.t, 2));
        if ( thisDist < minDist){
          minDist = thisDist;
        }
      }
      averageDist += minDist;
    }
    averageDist = averageDist / (cluster1.GetHitVector().size() );
    return averageDist;

  }


}

#endif
