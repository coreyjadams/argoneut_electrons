#ifndef RECOTOOL_CFALGOMATCHSTART_CXX
#define RECOTOOL_CFALGOMATCHSTART_CXX

#include "CFAlgoMatchStart.h"

namespace cmtool {

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
  float CFAlgoMatchStart::Float(const std::vector<const cluster::ClusterParamsAlg*> &clusters)
  //----------------------------------------------------------------------------------------------
  {
    
    // Code-block by Kazu starts
    // This ensures the algorithm works only if # clusters is > 2 (and not =2)
    // You may take out this block if you want to allow matching using clusters from only 2 planes.
    if(_require_3planes && clusters.size()==2) return -1;
    // Code-block by Kazu ends

    // First, find the showers on the collection plane:
    cluster::ClusterParamsAlg shower;
    int nshowers = 0;
    // Look for showers on the collection plane:
    for(auto const& c : clusters){
      if (ts.trackOrShower(*c) == argo::TrackShower::kShower && c -> Plane() == 1){
        // std::cout << "found a collection shower!\n";
        shower = *c;
        nshowers++;
      }
    }

    shower.RefineStartPointAndDirection(true);

    // print out the start point of the shower:
    auto start_point = shower.GetParams().start_point;

    if (start_point.w != start_point.w){
      return -1;
    }

    std::cout << "Start point is at (" << start_point.w << ", " << start_point.t << ")" << std::endl; 
    

    // Find the closest hits to the start point
    std::vector<larutil::PxHit> close_hits = GetClosestHits(shower, start_point, 10, false);
    return -1;
   }

  
    
  //------------------------------
  void CFAlgoMatchStart::Report()
  //------------------------------
  {
  }

  std::vector<larutil::PxHit> CFAlgoMatchStart::GetClosestHits(const cluster::ClusterParamsAlg & params,
                                                                larutil::PxPoint start, 
                                                                int n_hits, bool forward_only){
    auto all_hits = params.GetHitVector();

    std::vector<larutil::PxHit> close_hits;
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
      if (distances.size() < n_hits){
        distances.push_back(dist);
        close_hits.push_back(hit);
      }
      for (auto distance : distances){
        if (dist < distance){
          std::cout << dist << " is less than " << distance << "\n";
          distances.insert(distances.begin(),dist);
          close_hits.insert(close_hits.begin(),hit);
          if (distances.size() > n_hits){
            distances.pop_back();
            close_hits.pop_back();
          }
          break;
        }
      }
      std::cout << "\nhitlist is:\n";
      for (int i = 0; i < n_hits; ++i)
      {
        if (i > close_hits.size()) break;
        std::cout << "(" << close_hits[i].w << ", " << close_hits[i].t << "): " << distances[i] << "\n";
      }
    }

    return close_hits;
  }


}
#endif
