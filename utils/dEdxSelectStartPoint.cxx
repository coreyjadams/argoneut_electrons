#ifndef DEDXSELECTSTARTPOINT_CXX
#define DEDXSELECTSTARTPOINT_CXX

#include "dEdxSelectStartPoint.h"
// #include "CRUException.h"
#include "LArUtil/GeometryHelper.h"
#include <map>

namespace argoutils {

void dEdxSelectStartPoint::do_params_fill(::cluster::cluster_params & cluster) {

  // Look at all the start point candidates and pick the one that is best
  // A start point candidate already is close to the min avgTan angle
  // and has passed a requirement that it's interior angle is less than pi/2
  //
  // Next, fit a chi2 of the hits against the start dir and pick the one with the min
  // chi2.  If values are close (within x2), pick the point that is closest to the mean
  // of the cluster.
  //
  // If there is only one candidate, then just set the value in cluster params and leave


  auto const& hits = cluster.hit_vector;
  auto & starts = cluster.start_point_cand;
  auto & shwrs = cluster.shwr_point_cand;
  auto & dirs = cluster.start_dir_cand;

  if (_verbose) {
    std::cout << "Candidate start points are: \n";
    for (size_t i = 0; i < starts.size(); i++) {
      std::cout << "\t(" << starts.at(i).w << ", " << starts.at(i).t << ")";
      std::cout << ", dir: (" << dirs.at(i).w << ", " << dirs.at(i).t << ")\n";
    }
    std::cout << std::endl;
  }


  if (starts.size() != shwrs.size()) {
    return;
  }
  if (starts.size() != dirs.size()) {
    return;
  }

  if (starts.size() == 0) {
    // cluster.start_point;
    return;
  }




  size_t leftMostIndex;
  float leftMostWire(1000);
  for (size_t i = 0; i < starts.size(); i++) {
    if (starts.at(i).w < leftMostWire) {
      leftMostIndex = i;
      leftMostWire = starts.at(i).w;
    }
  }

  if (starts.size() == 1) {
    // cluster.start_point;
    return;
  }

  // Pick the leftmost start point candidate:
  cluster.start_point = starts.at(leftMostIndex);
  cluster.showering_point = shwrs.at(leftMostIndex);


  return;
}

} // cluster

#endif
