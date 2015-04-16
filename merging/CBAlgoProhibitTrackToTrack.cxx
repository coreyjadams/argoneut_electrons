#ifndef RECOTOOL_CBALGOPROHIBITTRACKTOTRACK_CXX
#define RECOTOOL_CBALGOPROHIBITTRACKTOTRACK_CXX

#include "CBAlgoProhibitTrackToTrack.h"

namespace cmtool {

  CBAlgoProhibitTrackToTrack::CBAlgoProhibitTrackToTrack() : CBoolAlgoBase()
  {
    SetMinHits(15);
    SetMinModHitDens(0);
    SetMinMHitWiresFraction(9999);
    SetMinPrincipal(9999);
    SetMinCharge(00);
    SetMinLengthWidthRatio(99999);
  }


  
  bool CBAlgoProhibitTrackToTrack::Bool(
      const ::cluster::ClusterParamsAlg &cluster1,
      const ::cluster::ClusterParamsAlg &cluster2)
  {

    // Don't bother looking if too small:
    if (cluster1.GetParams().N_Hits < _min_hits ||
        cluster2.GetParams().N_Hits < _min_hits)
    {
      return true;
    }

    if (isTrack(cluster1) && isTrack(cluster2)) return true;

    return false;

  }

/*---------------------------------------------------------------------*/
// Determine is a cluster is a track based on some parameters.

  bool CBAlgoProhibitTrackToTrack::isTrack(
    const ::cluster::ClusterParamsAlg &cluster)
  {

    auto & params = cluster.GetParams();


    float N_Hits               = params.N_Hits;
    float multi_hit_wiresRat   = params.multi_hit_wires / params.N_Wires;
    float eigenvalue_principal = params.eigenvalue_principal;
    float length = 99999;
    if (params.width != 0) 
      length = params.length / params.width;    


    // // Printing out debug information:
    // std::cout << "This cluster starts at "
    //           << params.start_point.w << ", "
    //           << params.start_point.t << " and stops at "
    //           << params.end_point.w << ", "
    //           << params.end_point.t << ".\n"
    //           << "\tN Hits : " << N_Hits << "\n"
    //           << "\tMHW ratio: " << multi_hit_wiresRat << "\n"
    //           << "\tEP: " << eigenvalue_principal << "\n"
    //           << "\tL/W: " << length  << "\n";



    if( 
          (N_Hits > _min_hits) 
       && (multi_hit_wiresRat < _min_multihit_wires_fraction)
       && (eigenvalue_principal > _min_principal)
       && (length > _min_length_width_ratio)
      )
    {
        // std::cout << "\t TRACK\n";
        return true;
    }

    // std::cout << "\t NOT TRACK\n";

    return false;
  }


}

#endif
