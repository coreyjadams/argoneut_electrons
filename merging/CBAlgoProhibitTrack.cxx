#ifndef RECOTOOL_CBALGOPROHIBITTRACK_CXX
#define RECOTOOL_CBALGOPROHIBITTRACK_CXX

#include "CBAlgoProhibitTrack.h"

namespace cmtool {

  //-------------------------------------------------------
  CBAlgoProhibitTrack::CBAlgoProhibitTrack() : CBoolAlgoBase()
  //-------------------------------------------------------
  {
    SetMinEP(.990000);
  }

  //-----------------------------
  void CBAlgoProhibitTrack::Reset()
  //-----------------------------
  {

  }

  //------------------------------------------------------------------------------------------
  //void CBAlgoProhibitAllTracks::EventBegin(const std::vector<cluster::ClusterParamsAlg> &clusters)
  //------------------------------------------------------------------------------------------
  //{
  //
  //}

  //-------------------------------
  //void CBAlgoProhibitAllTracks::EventEnd()
  //-------------------------------
  //{
  //
  //}

  //-----------------------------------------------------------------------------------------------
  //void CBAlgoProhibitAllTracks::IterationBegin(const std::vector<cluster::ClusterParamsAlg> &clusters)
  //-----------------------------------------------------------------------------------------------
  //{
  //
  //}

  //------------------------------------
  //void CBAlgoProhibitAllTracks::IterationEnd()
  //------------------------------------
  //{
  //
  //}
  
  //----------------------------------------------------------------
  bool CBAlgoProhibitTrack::Bool(const ::cluster::ClusterParamsAlg &cluster1,
			       const ::cluster::ClusterParamsAlg &cluster2)
  //----------------------------------------------------------------
  {
    //return true means don't prohibit these two clusters
    if(cluster1.GetParams().eigenvalue_principal > _min_EP ||
       cluster2.GetParams().eigenvalue_principal > _min_EP) 
      {
	if(_verbose) 
	  std::cout<<"Prohibiting clusters with EP's of "
		   <<cluster1.GetParams().eigenvalue_principal
		   <<" and "
		   <<cluster2.GetParams().eigenvalue_principal
		   <<std::endl;
	return true;
      }
    return false;
  }

  //------------------------------
  void CBAlgoProhibitTrack::Report()
  //------------------------------
  {

  }
    
}
#endif
