#ifndef RECOTOOL_CBALGOMERGEEXTENDBLOB_CXX
#define RECOTOOL_CBALGOMERGEEXTENDBLOB_CXX

#include "CBAlgoMergeExtendBlob.h"

namespace argomerge {

CBAlgoMergeExtendBlob::CBAlgoMergeExtendBlob() : CBoolAlgoBase()
{
  _min_hits_to_project_from = 25;
  _principal_ev_cut = 0.9;
  _rms_scale = 2.0;
  _length_jump_scale = 0.5;
  _mode = 0;
  _debug = false;
}



bool CBAlgoMergeExtendBlob::Bool(const ::cluster::cluster_params &cluster1,
                                 const ::cluster::cluster_params &cluster2)
{
  if (cluster1.hit_vector.size() < _min_hits_to_project_from &&
      cluster2.hit_vector.size() < _min_hits_to_project_from ) {
    return false;
  }

  const ::cluster::cluster_params * bigCluster, * littleCluster;

  // Option of two modes:
  // Merge small into big
  // Merge downstream into upstream

  if (_mode == 0) {
    if (cluster1.hit_vector.size() > cluster2.hit_vector.size()) {
      bigCluster = &cluster1;
      littleCluster = &cluster2;
    }
    else {
      bigCluster = & cluster2;
      littleCluster = &cluster1;
    }
  }
  else {
    // compare which cluster is further upstream:
    if (cluster1.start_point.w < cluster2.start_point.w) {
      bigCluster = &cluster1;
      littleCluster = &cluster2;
    }
    else {
      bigCluster = &cluster2;
      littleCluster = &cluster1;
    }
  }

  if (bigCluster -> hit_vector.size() < _min_hits_to_project_from ) return false;

  // if (cluster1.hit_vector.size() >= cluster2.hit_vector.size()){

  // Here are the criteria:
  // First, don't suck in tracks.  If it's not a track or < 10 hits, fair game
  // Make sure that the big cluster is in front of the small cluster
  //   -For this, assuming big cluster's start point is correct.  Then
  //    direction is positive towards the center of cluster.
  //   -Then check that the second cluster is in the same direction
  // Find the distance between the line along the big cluster to the
  // center of the small cluster.  Compare this distance to the rms of the small cluster
  // Find the distance between the two clusters and make sure it's not more
  // that the length of the big cluster.  Distance is measure along principal axis of bigger clust

  // Check on trackness:
  if (! ts.isTrack(*littleCluster) || littleCluster->hit_vector.size() < 10 ) {
    // can continue
    // Only take events with at least a reasonably strong directionality:
    if (bigCluster ->eigenvalue_principal < _principal_ev_cut) {
      return false;
    }

    // Check that cluster1 is in front of cluster2
    if (!isInFrontOf(*bigCluster, *littleCluster)) {
      return false;
    }

    // For the closest approach part, use different criteria:
    // For small clusters, 4 hits or less, use rms of big cluster along axis compared to offset
    // For others, use rms of second cluster compared to offset

    float closest_approach = getClosestApproachTo(*bigCluster, *littleCluster);
    // if (littleCluster->hit_vector.size() <= 4){
    float rms = fmin(getRMSAlongAxis(*bigCluster), 2);
    // std::cout << "\trms is " << rms << std::endl;
    if (closest_approach > _rms_scale * rms) {
      // reject if offset is more than 2 rms distances away
      if (_debug) std::cout << "\trejecting by closest approach \n";
      return false;
    }
    // }
    // else{
    //   float rms_clust2 = sqrt(pow(littleCluster-.rms_x, 2)
    //                         + pow(littleCluster-.rms_y, 2));
    //   std::cout << "\trms is " << rms_clust2 << std::endl;
    //   if (closest_approach > 3*rms_clust2){
    //     // reject if offset is more than 2 rms distances away
    //     std::cout << "\trejecting by closest approach \n";
    //     return false;
    //   }
    // }

    // Now do the checking:




    float separation = getSeparation(*bigCluster, *littleCluster);
    float clust1_length = getLength(*bigCluster);
    // check this cut:
    // If the second cluster is only a few hits, greatly reduce the allowed gap
    if (littleCluster -> hit_vector.size() < 4) clust1_length = 2.0;
    if ( separation > _length_jump_scale * clust1_length) {
      if (_debug) std::cout << "\tFailed Separation cut, too far away\n";
      return false;
    }
    if (separation < 0 && fabs(separation) > clust1_length / 2.0 ) {
      if (_debug) std::cout << "\tFailed Separation cut, too far back\n";
      return false;
    }


    // at this point, all checks are passed
    if (_debug) std::cout << "Successful merge!\n";
    return true;

  }
  else {
    return false;
  }



  return false;
}



bool CBAlgoMergeExtendBlob::isInFrontOf(
  const ::cluster::cluster_params &cluster1,
  const ::cluster::cluster_params &cluster2)
{

  // Check the direction of cluster1 by comparing its start point to it's center
  float displacement_w = cluster1.mean_x - cluster1.start_point.w;
  float displacement_t = cluster1.mean_y - cluster1.start_point.t;

  // Project along this direction from the start of cluster1 to the start of cluster2
  TVector2 start1, start2, direction;
  start1.Set(cluster1.start_point.w, cluster1.start_point.t);
  start2.Set(cluster2.start_point.w, cluster2.start_point.t);
  direction.Set(displacement_w, displacement_t);

  // if start2 is filled with junk, replace it with the first hit
  if (start2.X() == -999.999) {
    start2.Set(cluster2.hit_vector.front().w, cluster2.hit_vector.front().t);
  }

  float t = (start2 - start1) * direction;
  if (t > 0) {
    if (_debug) {
      std::cout << "\n\n(" << start1.X() << ", " << start1.Y() << ") ";
      std::cout << "is in front of ";
      std::cout << "(" << start2.X() << ", " << start2.Y() << ")";
      std::cout << " -- Plane " << cluster1.plane_id.Plane << ", strength " << cluster1.eigenvalue_principal << std::endl;
    }
    return true;
  }
  else {
    if (_debug) {
      std::cout << "\n\n(" << start1.X() << ", " << start1.Y() << ")";
      std::cout << "is behind ";
      std::cout << "(" << start2.X() << ", " << start2.Y() << ")";
      std::cout << " -- Plane " << cluster1.plane_id.Plane << std::endl;
    }
    return false;
  }

  return false;
}

float CBAlgoMergeExtendBlob::getClosestApproachTo(
  const ::cluster::cluster_params &cluster1,
  const ::cluster::cluster_params &cluster2)
{

  // Project along this direction from the start of cluster1 to the center of cluster2
  TVector2 start1, center2, direction;
  start1.Set(cluster1.start_point.w, cluster1.start_point.t);
  center2.Set(cluster2.mean_x , cluster2.mean_y);
  direction.Set(cluster1.principal_dir[0], cluster1.principal_dir[1]);

  // if center2 is filled with junk, replace it with the first hit
  if (center2.X() == -999.999) {
    center2.Set(cluster2.hit_vector.front().w, cluster2.hit_vector.front().t);
  }

  float t = (center2 - start1) * direction;
  t /= direction.Mod2();

  // Distance can now be calculated:
  float distance = (start1 + t * direction - center2).Mod();
  if (_debug) std::cout << "\tclosest approach is " << distance << std::endl;
  return distance;
  // if (t > 0) {
  //   // std::cout << "("<<start1.X() << ", " << start1.Y() << ") ";
  //   // std::cout << "is in front of ";
  //   // std::cout << "("<<center2.X() << ", " << center2.Y() << ")\n";
  //   return true;
  // }
  // else{
  //   // std::cout << "("<<start1.X() << ", " << start1.Y() << ")";
  //   // std::cout << "is behind ";
  //   // std::cout << "("<<center2.X() << ", " << center2.Y() << ")\n";
  //   return false;
  // }
}

float CBAlgoMergeExtendBlob::getSeparation(
  const ::cluster::cluster_params &cluster1,
  const ::cluster::cluster_params &cluster2) {
  // Get the separation between the two clusters.  Find the highest
  // and lowest points along the principal axis of cluster 1.  Then subtract
  // the distance along the axis between the two: start2 - end1
  // don't return negative, truncate at 0

  TVector2 start1, start2, direction;
  direction.Set(cluster1.principal_dir[0], cluster1.principal_dir[1]);
  TVector2 ort = direction.Ort();

  // The direction and the orthogonal direction function as a rotation matrix to the
  // coords of principal axes

  float top_clust1 = -9999;
  float bot_clust2 =  9999;

  auto top_hit = cluster1.hit_vector.front();
  auto bot_hit = cluster2.hit_vector.front();


  // find the top of cluster 1
  for (auto & hit : cluster1.hit_vector ) {
    if (hit.w * direction.X() + hit.t * ort.X() > top_clust1) {
      top_clust1 = hit.w * direction.X() + hit.t * ort.X();
      top_hit = hit;
    }
  }

  // find the bottom of cluster 2
  for (auto & hit : cluster2.hit_vector ) {
    if (hit.w * direction.X() + hit.t * ort.X() < bot_clust2) {
      bot_clust2 = hit.w * direction.X() + hit.t * ort.X();
      bot_hit = hit;
    }
  }
  if (_debug) {
    std::cout << "\tTop is (" << top_hit.w << ", " << top_hit.t << "), bot is "
              <<  "(" << bot_hit.w << ", " << bot_hit.t << ") - separation "
              << bot_clust2 - top_clust1 << "\n";
  }

  return bot_clust2 - top_clust1;
}

float CBAlgoMergeExtendBlob::getLength(
  const ::cluster::cluster_params &cluster1) {

  TVector2 start1, start2, direction;
  direction.Set(cluster1.principal_dir[0], cluster1.principal_dir[1]);
  TVector2 ort = direction.Ort();

  // The direction and the orthogonal direction function as a rotation matrix to the
  // coords of principal axes

  float top_clust1 = -9999;
  float bot_clust1 =  9999;

  auto top_hit = cluster1.hit_vector.front();
  auto bot_hit = cluster1.hit_vector.back();


  for (auto & hit : cluster1.hit_vector ) {
    if (hit.w * direction.X() + hit.t * ort.X() > top_clust1) {
      top_clust1 = hit.w * direction.X() + hit.t * ort.X();
      top_hit = hit;
    }
    if (hit.w * direction.X() + hit.t * ort.X() < bot_clust1) {
      bot_clust1 = hit.w * direction.X() + hit.t * ort.X();
      bot_hit = hit;
    }
  }
  if (_debug) {
    std::cout << "\tTop of clust is (" << top_hit.w << ", " << top_hit.t << "), bot is "
              <<  "(" << bot_hit.w << ", " << bot_hit.t << ") - clust length "
              << fabs(top_clust1 - bot_clust1) << "\n";
  }

  return fabs(bot_clust1 - top_clust1);

}

float CBAlgoMergeExtendBlob::getRMSAlongAxis(
  const ::cluster::cluster_params &cluster1) {

  TVector2 start1, start2, direction;
  direction.Set(cluster1.principal_dir[0], cluster1.principal_dir[1]);
  TVector2 ort = direction.Ort();

  // The direction and the orthogonal direction function as a rotation matrix to the
  // coords of principal axes

  // In this frame the offset from principal axis ought to be zero, so just sum the squared val

  float rms = 0.0;

  for (auto & hit : cluster1.hit_vector ) {
    rms += pow(hit.w * direction.Y() - hit.t * ort.Y(), 2);
  }

  rms = sqrt(rms);

  if (_debug)
    std::cout << "\twidth is  "
              << rms << "\n";

  return rms;

}

}

#endif
