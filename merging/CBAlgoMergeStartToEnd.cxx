#ifndef RECOTOOL_CBALGOMERGESTARTTOEND_CXX
#define RECOTOOL_CBALGOMERGESTARTTOEND_CXX

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

    //Use auto when you're not sure what the type is-- PolyOverlap is a boolean, not Polygon2D
    auto overlap = cluster2.GetParams().PolyObject.PolyOverlap(cluster1.GetParams().PolyObject ) ;
    //Polygon2D overlapPoly = Polygon2D(const Polygon2D &cluster1.GetParams().PolyObject, const Polygon2D &cluster2.GetParams().PolyObject);

    // Area() is type double, not Polygon2D 
    //Polygon2D polyArea = overlapPoly.PolyObject.Area();

    float maxDist = 0.5 ; //semicolon; Can't use "int" for non-integer (like 0.5).
    float actDist1 = pow(pow((cluster1.GetParams().start_point.w - cluster2.GetParams().end_point.w), 2) + pow((cluster1.GetParams().start_point.t - cluster2.GetParams().end_point.t), 2), 0.5); //extra parentheses
    float actDist2 = pow(pow((cluster1.GetParams().end_point.w - cluster2.GetParams().start_point.w), 2) + pow((cluster1.GetParams().end_point.t - cluster2.GetParams().start_point.t), 2), 0.5); //extra parentheses


    //You should not return true here-- you want to take more things into account than just whether or not the polygons overlap.  
    //For ex, you include cases here where the polygon overlap at the same vertex--definitely do not want to merge those
    //One slightly more complex example is :
    // if ( overlap && ( actDist1 <maxDist || actDist2 < maxDist ))
    if (overlap) 
      return true;
    else if (actDist1 < maxDist || actDist2 < maxDist)      //if the clusters' start and end points are within a certain distance of each other, return true to merge
      return true;
    else 
      return false;
    
  }
}

#endif
