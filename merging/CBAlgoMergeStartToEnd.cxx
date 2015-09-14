#ifndef RECOTOOL_CBALGOMERGESTARTTOEND_CXX
#define RECOTOOL_CBALGOMERGESTARTTOEND_CXX

#include "CBAlgoMergeStartToEnd.h"
#include "math.h"
#include <iostream>
#include "CMTool/CMToolBase/CBoolAlgoBase.h"


namespace argomerge {

//defining best_slope
float CBAlgoMergeStartToEnd::best_slope (const ::cluster::cluster_params & cluster) {

  auto hit_v = cluster.hit_vector ;
  float diff_x_sq = 0;
  float diff_x_by_diff_y = 0;
  float mean_x = cluster.mean_x;
  float mean_y = cluster.mean_y;
  float cluster_slope = 0;

    for(auto const& hit_index : hit_v) {
      if (hit_index.charge > 0.10 * cluster.sum_charge)
        diff_x_by_diff_y += (hit_index.w - mean_x) * (hit_index.t - mean_y) ;
        diff_x_sq += pow((hit_index.w - mean_x), 2) ;

    }
    //redefine cluster_slope here
    cluster_slope = diff_x_by_diff_y / diff_x_sq ;

  return cluster_slope ;
}

//defining getShortestDist
float CBAlgoMergeStartToEnd::getShortestDist(
      const ::cluster::cluster_params &cluster1,
      const ::cluster::cluster_params &cluster2)
  {


    float minDist(9999.0);
    for (auto & hit : cluster2.hit_vector){
      for (auto & otherHit : cluster1.hit_vector){
        float thisDist = sqrt(pow(hit.w - otherHit.w,2 ) + pow(hit.t - otherHit.t, 2));
        if ( thisDist < minDist){
          minDist = thisDist;
        }
      }
    }
    return minDist;
  }

  //defining touching
  bool CBAlgoMergeStartToEnd::touching (const ::cluster::cluster_params & cluster1, const ::cluster::cluster_params & cluster2) {

  auto hit_v_1 = cluster1.hit_vector ;
  auto hit_v_2 = cluster2.hit_vector ;
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
      const ::cluster::cluster_params &cluster1,
      const ::cluster::cluster_params &cluster2)
  {

    // ::cluster::cluster_params poly1 = cluster1;
    // ::cluster::cluster_params poly2 = cluster2;
    auto overlap = cluster1.PolyObject.PolyOverlap(cluster2.PolyObject);
    auto overlapPoly = Polygon2D(cluster1.PolyObject,cluster2.PolyObject);
    
    float _max_distance = 1; 
    float start_to_end = pow(pow((cluster1.start_point.w - cluster2.end_point.w), 2) + pow((cluster1.start_point.t - cluster2.end_point.t), 2), 0.5); 
    float start_to_start = pow(pow((cluster1.start_point.w - cluster2.start_point.w), 2) + pow((cluster1.start_point.t - cluster2.start_point.t), 2), 0.5); 
    float end_to_end = pow(pow((cluster1.end_point.w - cluster2.end_point.w), 2) + pow((cluster1.end_point.t - cluster2.end_point.t), 2), 0.5); 
    float slope1 = best_slope(cluster1) ;
    float slope2 = best_slope(cluster2) ;
    // angle between in degrees
    float angle_between = atan(abs((slope1 - slope2)/(1 + slope1*slope2))) * 180/M_PI ;

    
    auto st = cluster1.start_point ;
    std::pair<double,double> start(st.w, st.t) ;

 
    // do not merge small clusters -- "one hit wonders"
    if (cluster1.hit_vector.size() < 3 && cluster2.hit_vector.size() < 3)
      return false ;

    else
      // if clusters are not pointing in the same direction, do not merge
      if (cluster1.direction != cluster2.direction)
        return false ;

      // does not merge vertices
      else if (start_to_start < 2)
        return false ;

      // does not merge vertices, part 2
      else if ((start_to_start - getShortestDist(cluster1, cluster2)) < 2)
        return false ;

      // does not merge clusters whose ends are closer than start to end
      else if (abs(start_to_end - end_to_end) < 4)
        return false ;

      // does not merge clusters whose ends are closer than start to end
      else if (start_to_end < 2 || (overlap && start_to_end < 4))
        return false ;

      // does not merge clusters that get progressively farther apart
      else if (abs(start_to_start - end_to_end) > 2)
        return false ;

      // merge in-line clusters
      else if (start_to_end < 5 && angle_between < 3)
        return true ;

      // merge shower branching to trunks
      else if (overlapPoly.PointInside(start) == true && (abs(start_to_end -
							      start_to_start) < 2))
        return true ;

      //merge polygons that overlap a lot
      else if (overlapPoly.Area() > 0.3 * cluster1.PolyObject.Area())
        return true ;

      // does not merge clusters whose overlapping area is small
      else if (overlapPoly.Area() < 0.75)
        return false ;

      // merge overlapping clusters
      else if (overlap && angle_between < 3 && start_to_end < start_to_start)
        return true ;

      // merge if overlap area is above a certain value
      else if (overlapPoly.Area() > 5)
  	return true ; 

      // merge if shortest distance between clusters is small
      else if (getShortestDist(cluster1, cluster2) < _max_distance && angle_between < 4)
        return true ;

      // merge touching clusters
      else if (touching(cluster1, cluster2) == true && 
	       angle_between < 3 && start_to_end < start_to_start)
        return true ;

      // merge remaining overlap
      else if (overlap || getShortestDist(cluster1, cluster2) < 3)
        return true ;

      else
        return false ;

  }
}





#endif

