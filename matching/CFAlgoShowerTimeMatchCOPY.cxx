#ifndef RECOTOOL_CFALGOSHOWERTIMEMATCHCOPY_CXX
#define RECOTOOL_CFALGOSHOWERTIMEMATCHCOPY_CXX

#include "CFAlgoShowerTimeMatchCOPY.h"

namespace cmtool {

  //-------------------------------------------------------
  CFAlgoShowerTimeMatchCOPY::CFAlgoShowerTimeMatchCOPY() : CFloatAlgoBase()
  //-------------------------------------------------------
  {
    SetRatioCut(0.001) ; //(0.095) ;  
    SetStartTimeCut(10) ;
    SetDebug(false) ;
    SetVerbose(false) ;
    RequireThreePlanes(false) ;
    ts.setFannFileName("/uboone/app/users/npereira/larlite/UserDev/argoneut_electrons/utils/fann_training/trackShowerAnn.dat") ;
    ts.init();
  }

  //-----------------------------
  void CFAlgoShowerTimeMatchCOPY::Reset()
  //-----------------------------
  {

  }

  //----------------------------------------------------------------------------------------------
  float CFAlgoShowerTimeMatchCOPY::Float(const std::vector<const cluster::ClusterParamsAlg*> &clusters)
  //----------------------------------------------------------------------------------------------
  {
    
    // Code-block by Kazu starts
    // This ensures the algorithm works only if # clusters is > 2 (and not =2)
    // You may take out this block if you want to allow matching using clusters from only 2 planes.
    if(_require_3planes && clusters.size()==2) return -1;
    // Code-block by Kazu ends

    // std::cout << "This pair has " << clusters.size() << " clusters.\n";


    
    // First, find the showers on the collection plane:
    cluster::ClusterParamsAlg const *  shower;
    int nshowers = 0;
    // Look for showers on the collection plane:
    for(auto const& c : clusters){
      if (ts.trackOrShower(*c) == argo::TrackShower::kShower && c -> Plane() == 1){
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

  std::vector<float> CFAlgoShowerTimeMatchCOPY::getTimeRange(
    const cluster::ClusterParamsAlg* cluster)
  {
    std::vector<float> range;
    range.resize(2);
    auto hits = cluster->GetHitVector();
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

  float CFAlgoShowerTimeMatchCOPY::getScore( std::vector<float> showerRange, 
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
      // std::cout << "There is an overlap in TimeMatchCOPY" << "\n";
    }
    return overlap / (showerRange.front() - showerRange.back());
  }

  //------------------------------
  void CFAlgoShowerTimeMatchCOPY::Report()
  //------------------------------
  {
  }
    
}
#endif