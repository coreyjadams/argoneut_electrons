#ifndef TRACKSHOWER_CXX
#define TRACKSHOWER_CXX

#include "TrackShower.h"

namespace argoutils {

TrackShower::TrackShower() {
  _shower_prin_eig = 0.995;
  _shower_secd_eig = 0.1;
  _shower_wid_len_rat = 0.05;
  _shower_min_mul_hit_w = 0.2;
  _shower_min_h_c_h = 0.3;
  _shower_nhits = 50;

  // Track parameters
  _min_hits = 15.0;
  _min_principal = 0.993;
  _min_length = 12.0;

  // default filename:
  _fann_file_name =
    "/home/cadams/larlite/UserDev/argoneut_electrons/utils/fann_training/"
    "trackShowerAnn.dat";

  SetDebug(false);
}

void TrackShower::init() {
  _fann.setFANNFileName(_fann_file_name);
  _fann.LoadFromFile();
}

bool TrackShower::isTrack(const ::cluster::cluster_params& cluster) {
  size_t N_Hits = cluster.hit_vector.size();
  auto start_point = cluster.start_point;
  double length = cluster.length;
  double plane = cluster.start_point.plane;
  double start_x = cluster.start_point.w;
  double start_y = cluster.start_point.t;
  double end_x = cluster.end_point.w;
  double end_y = cluster.end_point.t;
  double ep = cluster.eigenvalue_principal;

  if (_debug) {
    std::cout << "-------------------------------------------------------------"
              "-----------------" << std::endl;
    std::cout << "cluster plane: " << plane << "  cluster start: (" << start_x
              << "," << start_y << ")"
              << " cluster end: (" << end_x << "," << end_y << ")" << std::endl;
    std::cout << "N_Hits: " << N_Hits << "  min_hits: " << _min_hits
              << std::endl;
    std::cout << "EP: " << ep << "  min_ep: " << _min_principal << std::endl;
    std::cout << "length: " << length << "  min_length: " << _min_length
              << std::endl;
  }

  if (((N_Hits > _min_hits) && (length > _min_length) &&
       (ep > _min_principal)) ||
      (ep > (_min_principal + 0.006))) {
    if (_debug) std::cout << "This is a track!" << std::endl;

    return true;
  }

  if (_debug) std::cout << "This is not a track" << std::endl;

  return false;
}

bool TrackShower::isShower(const ::cluster::cluster_params& cluster) {
  // These are the parameters from the FANN vector:
  // Opening Angle (normalized)  ... : fParams.opening_angle / PI;
  // Opening Angle charge weight  .. : fParams.opening_angle_charge_wgt / PI;
  // Closing Angle (normalized)  ... : fParams.closing_angle / PI;
  // Closing Angle charge weight  .. : fParams.closing_angle_charge_wgt / PI;
  // Principal Eigenvalue  ......... : fParams.eigenvalue_principal;
  // Secondary Eigenvalue  ......... : fParams.eigenvalue_secondary;
  // Width / Length  ............... : fParams.width / fParams.length;
  // Hit Density / M.H.D.  ......... : fParams.hit_density_1D /
  // fParams.modified_hit_density;
  // Percent MultiHit Wires  ....... : fParams.multi_hit_wires/fParams.N_Wires;
  // Percent High Charge Hits  ..... : fParams.N_Hits_HC / fParams.N_Hits;
  // Modified Hit Density  ......... : fParams.modified_hit_density;
  // Charge RMS / Mean Charge ...... : fParams.RMS_charge / fParams.mean_charge;
  // log(Sum Charge / Length) ...... : log(fParams.sum_charge / fParams.length);

  // Get them from cpan

  std::vector<float> data;
  cluster.GetFANNVector(data);

  // Do this identification stupidly for the moment.

  // Require a minimum number of hits
  if (cluster.N_Hits < _shower_nhits) return false;

  // if (_debug && cluster.Plane() == 1){
  //   std::cout << "(" << cluster.start_point.w << ", "
  //             << cluster.start_point.t << ") -> ("
  //             << cluster.end_point.w << ", "
  //             << cluster.end_point.t << "), Plane "
  //             << cluster.Plane() << "\n"
  //             << "\tPrincipal Eigenvalue  ....." << data[4]
  //             << "\t( < " << _shower_prin_eig << ")\n"
  //             // << "\tSecondary Eigenvalue  ....." << data[5]
  //             // << "\t( > " << _shower_secd_eig << ")\n"
  //             << "\tWidth / Length  ..........." << data[6]
  //             << "\t( > " << _shower_wid_len_rat << ")\n"
  //             << "\tPercent MultiHit Wires  ..." << data[8]
  //             << "\t( > " << _shower_min_mul_hit_w << ")\n"
  //             << "\tPercent High Charge Hits  ." << data[9]
  //             << "\t( > " << _shower_min_h_c_h << ")\n";
  // }

  // Cut on showerness variables:
  if (data[4] < _shower_prin_eig &&
      // data[5] > _shower_secd_eig &&
      data[6] > _shower_wid_len_rat && data[8] > _shower_min_mul_hit_w &&
      data[9] > _shower_min_h_c_h) {
    return true;
  }

  return false;
}

TrackShower::Topology TrackShower::trackOrShower(
    const ::cluster::cluster_params& cluster) {
  if (cluster.hit_vector.size() < 40) return kUnknown;

  // get the data used to make the separation:
  std::vector<float> input_data;
  cluster.GetFANNVector(input_data);

  // run the separation network:
  std::vector<float> output_data;
  _fann.run(input_data, output_data);

  // Make the classification:
  float _classification = output_data[0] - output_data[1];

  if (_classification > 0.9) {
    return kShower;
  } else if (_classification < -0.9) {
    return kTrack;
  }

  return kUnknown;
}

} // argo


#endif
