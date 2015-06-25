#ifndef RECOTOOL_CBALGOMERGESTARTTOEND_CXX
#define RECOTOOL_CBALGOMERGESTARTTOEND_CXX

#include "CBAlgoMergeStartToEnd.h"
#include "../../RecoTool/ClusterRecoUtil/ClusterParamsAlg.h"
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
    auto overlapPoly = Polygon2D(cluster1.GetParams().PolyObject, cluster2.GetParams().PolyObject);
    

    // Area() is type double, not Polygon2D 
    //double polyArea = overlapPoly.PolyObject.Area();

    float maxDist = 2 ; //semicolon; Can't use "int" for non-integer (like 0.5).
    float actDist1 = pow(pow((cluster1.GetParams().start_point.w - cluster2.GetParams().end_point.w), 2) + pow((cluster1.GetParams().start_point.t - cluster2.GetParams().end_point.t), 2), 0.5); //extra parentheses
    float actDist2 = pow(pow((cluster1.GetParams().end_point.w - cluster2.GetParams().start_point.w), 2) + pow((cluster1.GetParams().end_point.t - cluster2.GetParams().start_point.t), 2), 0.5); //extra parentheses
    //new lines below
    float actDist3 = pow(pow((cluster1.GetParams().start_point.w - cluster2.GetParams().start_point.w), 2) + pow((cluster1.GetParams().start_point.t - cluster2.GetParams().start_point.t), 2), 0.5); //extra parentheses
    //float actDist4 = pow(pow((cluster1.GetParams().end_point.w - cluster2.GetParams().end_point.w), 2) + pow((cluster1.GetParams().end_point.t - cluster2.GetParams().end_point.t), 2), 0.5); //extra parentheses


    if (cluster1.GetNHits() < 3 && cluster1.GetNHits() < 3)
      return false;
    else
      if ( overlap && ( actDist1 < maxDist || actDist2 < maxDist ) && (cluster1.GetParams().angle_2d <= cluster2.GetParams().angle_2d + 0.05 || cluster1.GetParams().angle_2d >= cluster2.GetParams().angle_2d - 0.05)) 
        return true;
      else if (actDist1 < maxDist || actDist2 < maxDist)      //if the clusters' start and end points are within a certain distance of each other, return true to merge
        return true;
      else if ((actDist1 < 4 || actDist2 < 4) && (cluster1.GetParams().angle_2d <= cluster2.GetParams().angle_2d + 0.0125 || cluster1.GetParams().angle_2d >= cluster2.GetParams().angle_2d - 0.0125))
        return true;
      else if (actDist3 < maxDist)
        return false;
      else if (overlapPoly.Area() <= 0.5)
       return false;
      else 
       return false;
    
    
  }
}

#endif
