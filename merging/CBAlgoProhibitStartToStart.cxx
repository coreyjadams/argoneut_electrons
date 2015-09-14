#ifndef RECOTOOL_CBALGOPROHIBITSTARTTOSTART_CXX
#define RECOTOOL_CBALGOPROHIBITSTARTTOSTART_CXX

#include "CBAlgoProhibitStartToStart.h"

namespace argomerge {

CBAlgoProhibitStartToStart::CBAlgoProhibitStartToStart() : CBoolAlgoBase()
{
  // SetDebug(false);

  // SetMinHits(20);
  // SetMinModHitDens(1.4);
  // SetMinMHitWires(3.5);
  // SetMinCharge(100);
  // //"fPrincipal" is log(1-eigenvalue_principal)
  // // >-7 means EP > 0.99908
  // SetMinPrincipal(-6.);

  _min_separation = 0;
  _min_hits = 10;
  debug = false;
}



bool CBAlgoProhibitStartToStart::Bool(
  const ::cluster::cluster_params &cluster1,
  const ::cluster::cluster_params &cluster2)
{

  // Don't bother looking for start and end points if too small:
  if (cluster1.N_Hits < _min_hits ||
      cluster2.N_Hits < _min_hits)
  {
    return false;
  }

  // Each cluster has a start point and an end point
  // use all 4 combinations and take the smallest distance
  // this is to account for any cluster being backwards

  float dist = getDistance(cluster1.start_point,
                           cluster2.start_point);

  float tempDist = getDistance(cluster1.start_point,
                               cluster2.end_point);
  if (dist > tempDist) dist = tempDist;

  tempDist = getDistance(cluster1.end_point,
                         cluster2.start_point);
  if (dist > tempDist) dist = tempDist;

  tempDist = getDistance(cluster1.end_point,
                         cluster2.end_point);
  if (dist > tempDist) dist = tempDist;

  // Debug printouts:
  if (debug && dist < _min_separation) {
    std::cout << "Cluster1 (Plane " << cluster1.plane_id.Plane << "): \n\tstart:"
              << cluster1.start_point.w << ", "
              << cluster1.start_point.t << "\tend:"
              << cluster1.end_point.w << ", "
              << cluster1.end_point.t << "\n"
              << "Cluster2 (Plane " << cluster2.plane_id.Plane << "): \n\tstart:"
              << cluster2.start_point.w << ", "
              << cluster2.start_point.t << "\tend:"
              << cluster2.end_point.w << ", "
              << cluster2.end_point.t << "\n";
    std::cout << "MinDist is " << dist << "\n\n";
  }

  if (dist < _min_separation) return true;

  else return false;

}

float CBAlgoProhibitStartToStart::getDistance(larutil::PxPoint p1,
    larutil::PxPoint p2)
{
  return sqrt( pow(p1.w - p2.w, 2)
               + pow(p1.t - p2.t, 2));
}

}

#endif
