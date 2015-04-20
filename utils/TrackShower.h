/**
 * \file TrackShower.h
 *
 * \ingroup utils
 * 
 * \brief Class def header for a class TrackShower
 *
 * @author cadams
 */

/** \addtogroup utils

    @{*/
#ifndef TRACKSHOWER_H
#define TRACKSHOWER_H

#include <iostream>
#include "LArUtil/GeometryUtilities.h"
#include "ClusterRecoUtil/ClusterParamsAlg.h"

/**
   \class TrackShower
   User defined class TrackShower ... these comments are used to generate
   doxygen documentation!
 */

namespace argo{
  class TrackShower{


  public:

    enum Topology
    {
      kTrack, kShower, kUnknown
    };


    /// Default constructor
    TrackShower(){}

    /// Default destructor
    ~TrackShower(){}

    Topology trackOrShower(const ::cluster::ClusterParamsAlg & cluster);

    bool isTrack(const ::cluster::ClusterParamsAlg & cluster);
    bool isShower(const ::cluster::ClusterParamsAlg & cluster);

  private:

    // Parameters that might define trackness:

    // Parameter that might define showerness:

  };
}

#endif
/** @} */ // end of doxygen group 

