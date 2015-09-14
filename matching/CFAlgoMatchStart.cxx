#ifndef RECOTOOL_CFALGOMATCHSTART_CXX
#define RECOTOOL_CFALGOMATCHSTART_CXX

#include "CFAlgoMatchStart.h"

namespace argomatch {

  //-------------------------------------------------------
  CFAlgoMatchStart::CFAlgoMatchStart() : CFloatAlgoBase()
  //-------------------------------------------------------
  {
    SetRatioCut(0.001) ; //(0.095) ;  
    SetStartTimeCut(10) ;
    SetDebug(false) ;
    SetVerbose(false) ;
    RequireThreePlanes(false) ;
    ts.init();

  }

  //-----------------------------
  void CFAlgoMatchStart::Reset()
  //-----------------------------
  {

  }

  //----------------------------------------------------------------------------------------------
  float CFAlgoMatchStart::Float(const std::vector<const cluster::cluster_params*> &clusters)
  //----------------------------------------------------------------------------------------------
  {
    
    // Code-block by Kazu starts
    // This ensures the algorithm works only if # clusters is > 2 (and not =2)
    // You may take out this block if you want to allow matching using clusters from only 2 planes.
    if(_require_3planes && clusters.size()==2) return -1;
    // Code-block by Kazu ends

    // First, find the showers on the collection plane:
    cluster::cluster_params shower;
    // cluster::cluster_params other;
    int nshowers = 0;
    // Look for showers on the collection plane:
    for(auto const& c : clusters){
      if (ts.trackOrShower(*c) == argoutils::TrackShower::kShower && c -> plane_id.Plane == 1){
        // std::cout << "found a collection shower!\n";
        shower = *c;
        nshowers++;
      }
      // else{
      //   other = *c;
      // }
    }


    // print out the start point of the shower:
    auto start_point = shower.start_point;

    if (start_point.w != start_point.w){
      return -1;
    }

    

    // Find the closest hits to the start point
    int n_hits = 15;
    std::vector<Hit2D> close_hits = GetClosestHits(shower, start_point, n_hits, true);
    if (close_hits.size() < 10) return false;

    // std::cout << "\nhitlist is:\n";
    // for (int i = 0; i < n_hits; ++i)
    // {
    //   if (i >= close_hits.size()) break;
    //   std::cout << "(" << close_hits[i].w << ", " << close_hits[i].t << "): " << close_hits[i].charge << "\n";
    // }

    // try to fit a line to this hit list 

    // Compute the average displacement vector (unit)
    Point2D displacement;
    // std::cout << displacement.w << "-----" << displacement.t << "\n";
    for (auto & hit : close_hits){
      float dist = sqrt(dist = pow((hit.w - start_point.w),2) + pow((hit.w - start_point.w),2));
      if (dist == 0) continue;
      float addition_w = hit.w - start_point.w;
      float addition_t = hit.t - start_point.t;
      displacement.t += addition_t /((close_hits.size() -1)* dist);
      displacement.w += addition_w /((close_hits.size() -1)* dist);
    }
    // std::cout << displacement.w << "-----" << displacement.t << "\n";

    float step = displacement.w / 0.4;
    float chi2 = 0.0;

    for (auto & hit : close_hits){
      float disp = (hit.w - start_point.w)/displacement.w;
      chi2 += pow((hit.t - (start_point.t + disp*displacement.t)),2);
    }


    if (chi2 / n_hits < 0.1){
      // std::cout << "\n\nStart hit_vector is at (" << start_point.w << ", " << start_point.t << ")" << std::endl; 
      // std::cout << "Angle is " << shower.angle_2d << std::endl;
      // std::cout << "chi2 is " << chi2 / n_hits << "\n";

      // // Found a shower that might be reconstructable!
      // // Compare its start time to the start and end time of the other cluster:
      // std::cout << "Other start point is " << other.start_point.w << ", "
      //           << other.start_point.t << std::endl;
      // std::cout << "Other angle is " << other.angle_2d << std::endl;
      return 1.0;
    }

    // for (float i = 0; i < 10; ++i)
    // {
    //   std::cout << "Point is (" << start_point.w + (i/step)*displacement.w << ", "
    //             << start_point.t + (i/step)*displacement.t << ")" << std::endl;
    // }

    return -1;
   }

  
    
  //------------------------------
  void CFAlgoMatchStart::Report()
  //------------------------------
  {
  }

  std::vector<Hit2D> CFAlgoMatchStart::GetClosestHits(const cluster::cluster_params & params,
                                                                Point2D start, 
                                                                int n_hits, bool forward_only){
    auto all_hits = params.hit_vector;

    std::vector<Hit2D> close_hits;
    close_hits.reserve(n_hits);
    std::vector<float> distances;
    distances.reserve(n_hits);


    for (auto & hit : all_hits){
      if (forward_only){
        if (hit.w < start.w) continue;
      }
      float dist = pow((hit.w - start.w),2) + pow((hit.w - start.w),2);
      // Loop over the distance list, if the distance is less than 
      // a point replace it and slide everything back
      if (dist > 30) continue; // don't copy if really far away
      if (distances.size() == 0){
        distances.push_back(dist);
        close_hits.push_back(hit);
        continue;
      }
      for (auto _i_dist = 0; _i_dist < distances.size(); _i_dist ++){
        if (dist < distances[_i_dist]){
          // std::cout << dist << " is less than " << distances[_i_dist] << "\n";
          distances.insert(distances.begin() + _i_dist,dist);
          close_hits.insert(close_hits.begin() + _i_dist,hit);
          if (distances.size() > n_hits){
            distances.pop_back();
            close_hits.pop_back();
          }
          break;
        }
      }
      // If this point is reached, this distance was bigger than all the others
      // push it back but only if the list is less than n_hits long
      if (distances.size() < n_hits){
        distances.push_back(dist);
        close_hits.push_back(hit);
      }

      // std::cout << "\nhitlist is:\n";
      // for (int i = 0; i < n_hits; ++i)
      // {
      //   if (i >= close_hits.size()) break;
      //   std::cout << "(" << close_hits[i].w << ", " << close_hits[i].t << "): " << distances[i] << "\n";
      // }
    }

    return close_hits;
  }


}
#endif
