#ifndef RECOTOOL_CBALGOMERGESTARTTOEND_CXX
#define RECOTOOL_CBALGOMERGESTARTTOEND_CXX

#include "CBAlgoMergeStartToEnd.h"
#include "math.h"
#include "../../RecoTool/ClusterRecoUtil/ClusterParamsAlg.h"
#include <iostream>
#include "CMTool/CMToolBase/CBoolAlgoBase.h"


namespace cmtool {

float CBAlgoMergeStartToEnd::best_slope (const ::cluster::ClusterParamsAlg & cluster) {

    auto hit_v = cluster.GetHitVector() ;

//
  float diff_x_sq = 0;
  float diff_x_by_diff_y = 0;
  float mean_x = cluster.GetParams().mean_x;
  float mean_y = cluster.GetParams().mean_y;
  float cluster_slope = 0;

    for(auto const& hit_index : hit_v) {
      diff_x_by_diff_y += (hit_index.w - mean_x) * (hit_index.t - mean_y) ;
      diff_x_sq += pow((hit_index.w - mean_x), 2) ;

    }
    cluster_slope = diff_x_by_diff_y / diff_x_sq ;

  return cluster_slope ;
}
  
  CBAlgoMergeStartToEnd::CBAlgoMergeStartToEnd() : CBoolAlgoBase()
  {
  }
  

  bool CBAlgoMergeStartToEnd::Bool(
      const ::cluster::ClusterParamsAlg &cluster1,
      const ::cluster::ClusterParamsAlg &cluster2)
  {

    auto overlap = cluster2.GetParams().PolyObject.PolyOverlap(cluster1.GetParams().PolyObject ) ;
    auto overlapPoly = Polygon2D(cluster1.GetParams().PolyObject, cluster2.GetParams().PolyObject);
    

    //double polyArea = overlapPoly.PolyObject.Area();

    float maxDist = 1 ; //semicolon; Can't use "int" for non-integer (like 0.5).
    float actDist1 = pow(pow((cluster1.GetParams().start_point.w - cluster2.GetParams().end_point.w), 2) + pow((cluster1.GetParams().start_point.t - cluster2.GetParams().end_point.t), 2), 0.5); //extra parentheses
    float actDist2 = pow(pow((cluster1.GetParams().end_point.w - cluster2.GetParams().start_point.w), 2) + pow((cluster1.GetParams().end_point.t - cluster2.GetParams().start_point.t), 2), 0.5); //extra parentheses
    //new lines below
    float actDist3 = pow(pow((cluster1.GetParams().start_point.w - cluster2.GetParams().start_point.w), 2) + pow((cluster1.GetParams().start_point.t - cluster2.GetParams().start_point.t), 2), 0.5); //extra parentheses
    //float actDist4 = pow(pow((cluster1.GetParams().end_point.w - cluster2.GetParams().end_point.w), 2) + pow((cluster1.GetParams().end_point.t - cluster2.GetParams().end_point.t), 2), 0.5); //extra parentheses
    //auto avgAngle1 = (cluster1.GetParams().opening_angle + cluster1.GetParams().closing_angle) / 2 ;
    //auto avgAngle2 = (cluster2.GetParams().opening_angle + cluster2.GetParams().closing_angle) / 2 ;
    float rmsDist = pow(pow(abs(cluster1.GetParams().rms_x - cluster2.GetParams().rms_x), 2) + pow(abs(cluster1.GetParams().rms_y - cluster2.GetParams().rms_y), 2), 0.5);
    float slope1 = best_slope(cluster1) ;
    float slope2 = best_slope(cluster2) ;

    if (cluster1.GetNHits() < 3 && cluster1.GetNHits() < 3)
      return false;
    else
      if (cluster1.GetParams().direction != cluster2.GetParams().direction)
        return false;
      //else if (slope1 >= slope2 - 3 || slope1 <= slope2 + 3)
        //return false;
      //else if (overlapPoly.Area() < 4)
       //return false;
      else if ( overlap && ((actDist1 < 4 && actDist1 /*> 1*/) || (actDist2 < 4 && actDist2/* > 1*/)) && (slope1 >= slope2 - 0.25 || slope1 <= slope2 + 0.25) && overlapPoly.Area() > 4)
       return true;
      else if (((actDist1 < 4 && actDist1 /*> 1*/) || (actDist2 < 4 && actDist2 /*> 1*/)) && ((slope1 >= slope2 - 0.25 || slope1 <= slope2 + 0.25)))
        return true;
      else if (actDist3 < 0.05)
        return false;
      else if (rmsDist > maxDist)
       return false;
      else 
       return false;
    
    
  }
}





#endif

