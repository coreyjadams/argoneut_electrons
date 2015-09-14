#ifndef RECOTOOL_CFALGOSHOWERTIMEMATCH_CXX
#define RECOTOOL_CFALGOSHOWERTIMEMATCH_CXX

#include "CFAlgoShowerTimeMatch.h"

namespace argomatch {

  //-------------------------------------------------------
  CFAlgoShowerTimeMatch::CFAlgoShowerTimeMatch() : CFloatAlgoBase()
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
  void CFAlgoShowerTimeMatch::Reset()
  //-----------------------------
  {

  }

  //----------------------------------------------------------------------------------------------
  float CFAlgoShowerTimeMatch::Float(const std::vector<const cluster::cluster_params*> &clusters)
  //----------------------------------------------------------------------------------------------
  {
    
    // Code-block by Kazu starts
    // This ensures the algorithm works only if # clusters is > 2 (and not =2)
    // You may take out this block if you want to allow matching using clusters from only 2 planes.
    if(_require_3planes && clusters.size()==2) return -1;
    // Code-block by Kazu ends

    // std::cout << "This pair has " << clusters.size() << " clusters.\n";


    
    // First, find the showers on the collection plane:
    cluster::cluster_params const *  shower;
    int nshowers = 0;
    // Look for showers on the collection plane:
    for(auto const& c : clusters){
      if (ts.trackOrShower(*c) == argoutils::TrackShower::kShower && c -> plane_id.Plane == 1){
        // std::cout << "found a collection shower!\n";
        shower = c;
        nshowers++;
      }
    }

    if (nshowers != 1) return -1.0;

    auto showerRange = getTimeRange(shower);
    std::vector<float> otherRange;

    for(auto const& c : clusters){
      if (c == shower) continue;
      else
        otherRange = getTimeRange(c);
    }

    float score = getScore(showerRange,otherRange);
    // std::cout << "Score is " << score << std::endl;
    return score;

      
    
    // if(_verbose && ratio> _time_ratio_cut)
      // std::cout<<"**************************FOUND A MATCH . ratio is: "<<ratio<<"\n\n\n"<<std::endl;
    
    // return (ratio > _time_ratio_cut ? ratio : -1 ); 
    
   }

  std::vector<float> CFAlgoShowerTimeMatch::getTimeRange(
    const cluster::cluster_params* cluster)
  {
    std::vector<float> range;
    range.resize(2);
    auto hits = cluster->hit_vector;
    range.front() = hits.front().t;
    range.back()  = hits.front().t;
    for (auto & hit : hits){
      if (hit.t  > range.front() )
        range.front() = hit.t;
      if (hit.t  < range.back() )
        range.back() = hit.t;
    }
    return range;
  }

  float CFAlgoShowerTimeMatch::getScore( std::vector<float> showerRange, 
                  std::vector<float> otherRange)
  {
    // Find out how much these two intervals overlap, and normalize 
    // it to the shower range
    float overlap = -1.0;
    // std::cout << "Ranges are: shower - (" << showerRange.front()
    //           << " -> " << showerRange.back() << "), other - ("
    //           << otherRange.front() 
    //           << " -> " << otherRange.back() << ").\n";

    if (otherRange.front()  - showerRange.back() >= 0 && 
        showerRange.front() - otherRange.back()  >= 0 ) 
    { // overlap
      overlap = fmax(showerRange.front(), otherRange.front())
              - fmax(showerRange.back(), otherRange.back());
    }
    return overlap / (showerRange.front() - showerRange.back());
  }

  //------------------------------
  void CFAlgoShowerTimeMatch::Report()
  //------------------------------
  {
  }
    
}
#endif
