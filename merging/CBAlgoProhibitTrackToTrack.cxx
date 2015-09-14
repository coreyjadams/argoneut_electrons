#ifndef RECOTOOL_CBALGOPROHIBITTRACKTOTRACK_CXX
#define RECOTOOL_CBALGOPROHIBITTRACKTOTRACK_CXX

#include "CBAlgoProhibitTrackToTrack.h"

namespace argomerge {

CBAlgoProhibitTrackToTrack::CBAlgoProhibitTrackToTrack() : CBoolAlgoBase()
{
  SetMinHits(15);
  SetMinModHitDens(0);
  SetMinMHitWiresFraction(9999);
  SetMinPrincipal(9999);
  SetMinCharge(00);
  SetMinLengthWidthRatio(99999);
  debug = false;
  _mode = kBOTH;
}



bool CBAlgoProhibitTrackToTrack::Bool(
  const ::cluster::cluster_params &cluster1,
  const ::cluster::cluster_params &cluster2)
{

  // Don't bother looking if too small:
  if (cluster1.N_Hits < _min_hits ||
      cluster2.N_Hits < _min_hits)
  {
    return true;
  }

  if (_mode == kBOTH) {
    if (isTrack(cluster1) && isTrack(cluster2)) return true;
  }

  else if (_mode == kEITHER) {
    if (isTrack(cluster1) || isTrack(cluster2)) return true;
  }

  return false;

}

void CBAlgoProhibitTrackToTrack::SetMode(mode m) {
  _mode = m;
}

/*---------------------------------------------------------------------*/
// Determine is a cluster is a track based on some parameters.

bool CBAlgoProhibitTrackToTrack::isTrack(const ::cluster::cluster_params &cluster)
{



  float N_Hits               = cluster.N_Hits;
  float multi_hit_wiresRat   = cluster.multi_hit_wires / cluster.N_Wires;
  float eigenvalue_principal = cluster.eigenvalue_principal;
  float length = 99999;
  if (cluster.width != 0)
    length = cluster.length / cluster.width;





  if (
    (N_Hits > _min_hits)
    && (multi_hit_wiresRat < _min_multihit_wires_fraction)
    && (eigenvalue_principal > _min_principal)
    && (length > _min_length_width_ratio)
  )
  {
    // Printing out debug information:
    if (debug) {
      std::cout << "This cluster starts at "
                << cluster.start_point.w << ", "
                << cluster.start_point.t << " and stops at "
                << cluster.end_point.w << ", "
                << cluster.end_point.t << ".\n"
                << "\tPlane: " << cluster.plane_id.Plane << "\n"
                << "\tN Hits : " << N_Hits << "\n"
                << "\tMHW ratio: " << multi_hit_wiresRat << "\n"
                << "\tEP: " << eigenvalue_principal << "\n"
                << "\tL/W: " << length  << "\n";
      std::cout << "\t TRACK\n";
    }
    return true;
  }

  // if (debug) std::cout << "\t NOT TRACK\n";

  return false;
}


}

#endif
