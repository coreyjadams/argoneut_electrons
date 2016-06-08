/**
 * \file ShowerCalo.h
 *
 * \ingroup analysis
 *
 * \brief Class def header for a class ShowerCalo
 *
 * @author cadams
 */

/** \addtogroup analysis

    @{*/
#ifndef SHOWERCALO_H
#define SHOWERCALO_H

#include <iostream>

#include "LArUtil/GeometryHelper.h"


#include "TVector3.h"
#include "LArUtil/PxUtils.h"
typedef larutil::PxHit Hit2D;
typedef larutil::PxPoint Point2D;

/**
   \class ShowerCalo
   User defined class ShowerCalo ... these comments are used to generate
   doxygen documentation!
 */

struct sort_first {
  bool operator()(const std::pair<int, int> &left, const std::pair<int, int> &right) {
    return left.first < right.first;
  }
};

struct sort_second {
  bool operator()(const std::pair<int, int> &left, const std::pair<int, int> &right) {
    return left.second < right.second;
  }
};


class ShowerCalo {


public:

  /// Default constructor
  ShowerCalo() {_is_mc = false;}

  /// Default destructor
  ~ShowerCalo() {}
  double dEdx_mean(int plane);
  double dEdx_median(int plane);
  double dEdx_best_median();
  double dEdx_LMA(int plane);
  double dEdx_best_LMA();
  double dEdx_modmean(int plane);
  double dEdx_best_modmean();
  double dEdx_meta(int plane);
  double dEdx_best_meta();
  double dEdx_meta_err(int plane);

  double joint_dEdx();

  // Picks the best plane, and reports that dEdx.
  // Biased to collection
  double best_dedx();

  // Actually determines which plane to use
  int best_plane();

  int n_hits(int plane);

  int n_good_hits(int plane);

  double vertex_error();

  double slope_error();

  void set_drop_first_hit(bool b){_drop_first_hit = b;}

  std::vector<double> good_dedx_hits(int plane, bool box=false);

  std::vector<double> best_dedx_hits(bool box=false);

  int run(){return _run;}
  int subrun(){return _subrun;}
  int event(){return _event;}
  int fileIndex(){return _file_index;}

  double distance(int plane);
  double pitch(int plane);

  double mc_3D_vertex_resolution(); // returned in cm
  double mc_3D_angle_resolution();  // return in degrees

  double mc_x_resolution();
  double mc_y_resolution();
  double mc_z_resolution();

  double mc_true_energy();
  double mc_deposited_energy();

public:


  const double dedx_dist_max = 4;

  bool _drop_first_hit;

  bool _is_mc;

  // Some mc variables, only meaningful when _is_mc == true
  TVector3 _true_start_point;
  TVector3 _true_direction;

  double _true_energy;
  double _true_deposited_energy;

  int _run;
  int _subrun;
  int _event;
  int _file_index;

  double _collection_dist;
  double _induction_dist;

  int _best_plane;

  std::vector<Hit2D> _collection_hits;
  std::vector<Hit2D> _induction_hits;

  std::vector<double> _collection_dedx;
  std::vector<double> _induction_dedx;

  std::vector<double> _collection_dedx_box;
  std::vector<double> _induction_dedx_box;

  std::vector<double> _collection_dqdx;
  std::vector<double> _induction_dqdx;

  std::vector<double> _collection_dist_3D;
  std::vector<double> _induction_dist_3D;

  Point2D _collection_start;
  Point2D _induction_start;

  double _collection_pitch;
  double _induction_pitch;

  double _collection_slope;
  double _induction_slope;

  double _collection_projection_error;
  double _induction_projection_error;

  TVector3 _3D_start_point;
  TVector3 _3D_direction;

  TVector3 _other_start_point;


};


class shower_collection : public std::vector<ShowerCalo> {

public:
  std::vector<double> dEdx_median(int plane);
  std::vector<double> dEdx_modmean(int plane);
  std::vector<double> dEdx_LMA(int plane);

  std::vector<double> dEdx_best_median();
  std::vector<double> dEdx_best_modmean();
  std::vector<double> dEdx_best_LMA();

  std::vector<double> dEdx_meta(int plane);
  std::vector<double> dEdx_meta_err(int plane);
  std::vector<double> n_hits(int plane);
  std::vector<double> n_good_hits(int plane);

  std::vector<double> best_dedx();
  std::vector<double> joint_dedx();

  std::vector<double> vertex_error();
  std::vector<double> slope_error();

  std::vector<double> distance(int plane);
  std::vector<double> pitch(int plane);

  std::vector<double> all_dedx_hits(int plane);
  std::vector<double> all_dedx_hits_box(int plane);
  std::vector<double> best_dedx_hits_box();


  std::vector<double> mc_x_resolution();
  std::vector<double> mc_y_resolution();
  std::vector<double> mc_z_resolution();

  std::vector<double> mc_3D_vertex_resolution(); // returned in cm
  std::vector<double> mc_3D_angle_resolution();  // return in degrees
  std::vector<double> mc_true_energy();
  std::vector<double> mc_deposited_energy();


  void set_drop_first_hit(bool b);

};

#endif
/** @} */ // end of doxygen group

