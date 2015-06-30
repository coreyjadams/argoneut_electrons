#ifndef RECOTOOL_CBALGOMERGESTARTTOEND_CXX
#define RECOTOOL_CBALGOMERGESTARTTOEND_CXX

#include "CBAlgoMergeStartToEnd.h"
#include "math.h"
#include "../../RecoTool/ClusterRecoUtil/ClusterParamsAlg.h"
#include <iostream>
#include "CMTool/CMToolBase/CBoolAlgoBase.h"


namespace cmtool {

//defining best_slope
float CBAlgoMergeStartToEnd::best_slope (const ::cluster::ClusterParamsAlg & cluster) {

  auto hit_v = cluster.GetHitVector() ;
  float diff_x_sq = 0;
  float diff_x_by_diff_y = 0;
  float mean_x = cluster.GetParams().mean_x;
  float mean_y = cluster.GetParams().mean_y;
  float cluster_slope = 0;

    for(auto const& hit_index : hit_v) {
      if (hit_index.charge > 0.10 * cluster.GetParams().sum_charge)
      	diff_x_by_diff_y += (hit_index.w - mean_x) * (hit_index.t - mean_y) ;
      	diff_x_sq += pow((hit_index.w - mean_x), 2) ;

    }
    //redefine cluster_slope here
    cluster_slope = diff_x_by_diff_y / diff_x_sq ;

  return cluster_slope ;
}

//defining getShortestDist
float CBAlgoMergeStartToEnd::getShortestDist(
      const ::cluster::ClusterParamsAlg &cluster1,
      const ::cluster::ClusterParamsAlg &cluster2)
  {


    float minDist(9999.0);
    for (auto & hit : cluster2.GetHitVector()){
      for (auto & otherHit : cluster1.GetHitVector()){
        float thisDist = sqrt(pow(hit.w - otherHit.w,2 ) + pow(hit.t - otherHit.t, 2));
        if ( thisDist < minDist){
          minDist = thisDist;
        }
      }
    }
    return minDist;
  }

  //defining touching
  bool CBAlgoMergeStartToEnd::touching (const ::cluster::ClusterParamsAlg & cluster1, const ::cluster::ClusterParamsAlg & cluster2) {

  auto hit_v_1 = cluster1.GetHitVector() ;
  auto hit_v_2 = cluster2.GetHitVector() ;
  bool touch = false;  

    for(auto const& hit_index_1 : hit_v_1) {
      for(auto const& hit_index_2 : hit_v_2) {
        if (pow((pow((hit_index_1.w - hit_index_2.w), 2) + pow((hit_index_1.t - hit_index_2.t), 2)), 0.5) >= 0.45 && hit_index_1.w > hit_index_2.w && hit_index_1.t > hit_index_2.t)
          touch = true;
	else if ((hit_index_1.w == hit_index_2.w || hit_index_1.t == hit_index_2.t) && (pow((pow((hit_index_1.w - hit_index_2.w), 2) + pow((hit_index_1.t - hit_index_2.t), 2)), 0.5) < 0.7))
          touch = true;
	else
          touch = false;
      }
    }
    return touch;
}
  

  //main 
  CBAlgoMergeStartToEnd::CBAlgoMergeStartToEnd() : CBoolAlgoBase()
  {
  }
  

