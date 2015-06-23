#ifndef RECOTOOL_CBALGOMERGESTARTTOENDANCE_CXX
#define RECOTOOL_CBALGOMERGESTARTTOENDANCE_CXX

#include "CBAlgoMergeStartToEnd.h"
#include <iostream>
#include "CMTool/CMToolBase/CBoolAlgoBase.h"


namespace cmtool {

  
  CBAlgoMergeStartToEnd::CBAlgoMergeStartToEnd() : CBoolAlgoBase()
  {
  }
  

  bool CBAlgoMergeStartToEnd::Bool(
      const ::cluster::ClusterParamsAlg &cluster1,
      const ::cluster::ClusterParamsAlg &cluster2)
  {

    Polygon2D overlapPoly = Polygon2D(const Polygon2D &cluster1.GetParams().PolyObject, const Polygon2D &cluster2.GetParams().PolyObject);
    Polygon2D polyArea = overlapPoly.PolyObject.Area();
    int maxDist = 0.5
    float actDist1 = pow(pow((cluster1.GetParams().start_point.w - cluster2.GetParams().end_point.w), 2) + pow((cluster1.GetParams().start_point.t - cluster2.GetParams().end_point.t), 2)), 0.5);
    float actDist2 = pow(pow((cluster1.GetParams().end_point.w - cluster2.GetParams().start_point.w), 2) + pow((cluster1.GetParams().end_point.t - cluster2.GetParams().start_point.t), 2)), 0.5);


    if (polyArea != 0)                                      //if the clusters actually overlap, return true to merge
      return true;
    else if (actDist1 < maxDist || actDist2 < maxDist)      //if the clusters' start and end points are within a certain distance of each other, return true to merge
      return true;
    else 
      return false;
    
  }
}

#endif
