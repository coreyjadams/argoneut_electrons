#ifndef TRACKSHOWER_CXX
#define TRACKSHOWER_CXX

#include "TrackShower.h"

namespace argo {

  bool TrackShower::isTrack(const ::cluster::ClusterParamsAlg & cluster){
    return false;
  }

  bool TrackShower::isShower(const ::cluster::ClusterParamsAlg & cluster){
    return false;
  }

  bool TrackShower::TrackOrShower(const ::cluster::ClusterParamsAlg & cluster){
    return false;
  }

} // argo


#endif
