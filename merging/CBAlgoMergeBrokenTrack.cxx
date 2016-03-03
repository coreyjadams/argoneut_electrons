#ifndef RECOTOOL_CBALGOMERGEBROKENTRACK_CXX
#define RECOTOOL_CBALGOMERGEBROKENTRACK_CXX

#include "CBAlgoMergeBrokenTrack.h"
#include "TVector2.h"
#include "TPrincipal.h"

namespace argomerge {

CBAlgoMergeBrokenTrack::CBAlgoMergeBrokenTrack() : CBoolAlgoBase()
{

  // Set parameters for merging candadates

  _max_wire_gap = 3 * 0.4; // centimeters
  _max_time_gap = 0.4; // centimeters
  _max_dist_gap = 2.0 * 0.4;
  _min_cos_angle_diff = 0.98;

  _deadWireStart = 66;
  _deadWireEnd   = 72;

  _min_hits = 5;

  _debug = false;

}

bool CBAlgoMergeBrokenTrack::Bool(
  const ::cluster::cluster_params &cluster1,
  const ::cluster::cluster_params &cluster2)
{

  // Checking if two clusters are very parallel, and if they
  // should be merged.

  if (cluster1.N_Hits < _min_hits || cluster2.N_Hits < _min_hits)
    return false;


  // The wire gap rule is enforced except in the region of known dead wires.

  size_t index1(0), index2(0);
  float _wire_gap = getWireGap(cluster1, cluster2, index1, index2);

  auto wire1 = cluster1.hit_vector.at(index1).w;
  auto wire2 = cluster2.hit_vector.at(index2).w;

  auto time1 = cluster1.hit_vector.at(index1).t;
  auto time2 = cluster2.hit_vector.at(index2).t;

  float time_gap = fabs(time1 - time2);

  // if (round(wire1/0.4) == 66 || round(wire2/0.4) == 66)
  //   _debug = true;

  if (_debug) {
    std::cout << "\nWire1: " << wire1
              << "\ttime1: " << time1
              << "\tn_hits: " << cluster1.hit_vector.size() << "\n"
              << "wire2: " << wire2
              << "\ttime2: " << time2
              << "\tn_hits: " << cluster2.hit_vector.size() << "\n"
              << " gap_w: " << _wire_gap
              << "\t gap_t: " << time_gap
              << std::endl;
  }

  // First, find out the wire gap is ok.
  bool _gap_exception = false;
  if (_wire_gap > _max_wire_gap) {
    // check if the closest approach is either side of the dead wires
    // if not, return false
    if (cluster1.plane_id.Plane == 0)
      _gap_exception = false;
    else {

      // Allowing exception to reach across the gap plus or minus 1 wire

      if (wire1 < wire2) {
        if (fabs( (wire1 ) - 0.4 * _deadWireStart ) < 0.5 &&
            fabs( (wire2 ) - 0.4 * _deadWireEnd ) < 0.5) {
          _gap_exception = true;
        }


      } else {
        if (fabs( (wire2 ) - 0.4 * _deadWireStart ) < 0.5 &&
            fabs( (wire1 ) - 0.4 * _deadWireEnd ) < 0.5) {
          _gap_exception = true;
        }
      }
    }
  }

  // Now, check if we should return right away:
  if (_wire_gap > _max_wire_gap && !_gap_exception) {
    if (_debug) {
      std::cout << "fail because gap too big and no exception.\n";
    }
    return false;
  }
  else {
    if (_wire_gap < 0) {
      if (_debug) {
        std::cout << "fail because gap is negative.\n";
      }
      return false;
    }

    // Check against the angle between the two clusters, using PCA:
    auto & dir1 = cluster1.principal_dir;
    auto & dir2 = cluster2.principal_dir;

    // Compute the angle between by just using the dot product:
    float cos_theta = fabs(dir1[0] * dir2[0] + dir1[1] * dir2[1]);
    // (only caring about parallel, and not directions)

    if (cos_theta < _min_cos_angle_diff) {
      if (_debug) {
        std::cout << "fail because cos_theta too low on first cut.\n";
      }
      return false;
    }


    // std::cout << "cos theta is " << cos_theta << std::endl;


    // For clusters jumping the big gap:
    if (_wire_gap > _max_wire_gap) {
      // Lastly, draw a line from the means of each cluster to each other
      // and the hits that are adjacent to the gap on the OTHER cluster.
      // Check that these lines also pass the "cos_theta" criteria.
      //
      // This is to prevent parallel but displaced clusters from merging.
      //
      TVector2 line1;
      TVector2 line2;

      line1.SetX( cluster1.hit_vector.at(index1).w
                  - cluster2.hit_vector.at(index2).w);
      line1.SetY( cluster1.hit_vector.at(index1).t
                  - cluster2.hit_vector.at(index2).t);
      line2.SetX( cluster1.mean_x - cluster2.mean_x);
      line2.SetY( cluster1.mean_y - cluster2.mean_y);

      line1 *= 1.0 / line1.Mod();
      line2 *= 1.0 / line2.Mod();

      cos_theta = fabs(line1.X() * line2.X() + line1.Y() * line2.Y());

      // std::cout << "cos theta 2 is " << cos_theta << std::endl;

      if (cos_theta < _min_cos_angle_diff) {
        if (_debug) {
          std::cout << "fail because cos_theta too low on second cut, version 1.\n";
        }
        return false;
      }
    }
    else {

      if (time_gap > _max_time_gap) {
        if (_debug) {
          std::cout << "fail because max time gap is too big, and wire gap is small\n";
        }
        return false;
      }

      // For clusters that are broken with a truly small gap,
      // the previous technique doesn't work.
      // Instead, form a temporary new cluster and run the PCA on it,
      // and then compare the strength of it's eigenvalue and direction
      // to the old ones
      TPrincipal fPrincipal(2, "D");
      for (auto& hit : cluster1.hit_vector) {
        // std::cout << "This hit has charge " <<  hit.charge << "\n";

        // Cast the wire/time information into a double array for prin. comp. analysis
        double data[2];
        data[0] = hit.w;
        data[1] = hit.t;
        // Add this data point to the PCA
        fPrincipal.AddRow(data);
      }
      for (auto& hit : cluster2.hit_vector) {
        // std::cout << "This hit has charge " <<  hit.charge << "\n";

        // Cast the wire/time information into a double array for prin. comp. analysis
        double data[2];
        data[0] = hit.w;
        data[1] = hit.t;
        // Add this data point to the PCA
        fPrincipal.AddRow(data);
      }

      // Run the PCA analysis
      fPrincipal.MakePrincipals();

      // Save the principal direction
      std::vector<float> principal_dir(2);
      // std::cout << (* fPrincipal.GetEigenVectors())[0][0] << ", " << (* fPrincipal.GetEigenVectors())[0][1] << "\n"
      // << (* fPrincipal.GetEigenVectors())[1][0] << ", " << (* fPrincipal.GetEigenVectors())[1][1] << "\n";

      principal_dir[0] = (* fPrincipal.GetEigenVectors())[0][0];
      principal_dir[1] = (* fPrincipal.GetEigenVectors())[1][0];

      // Now, we can make checks against the merged cluster
      TVector2 line2;
      line2.SetX( cluster1.mean_x - cluster2.mean_x);
      line2.SetY( cluster1.mean_y - cluster2.mean_y);

      line2 *= 1.0 / line2.Mod();

      cos_theta = fabs(line2.X() * principal_dir[0] + line2.Y() * principal_dir[1]);

      if (_debug) {
        std::cout << "very short gap comparison, wire1: " << wire1
                  << " wire2: " << wire2 << " cos_theta: " << cos_theta
                  << std::endl;
      }

      if (cos_theta < _min_cos_angle_diff) {
        if (_debug) {
          std::cout << "fail because cos_theta too low on second cut, version 2.\n";
        }
        return false;
      }

    }
    if (_debug) {
      std::cout << "pass!\n";
    }
    return true;
  }

  return false;

}


float CBAlgoMergeBrokenTrack::getWireGap(cluster::cluster_params clust_i,
    cluster::cluster_params clust_j,
    size_t & index_i,
    size_t & index_j
                                        ) {
  // First, determine the upper and lower wire bounds of each cluster
  // if they aren't [lower_i, upper_i, lower_j,upper_j] (or i <-> j)
  // then they are overlapping, and return -1

  auto geom = larutil::Geometry::GetME();

  float lower_i(clust_i.hit_vector.front().w);
  float lower_j(clust_j.hit_vector.front().w);
  float upper_i(clust_i.hit_vector.back().w);
  float upper_j(clust_j.hit_vector.back().w);

  size_t lower_i_index(0);
  size_t lower_j_index(0);
  size_t upper_i_index(clust_i.hit_vector.size() - 1);
  size_t upper_j_index(clust_j.hit_vector.size() - 1);

  size_t index = 0;
  for (auto hit : clust_i.hit_vector) {
    float wire = hit.w;
    if (wire < lower_i) {
      lower_i = wire;
      lower_i_index = index;
    }
    if (wire > upper_i) {
      upper_i = wire;
      upper_i_index = index;
    }
    index++;
  }

  index = 0;
  for (auto hit : clust_j.hit_vector) {
    float wire = hit.w;
    if (wire < lower_j) {
      lower_j = wire;
      lower_j_index = index;
    }
    if (wire > upper_j) {
      upper_j = wire;
      upper_j_index = index;
    }
    index++;
  }

  // Now check against overlap:
  if (lower_j < lower_i) {
    if (upper_j >= lower_i)
      return -1;
  }
  else { // lower_i <= lower_j
    if (upper_i >= lower_j)
      return -1;
  }

  // At this point, the wires regions don't overlap;
  // Figure out how many wires apart they are:
  float _wire_gap(0);
  if (lower_j < lower_i) {
    _wire_gap = lower_i - upper_j;
    index_i = lower_i_index;
    index_j = upper_j_index;
  }
  else { // lower_i <= lower_j
    _wire_gap = lower_j - upper_i;
    index_i = upper_i_index;
    index_j = lower_j_index;
  }


  return _wire_gap;



}



float CBAlgoMergeBrokenTrack::closestApproach(
  const ::cluster::cluster_params & cluster1,
  const ::cluster::cluster_params & cluster2)
{

  unsigned int npoints1 = cluster1.PolyObject.Size();
  unsigned int npoints2 = cluster2.PolyObject.Size();
  float bound = 10000;

  //loop over points on first polygon
  for (unsigned int i = 0; i < npoints1; ++i) {

    float pt1w = cluster1.PolyObject.Point(i).first;
    float pt1t = cluster1.PolyObject.Point(i).second;

    //loop over points on second polygon
    for (unsigned int j = 0; j < npoints2; ++j) {

      float pt2w = cluster2.PolyObject.Point(j).first;
      float pt2t = cluster2.PolyObject.Point(j).second;
      float dist = pow((pow(pt2w - pt1w, 2) + pow(pt2t - pt1t, 2)), 0.5);

      if (dist < bound) bound = dist;
    }
  }

  return bound;
}


}

#endif
