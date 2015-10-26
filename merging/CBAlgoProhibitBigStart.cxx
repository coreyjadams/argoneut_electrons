#ifndef RECOTOOL_CBALGOPROHIBITBIGSTART_CXX
#define RECOTOOL_CBALGOPROHIBITBIGSTART_CXX

#include "CBAlgoProhibitBigStart.h"

namespace argomerge {

CBAlgoProhibitBigStart::CBAlgoProhibitBigStart() : CBoolAlgoBase()
{
  // SetDebug(false);

  // SetMinHits(20);
  // SetMinModHitDens(1.4);
  // SetMinMHitWires(3.5);
  // SetMinCharge(100);
  // //"fPrincipal" is log(1-eigenvalue_principal)
  // // >-7 means EP > 0.99908
  // SetMinPrincipal(-6.);

  _min_separation = .4;
  _min_hits = 25;
  debug = false;
}



bool CBAlgoProhibitBigStart::Bool(
  const ::cluster::cluster_params &cluster1,
  const ::cluster::cluster_params &cluster2)
{

  // This algorithm probihits merging if a cluster is too close to the start point of the bigger cluster
  // Only triggers if the big cluster is a certain size (other size doesn't matter)
  // Looks at every point of the other cluster.

  // if both clusters are below the minimum number of hits, don't do this;
  // if (cluster1.N_Hits < _min_hits &&
  //     cluster2.N_Hits < _min_hits){
  //   return false;
  // }

  // std::cout << "min sep is " << _min_separation << std::endl;

  if (cluster1.N_Hits == 27 && cluster2.N_Hits == 1){
    std::cout << "This one?\n";
  }

  // Find the bigger cluster:
  if (cluster1.N_Hits > cluster2.N_Hits) {
    if (cluster1.N_Hits < _min_hits) {
      return false;
    }
    auto start_point = cluster1.start_point;
    larutil::PxPoint alt_start_point = alt_start(cluster1.hit_vector);
    if (closestApproach(start_point, cluster2) < _min_separation) {
      std::cout << "Prohibiting around point " << start_point.w << ", " << start_point.t
                << "in plane " << cluster1.plane_id.Plane << " -- dist " << closestApproach(start_point, cluster2) << std::endl;
      return true;
    }
    if (closestApproach(alt_start_point, cluster2) < _min_separation) {
      std::cout << "Prohibiting around point " << start_point.w << ", " << start_point.t
                << "in plane " << cluster1.plane_id.Plane << " -- dist " << closestApproach(alt_start_point, cluster2) << std::endl;
      return true;
    }
  }
  else {
    if (cluster2.N_Hits < _min_hits) {
      return false;
    }
    auto start_point = cluster2.start_point;
    larutil::PxPoint alt_start_point = alt_start(cluster2.hit_vector);
    if (closestApproach(start_point, cluster1) < _min_separation) {
      std::cout << "Prohibiting around other point " << start_point.w << ", " << start_point.t
                << "in plane " << cluster1.plane_id.Plane << " -- dist " << closestApproach(start_point, cluster1) << std::endl;
      return true;
    }
    if (closestApproach(alt_start_point, cluster1) < _min_separation) {
      std::cout << "Prohibiting around point " << start_point.w << ", " << start_point.t
                << "in plane " << cluster1.plane_id.Plane << " -- dist " << closestApproach(alt_start_point, cluster2) << std::endl;
      return true;
    }
  }

  return false;

}

float CBAlgoProhibitBigStart::closestApproach(
  larutil::PxPoint start_point,
  const ::cluster::cluster_params &cluster2)
{


  float minDist(9999.0);
  for (auto & hit : cluster2.hit_vector) {
    float thisDist = sqrt(pow(hit.w - start_point.w, 2 ) + pow(hit.t - start_point.t, 2));
    if ( thisDist < minDist) {
      minDist = thisDist;
    }
  }

  return minDist;
}

larutil::PxPoint CBAlgoProhibitBigStart::alt_start(const std::vector<larutil::PxHit> & hitlist) {
  int leftmost_index = 0;
  for (int i = 0; i < hitlist.size(); ++i)
  {
    if (hitlist.at(i).w < hitlist.at(leftmost_index).w) leftmost_index = i;
  }
  larutil::PxPoint newHit( hitlist.at(leftmost_index).plane,
                           hitlist.at(leftmost_index).w,
                           hitlist.at(leftmost_index).t );
  return hitlist.at(leftmost_index);

}


}

#endif
