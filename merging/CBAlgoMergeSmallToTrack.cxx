#ifndef RECOTOOL_CBALGOMERGESMALLTOTRACK_CXX
#define RECOTOOL_CBALGOMERGESMALLTOTRACK_CXX

#include "CBAlgoMergeSmallToTrack.h"

namespace cmtool {

  CBAlgoMergeSmallToTrack::CBAlgoMergeSmallToTrack() : CBoolAlgoBase()
  {
    // Nothing to be done in the base class
    SetDebug(false);

    SetMinHits(20);
    SetMinModHitDens(1.4);
    SetMinMHitWires(3.5);
    SetMinCharge(100);
    //"fPrincipal" is log(1-eigenvalue_principal)
    // >-7 means EP > 0.99908
    SetMinPrincipal(-6.);

    this->reconfigure();
  }


  void CBAlgoMergeSmallToTrack::reconfigure(){

    //not sure what needs to be reset/reconfigured for this algo
    
  }//end reconfigure function

  
  bool CBAlgoMergeSmallToTrack::Bool(
      const ::cluster::ClusterParamsAlg &cluster1,
      const ::cluster::ClusterParamsAlg &cluster2)
  {

    return false;
  }

  bool CBAlgoMergeSmallToTrack::isTrack(
    const ::cluster::ClusterParamsAlg &cluster)
  {
    return false;
  }
  bool CBAlgoMergeSmallToTrack::isSmall(
    const ::cluster::ClusterParamsAlg &cluster)
  {
    return false;
  }
}

#endif
