#ifndef LARLITE_FILTERMINOSTRACKS_CXX
#define LARLITE_FILTERMINOSTRACKS_CXX

#include "FilterMinosTracks.h"
#include "DataFormat/minos.h"
#include "DataFormat/track.h"

namespace larlite {

  bool FilterMinosTracks::initialize() {

	SetBoundary(2.) ;

    return true;
  }
  
  bool FilterMinosTracks::analyze(storage_manager* storage) {

  auto evt_minos = storage->get_data<larlite::event_minos>("minos");
  auto evt_track = storage->get_data<larlite::event_track>("ct");

  if(evt_minos->size() == 0 || evt_track->size() ==0 )
	return -1; 

	
  auto ass_info = evt_minos->association(evt_track->id()); 
  //evt_minos->list_association() ;

  for(auto const & track_indices : ass_info ) { 
//	  std::cout<<"track indices size: "<<track_indices.size()<<std::endl;
		for(size_t track_index=0; track_index < track_indices.size(); track_index++){
//		  	std::cout<<"track index: "<<track_index<<std::endl;
			auto argo_track = evt_track->at(track_index) ;

	        std::vector<double> larStart(3,0), larEnd(3,0);
    	    larStart[0] = argo_track.vertex_at(0)[0];
    	    larStart[1] = argo_track.vertex_at(0)[1];
        	larStart[2] = argo_track.vertex_at(0)[2];

  
   			 if(fabs(larStart[0])<fdBoundary || fabs(47.-larStart[0])<fdBoundary || fabs(larStart[1]+20.)<fdBoundary
       		 || fabs(20.-larStart[1])<fdBoundary || fabs(larStart[2])<fdBoundary || fabs(90-larStart[2])<fdBoundary){
				std::cout<<"Starting on a boundary..."<<std::endl;
    			return true;
      			}   

  			} 

    }   


	std::cout<<"Not starting on a boundary...get rid of it"<<std::endl;
    return false;
  }

  bool FilterMinosTracks::finalize() {

    return true;
  }

}
#endif
