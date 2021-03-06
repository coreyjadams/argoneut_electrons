/**
 * \file CBAlgoProhibitStartToStart.h
 *
 * \ingroup CMTool
 * 
 * \brief Class def header for a class CBAlgoProhibitStartToStart
 *
 * @author Corey Adams
 */

/** \addtogroup CMTool

    @{*/
#ifndef RECOTOOL_CBALGOPROHIBITSTARTTOSTART_H
#define RECOTOOL_CBALGOPROHIBITSTARTTOSTART_H

#include <iostream>
#include "CMTool/CMToolBase/CBoolAlgoBase.h"


namespace argomerge {
  /**
     \class CMalgoPolyContain
     Merge Polygons if the two overlap even partially
  */
  class CBAlgoProhibitStartToStart : public ::cmtool::CBoolAlgoBase{
    
  public:
    
    /// Default constructor
    CBAlgoProhibitStartToStart();
    
    /// Default destructor
    virtual ~CBAlgoProhibitStartToStart(){};
 
    /**
       Core function: given the cluster_params input, return whether a cluster should be
       merged or not.
    */
    virtual bool Bool(const ::cluster::cluster_params &cluster1,
                      const ::cluster::cluster_params &cluster2);

    void SetMinSeparation(float n){_min_separation = n;}
    void SetMinHits(int n){_min_hits = n;}

    float getDistance(larutil::PxPoint p1, larutil::PxPoint p2);
    void SetDebug(bool b){debug = b;}

  private:
    
    bool debug;
    float _min_separation;
    int _min_hits;
  };
}

#endif
/** @} */ // end of doxygen group 