  bool CBAlgoMergeStartToEnd::Bool(
      const ::cluster::ClusterParamsAlg &cluster1,
      const ::cluster::ClusterParamsAlg &cluster2)
  {

    ::cluster::ClusterParamsAlg poly1 = cluster1;
    ::cluster::ClusterParamsAlg poly2 = cluster2;
    poly1.FillPolygon(1.);
    poly2.FillPolygon(1.);
    auto overlap = poly1.GetParams().PolyObject.PolyOverlap(poly2.GetParams().PolyObject);
    auto overlapPoly = Polygon2D(poly1.GetParams().PolyObject,poly2.GetParams().PolyObject);
    //auto overlap = cluster2.GetParams().PolyObject.PolyOverlap(cluster1.GetParams().PolyObject ) ;
    //auto overlapPoly = Polygon2D(cluster1.GetParams().PolyObject, cluster2.GetParams().PolyObject);
    

    float maxDist = 1 ;
    float _max_distance = 0.125; 
    float actDist1 = pow(pow((cluster1.GetParams().start_point.w - cluster2.GetParams().end_point.w), 2) + pow((cluster1.GetParams().start_point.t - cluster2.GetParams().end_point.t), 2), 0.5); 
    float actDist2 = pow(pow((cluster1.GetParams().end_point.w - cluster2.GetParams().start_point.w), 2) + pow((cluster1.GetParams().end_point.t - cluster2.GetParams().start_point.t), 2), 0.5); 
    float actDist3 = pow(pow((cluster1.GetParams().start_point.w - cluster2.GetParams().start_point.w), 2) + pow((cluster1.GetParams().start_point.t - cluster2.GetParams().start_point.t), 2), 0.5); 
    float actDist4 = pow(pow((cluster1.GetParams().end_point.w - cluster2.GetParams().end_point.w), 2) + pow((cluster1.GetParams().end_point.t - cluster2.GetParams().end_point.t), 2), 0.5); 
    float rmsDist = pow(pow(abs(cluster1.GetParams().rms_x - cluster2.GetParams().rms_x), 2) + pow(abs(cluster1.GetParams().rms_y - cluster2.GetParams().rms_y), 2), 0.5);
    float slope1 = best_slope(cluster1) ;
    float slope2 = best_slope(cluster2) ;

    auto st = cluster1.GetParams().start_point ;
    std::pair<double,double> start(st.w, st.t) ;
 

    if (cluster1.GetNHits() < 3 && cluster2.GetNHits() < 3)
      return false;
    else
      if (cluster1.GetParams().direction != cluster2.GetParams().direction)
        return false;
      if (getShortestDist(cluster1, cluster2) > 1)
	return false;
      if (slope1 > slope2 + 0.25 || slope1 < slope2 - 0.25)
	return false;
      else if (overlap && actDist1 < actDist3 && (overlapPoly.Area() > 1) /*&& slope1 >= slope2 - 0.125 && slope1 <= slope2 + 0.125*/)
	if (touching(cluster1, cluster2) == true && actDist1 > actDist3)
	  return false;
	else
	  return true;
      else if ((overlapPoly.Area() > 0.2 * poly1.GetParams().PolyObject.Area() || overlapPoly.Area() > 0.2 * poly2.GetParams().PolyObject.Area()))
	if ((slope1 >= slope2 - 0.00125 && slope1 <= slope2 + 0.00125))
	  return false;
	else
	  return true;
      else if ((overlapPoly.Area() > 0.6 * poly1.GetParams().PolyObject.Area() || overlapPoly.Area() > 0.6 * poly2.GetParams().PolyObject.Area()))
	  return true;
      else if (overlapPoly.PointInside(start) == true)
	return true;
      else if ((slope1 >= slope2 - 0.0125 && slope1 <= slope2 + 0.0125) && touching(cluster1, cluster2) == true && actDist1 > actDist3)
	return true;
      else if (actDist1 < 2 && (slope1 >= slope2 - 0.25 && slope1 <= slope2 + 0.25))
	return true;
      else if (getShortestDist(cluster1, cluster2) < _max_distance)
        if (actDist3 > actDist1 || actDist3 > actDist2 || actDist4 > actDist1 || actDist4 > actDist2)
          return false;
        else
          return true;
      //try eliminating this line
      else if (rmsDist >= maxDist)
       return false;
      else if (actDist3 < 0.5)
        return false;     
      else if (touching(cluster1, cluster2) == true && ((slope1 >= slope2 - 0.75 || slope1 <= slope2 + 0.75)) && (actDist1 < rmsDist + 2 && actDist1 > rmsDist - 2))
        return true;
      else 
       return false;
    
    
  }
}





#endif

