#ifndef RECOTOOL_CBALGOMERGEINLINE_CXX
#define RECOTOOL_CBALGOMERGEINLINE_CXX

#include "CBAlgoMergeInline.h"

namespace argomerge {

  CBAlgoMergeInline::CBAlgoMergeInline() : CBoolAlgoBase()
  {
    _max_average_min_distance = 0.0;
    _use_all_hits = true;
  }


  
  bool CBAlgoMergeInline::Bool(
      const ::cluster::cluster_params &cluster1,
      const ::cluster::cluster_params &cluster2)
  {

    if (_use_all_hits){
      if (cluster1.hit_vector.size() <= cluster2.hit_vector.size()){
        if (getAverageDist(cluster1, cluster2) < _max_average_min_distance)
          return true;
      }
      else {
        if (getAverageDist(cluster2, cluster1) < _max_average_min_distance)
          return true;      
      }
    }

    else{
      if (cluster1.hit_vector.size() <= cluster2.hit_vector.size()){
        if (getBinnedDist(cluster1, cluster2) > _hit_fraction)
          return true;
      }
      else {
        if (getBinnedDist(cluster2, cluster1) > _hit_fraction)
          return true;      
      }
    }

    return false;
  }

  float CBAlgoMergeInline::getAverageDist(
      const ::cluster::cluster_params &cluster1,
      const ::cluster::cluster_params &cluster2)
  {


    float averageDist(0.0);
    for (auto & hit : cluster1.hit_vector){
      float minDist(9999.0);
      for (auto & otherHit : cluster2.hit_vector){
        float thisDist = sqrt(pow(hit.w - otherHit.w,2 ) + pow(hit.t - otherHit.t, 2));
        if ( thisDist < minDist){
          minDist = thisDist;
        }
      }
      averageDist += minDist;
    }
    averageDist = averageDist / (cluster1.hit_vector.size() );
    return averageDist;

  }

  float CBAlgoMergeInline::getBinnedDist(
      const ::cluster::cluster_params &cluster1,
      const ::cluster::cluster_params &cluster2)
  {
    // This function finds the closest hit for each other hit, and bins
    // them into a quasi histogram to see what fraction are within the
    // required distance

    const float step = _max_average_min_distance/3.0;

    // float averageDist(0.0);
    std::vector<float> n_hits_by_dist;
    // Units of this are wire spacing in argoneut: 4mm.
    // Want to be able to bin out to a few centimeters.  Use 10 spots (4cm)
    n_hits_by_dist.resize(10);
    for (auto & hit : cluster1.hit_vector){
      float minDist(9999.0);
      for (auto & otherHit : cluster2.hit_vector){
        float thisDist = sqrt(pow(hit.w - otherHit.w,2 ) + pow(hit.t - otherHit.t, 2));
        if ( thisDist < minDist){
          minDist = thisDist;
        }
      }
      // Now have the min dist for this hit.  Put it in the histogram
      int index = minDist/step;
      if (index > 9) 
        n_hits_by_dist.back() ++;
      else n_hits_by_dist.at(index) ++;
      // averageDist += minDist;
    }
    // averageDist = averageDist / (cluster1.hit_vector.size() );

    // Find out how many hits are within the _max_average_min_distance
    int n_good_hits = 0;
    for(size_t i = 0; i < n_hits_by_dist.size(); i++){
      if (step*(i+1) <= _max_average_min_distance) n_good_hits += n_hits_by_dist[i];
    }

    // if (n_good_hits > 1){
    //   std::cout << n_good_hits << " of " << cluster1.hit_vector.size() << " were good.";
    //   std::cout << "(Plane " << cluster1.Plane() << ", start pos: "
    //             << cluster1.GetParams().start_point.w << ", " << cluster1.GetParams().start_point.t << ")\n";
    //   std::cout << "Printing final vector: \n";
    //   for(int i = 0; i < n_hits_by_dist.size(); i++){
    //     std::cout << "\t[" << wirePitch*i << " - " << wirePitch*(i+1) << "]:\t"
    //               << n_hits_by_dist[i] << "\n";
    //   }
    // }

    return (float) n_good_hits/cluster1.hit_vector.size();

  }


}

#endif
