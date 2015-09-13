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
#ifndef RECOTOOL_CBALGOMERGEEXTENDBLOB_H
#define RECOTOOL_CBALGOMERGEEXTENDBLOB_H

#include <iostream>
#include "CMTool/CMToolBase/CBoolAlgoBase.h"
// #include "ClusterRecoUtil/Base/ClusterParams.h"
#include "LArUtil/GeometryUtilities.h"

#include "utils/TrackShower.h"

namespace cmtool {
  /**
     \class CMalgoPolyContain
     Merge Polygons if the two overlap even partially
  */
  class CBAlgoMergeExtendBlob : public CBoolAlgoBase{
    
  public:
    
    /// Default constructor
    CBAlgoMergeExtendBlob();
    
    /// Default destructor
    virtual ~CBAlgoMergeExtendBlob(){};
 
    /**
       Core function: given the cluster_params input, return whether a cluster should be
       merged or not.
    */
    virtual bool Bool(const ::cluster::cluster_params &cluster1,
                      const ::cluster::cluster_params &cluster2);



    bool isInFrontOf(          const ::cluster::cluster_params &cluster1,
                               const ::cluster::cluster_params &cluster2);

    float getClosestApproachTo(const ::cluster::cluster_params &cluster1,
                               const ::cluster::cluster_params &cluster2);

    float getSeparation(       const ::cluster::cluster_params &cluster1,
                               const ::cluster::cluster_params &cluster2);

    float getLength(           const ::cluster::cluster_params &cluster);

    float getRMSAlongAxis(     const ::cluster::cluster_params &cluster1);

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

