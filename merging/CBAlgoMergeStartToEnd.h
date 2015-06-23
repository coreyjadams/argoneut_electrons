/**
 * \file CBAlgoMergeStartToEnd.h
 *
 * \ingroup CMTool
 * 
 * \brief Class def header for a class CBAlgoMergeStartToEnd
 * This algorithm will merge small hits into nearby large clusters
 * You can set the minimum distance, which will be the closest approach
 * as well as the average distance between all hits.
 *
 * @author Nickie  
 */

/** \addtogroup CMTool

    @{*/
#ifndef RECOTOOL_CBALGOMERGESTARTTOEND_H
#define RECOTOOL_CBALGOMERGESTARTTOEND_H

#include <iostream>
#include "CMTool/CMToolBase/CBoolAlgoBase.h"
#include "LArUtil/GeometryUtilities.h"


namespace cmtool {
  /**
     \class CMalgoPolyContain
     Merge Polygons if the two overlap even partially
  */
  class CBAlgoMergeStartToEnd : public CBoolAlgoBase{
    
  public:
    
    /// Default constructor
    CBAlgoMergeStartToEnd();
    
    /// Default destructor
    virtual ~CBAlgoMergeStartToEnd(){};
 
    /**
       Core function: given the ClusterParamsAlg input, return whether a cluster should be
       merged or not.
    */
    virtual bool Bool(const ::cluster::ClusterParamsAlg &cluster1,
                      const ::cluster::ClusterParamsAlg &cluster2);


  private:
    
    float _max_distance;
  };
}

#endif
/** @} */ // end of doxygen group 

