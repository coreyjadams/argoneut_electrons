#ifndef RECOTOOL_CBALGOPROHIBITBIGTOBIG_CXX
#define RECOTOOL_CBALGOPROHIBITBIGTOBIG_CXX

#include "CBAlgoProhibitBigToBig.h"

namespace cmtool {

  CBAlgoProhibitBigToBig::CBAlgoProhibitBigToBig() : CBoolAlgoBase()
  {
    // SetDebug(false);

    // SetMinHits(20);
    // SetMinModHitDens(1.4);
    // SetMinMHitWires(3.5);
    // SetMinCharge(100);
    // //"fPrincipal" is log(1-eigenvalue_principal)
    // // >-7 means EP > 0.99908
    // SetMinPrincipal(-6.);

    _max_hits = 1;
  }


  
  bool CBAlgoProhibitBigToBig::Bool(
      const ::cluster::cluster_params &cluster1,
      const ::cluster::cluster_params &cluster2)
  {

    // Determine if this combination is exactly one cluster with 
    // multiple hits and one cluster with a single hit:
    if (cluster1.hit_vector.size() > _max_hits && cluster2.hit_vector.size() > _max_hits){
      return true;
    }
    return false;
  }

}

#endif
