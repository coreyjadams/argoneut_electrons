/**
 * \file CBAlgoMergeExtendBlob.h
 *
 * \ingroup CMTool
 * 
 * \brief Class def header for a class CBAlgoMergeExtendBlob
 * This algorithm looks along a shower's path for other things to suck in to it
 * For small clusters, it makes no requirements about what the other cluster is
 * For bigger clusters, it requires that the other cluster not be a track.
 * @author Corey Adams
 */

/** \addtogroup CMTool

    @{*/
#ifndef RECOTOOL_CBALGOMERGEEXTENDBLOBCOPY_H
#define RECOTOOL_CBALGOMERGEEXTENDBLOBCOPY_H

#include <iostream>
#include "CMTool/CMToolBase/CBoolAlgoBase.h"
#include "LArUtil/GeometryUtilities.h"

#include "utils/TrackShower.h"

namespace cmtool {
  /**
     \class CMalgoPolyContain
     Merge Polygons if the two overlap even partially
  */
  class CBAlgoMergeExtendBlobCOPY : public CBoolAlgoBase{
    
  public:
    
    /// Default constructor
    CBAlgoMergeExtendBlobCOPY();
    
    /// Default destructor
    virtual ~CBAlgoMergeExtendBlobCOPY(){};
 
    /**
       Core function: given the ClusterParamsAlg input, return whether a cluster should be
       merged or not.
    */
    virtual bool Bool(const ::cluster::ClusterParamsAlg &cluster1,
                      const ::cluster::ClusterParamsAlg &cluster2);



    bool isInFrontOf(          const ::cluster::ClusterParamsAlg &cluster1,
                               const ::cluster::ClusterParamsAlg &cluster2);

    float getClosestApproachTo(const ::cluster::ClusterParamsAlg &cluster1,
                               const ::cluster::ClusterParamsAlg &cluster2);

    float getSeparation(       const ::cluster::ClusterParamsAlg &cluster1,
                               const ::cluster::ClusterParamsAlg &cluster2);

    float getLength(           const ::cluster::ClusterParamsAlg &cluster);

    float getRMSAlongAxis(     const ::cluster::ClusterParamsAlg &cluster1);

    float best_slope (         const ::cluster::ClusterParamsAlg & cluster);

    float getShortestDist(     const ::cluster::ClusterParamsAlg &cluster1,     
			       const ::cluster::ClusterParamsAlg &cluster2);

    void set_principal_ev_cut(float f)      {_principal_ev_cut = f;}
    void set_rms_scale(float f)             {_rms_scale = f;}
    void set_length_jump_scale(float f)     {_length_jump_scale = f;}
    void set_min_hits_to_project_from(int f){_min_hits_to_project_from = f;}
    void set_mode(int m)                    {_mode = m;}
    void set_debug(bool b)                  {_debug = b;}

  private:
    
    // Mode of 0 (default) merges small clusters into the bigger one
    // Anything else merges downstream into upstream
    int _mode;

    unsigned int   _min_hits_to_project_from;


    float _principal_ev_cut;
    float _rms_scale;
    float _length_jump_scale;

    bool _debug;

    argo::TrackShower ts;

  };
}

#endif
/** @} */ // end of doxygen group 

