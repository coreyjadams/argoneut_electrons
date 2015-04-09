/**
 * \file CBAlgoMergeSingleToBig.h
 *
 * \ingroup CMTool
 * 
 * \brief Class def header for a class CBAlgoMergeSingleToBig
 *
 * @author Corey Adams
 */

/** \addtogroup CMTool

    @{*/
#ifndef RECOTOOL_CBALGOMERGESINGLETOBIG_H
#define RECOTOOL_CBALGOMERGESINGLETOBIG_H

#include <iostream>
#include "CMTool/CMToolBase/CBoolAlgoBase.h"
#include "LArUtil/GeometryUtilities.h"


namespace cmtool {
  /**
     \class CMalgoPolyContain
     Merge Polygons if the two overlap even partially
  */
  class CBAlgoMergeSingleToBig : public CBoolAlgoBase{
    
  public:
    
    /// Default constructor
    CBAlgoMergeSingleToBig();
    
    /// Default destructor
    virtual ~CBAlgoMergeSingleToBig(){};
 
    /**
       Core function: given the ClusterParamsAlg input, return whether a cluster should be
       merged or not.
    */
    virtual bool Bool(const ::cluster::ClusterParamsAlg &cluster1,
                      const ::cluster::ClusterParamsAlg &cluster2);

    void SetDebug(bool debug) { _debug = debug; }

    bool isContainedBy(const ::cluster::ClusterParamsAlg &cluster1,
                       const ::cluster::ClusterParamsAlg &cluster2);

    // bool isTrack(const ::cluster::ClusterParamsAlg &cluster);
    // bool isSmall(const ::cluster::ClusterParamsAlg &cluster);
    // float closestApproach(const ::cluster::ClusterParamsAlg &cluster1,
                          // const ::cluster::ClusterParamsAlg &cluster2);

    // /// Setter for track-like parameters
    // void SetMinHits(size_t mh)          { _min_hits           = mh; }
    // void SetMinModHitDens(Double_t mhd) { _min_mod_hit_dens   = mhd; }
    // void SetMinMHitWires(Double_t mmhw) { _min_multihit_wires = mmhw; }
    // void SetMinPrincipal(Double_t mp)   { _min_principal      = mp; }
    // void SetMinCharge(Double_t charge)  { _min_charge         = charge;}

    // // Setter for small like parameters
    // void SetMaxHit(int mh)      {_max_hits   = mh;}
    // void SetMaxCharge(float mc) {_max_charge = mc;}
    // void SetMaxLength(float ml) {_max_length = ml;}
    // void SetMaxWidth(float mw)  {_max_width  = mw;}
    
    // // Setter for merging parameters
    // void SetMaxClosestDist(float mcd)  {_max_closest_dist=mcd;} 
    // void SetMinDistToStart(float mdts) {_min_dist_to_start=mdts;} 
    // void SetMinDistToEnd(float mdte)   {_min_dist_to_end=mdte;} 
  private:
    
    float _max_distance;

    // // Criteria for a cluster to be a "track"
    // int   _min_hits;
    // float _min_mod_hit_dens;
    // float _min_multihit_wires;
    // float _min_principal;
    // float _min_charge;

    // // Criteria for a cluster to be "small"
    // int   _max_hits;
    // float _max_charge;
    // float _max_length;
    // float _max_width;

    // // Criteria to merge a small cluster into a track: 
    // float _max_closest_dist;
    // float _min_dist_to_start;
    // float _min_dist_to_end;


    bool _debug;
    // size_t _min_hits;
  };
}

#endif
/** @} */ // end of doxygen group 

