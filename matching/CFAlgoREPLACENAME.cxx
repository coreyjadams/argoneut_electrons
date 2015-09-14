#ifndef RECOTOOL_CFALGOREPLACENAME_CXX
#define RECOTOOL_CFALGOREPLACENAME_CXX

#include "CFAlgoREPLACENAME.h"

namespace argomatch {

  //-------------------------------------------------------
  CFAlgoREPLACENAME::CFAlgoREPLACENAME() : CFloatAlgoBase()
  //-------------------------------------------------------
  {
    SetRatioCut(0.001) ; //(0.095) ;  
    SetStartTimeCut(10) ;
    SetDebug(false) ;
    SetVerbose(false) ;
    RequireThreePlanes(true) ;
  }

  //-----------------------------
  void CFAlgoREPLACENAME::Reset()
  //-----------------------------
  {

  }

  //----------------------------------------------------------------------------------------------
  float CFAlgoREPLACENAME::Float(const std::vector<const cluster::cluster_params*> &clusters)
  //----------------------------------------------------------------------------------------------
  {
    
    // Code-block by Kazu starts
    // This ensures the algorithm works only if # clusters is > 2 (and not =2)
    // You may take out this block if you want to allow matching using clusters from only 2 planes.
    if(_require_3planes && clusters.size()==2) return -1;
    // Code-block by Kazu ends

    double ratio               = 1;
    double time_difference     = 0;
    double charge_ratio        = 1;	
    double _time_ratio_cut = 1; 

    //Preserve location in time space. Cut clusters that have similar time differences,
    // but hit wires at very different times. 
    double start_t      = 0;
    double end_t        = 0;
    double prev_start_t = 0;
    double prev_end_t   = 0;

    double max_hits_1 = 0;
    double max_hits_2 = 0;
    
    for(auto const& c : clusters){
      
      auto charge = c->sum_charge ;		
      
      time_difference  = c->start_point.t - c->end_point.t ; 
      
      max_hits_1 = c->N_Hits;
      max_hits_2 = c->N_Hits;
      
    }

      
//      if( _debug && c_index==(clusters.size()-1) && ratio > _time_ratio_cut ){
//	std::cout<<"\nPLANE: "<<c->Plane() ;
//	std::cout<<"\nStart point: "<<start_t<<std::endl;
//	std::cout<<"End Point: "<<end_t  <<std::endl;
//	//	std::cout<<"Previous start time: "<<prev_start_t<<std::endl;
//	std::cout<<"Time diff: "<<time_difference<<std::endl;
//	std::cout<<"Max time diff: "<<max_time_difference<<std::endl;
//	std::cout<<"Ratio for each cluster: "<<ratio<<std::endl;
//	//				std::cout<<"Charge: "<<charge<<std::endl;
//	std::cout<<"Charge Ratio: "<<charge_ratio<<std::endl;
//	//std::cout<<"Hits are: "<<min_hits<<", "<<middle_hits<<", "<<max_hits<<std::endl;
//	//				std::cout<<"Adjusted Charge Ratio: "<<adjusted_charge_ratio<<std::endl;
//	std::cout<<"Length and Width: "<<c->length<<", "<<c->width<<std::endl;
//      } 
      
    
    if(_verbose && ratio> _time_ratio_cut)
      std::cout<<"**************************FOUND A MATCH . ratio is: "<<ratio<<"\n\n\n"<<std::endl;
    
    return (ratio > _time_ratio_cut ? ratio : -1 ); 
    
   }

  
    
  //------------------------------
  void CFAlgoREPLACENAME::Report()
  //------------------------------
  {
  }
    
}
#endif
