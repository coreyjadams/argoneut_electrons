#ifndef RECOTOOL_CBALGOMERGESINGLETOBIG_CXX
#define RECOTOOL_CBALGOMERGESINGLETOBIG_CXX

#include "CBAlgoMergeSingleToBig.h"

namespace argomerge {

  CBAlgoMergeSingleToBig::CBAlgoMergeSingleToBig() : CBoolAlgoBase()
  {
    _max_distance = 0.4;
    _max_average_distance = 99999;
    _max_small_clust_hits = 1;
  }


  
  bool CBAlgoMergeSingleToBig::Bool(
      const ::cluster::cluster_params &cluster1,
      const ::cluster::cluster_params &cluster2)
  {


    if (cluster1.hit_vector.size() <= _max_small_clust_hits){
      if (isContainedBy(cluster1, cluster2))
        return true;
    }
    else if (cluster2.hit_vector.size() <= _max_small_clust_hits ){
      if (isContainedBy(cluster2, cluster1))
        return true;      
    }


    return false;
  }

  bool CBAlgoMergeSingleToBig::isContainedBy(
      const ::cluster::cluster_params &cluster1,
      const ::cluster::cluster_params &cluster2)
  {


    float minDist(9999.0);
    float averageDist(0.0);
    for (auto & hit : cluster2. hit_vector){
      for (auto & otherHit : cluster1.hit_vector){
        float thisDist = sqrt(pow(hit.w - otherHit.w,2 ) + pow(hit.t - otherHit.t, 2));
        if ( thisDist < minDist){
          minDist = thisDist;
        }
        averageDist += thisDist;
      }
    }
    averageDist = averageDist / (cluster2.hit_vector.size() * cluster1.hit_vector.size());
    if (minDist < _max_distance && averageDist < _max_average_distance){
      return true;
    }

    return false;


  }


}

#endif
