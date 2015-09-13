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
#include "ClusterRecoUtil/Base/ClusterParams.h"
#include "FANN/FANNModule.h"

/**
   \class TrackShower
   User defined class TrackShower ... these comments are used to generate
   doxygen documentation!
 */

namespace argoutils{
  class TrackShower{


  public:

    enum Topology
    {
      kTrack, kShower, kUnknown
    };


    /// Default constructor
    TrackShower();

    /// Default destructor
    ~TrackShower(){}

    Topology trackOrShower(const ::cluster::cluster_params & cluster);

    bool isTrack(const ::cluster::cluster_params & cluster);
    bool isShower(const ::cluster::cluster_params & cluster);

    void SetDebug(bool b){_debug = b;}

    void setFannFileName(std::string s){_fann_file_name = s;}
    void init();

  private:

    bool _debug;

    cluster::FANNModule _fann;
    std::string _fann_file_name;

    // Parameters that might define trackness:
    unsigned int   _min_hits;
    // float _min_multihit_wires;
    float _min_principal;
    float _min_length;

    // Parameter that might define showerness:
    int _shower_nhits;
    float _shower_prin_eig;
    float _shower_secd_eig;
    float _shower_wid_len_rat;
    float _shower_min_mul_hit_w;
    float _shower_min_h_c_h;
    // These are the parameters from the FANN vector:
    // Opening Angle (normalized)  ... : fParams.opening_angle / PI;
    // Opening Angle charge weight  .. : fParams.opening_angle_charge_wgt / PI;
    // Closing Angle (normalized)  ... : fParams.closing_angle / PI;
    // Closing Angle charge weight  .. : fParams.closing_angle_charge_wgt / PI;
    // Principal Eigenvalue  ......... : fParams.eigenvalue_principal;
    // Secondary Eigenvalue  ......... : fParams.eigenvalue_secondary;
    // Width / Length  ............... : fParams.width / fParams.length;
    // Hit Density / M.H.D.  ......... : fParams.hit_density_1D / fParams.modified_hit_density;
    // Percent MultiHit Wires  ....... : fParams.multi_hit_wires/fParams.N_Wires;
    // Percent High Charge Hits  ..... : fParams.N_Hits_HC / fParams.N_Hits;
    // Modified Hit Density  ......... : fParams.modified_hit_density;
    // Charge RMS / Mean Charge ...... : fParams.RMS_charge / fParams.mean_charge;
    // log(Sum Charge / Length) ...... : log(fParams.sum_charge / fParams.length);

    // Get them from cpan
    // They are known to be useful in T/S separation

  };
}

#endif
/** @} */ // end of doxygen group 

