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
class ShowerCalo {


public:

  /// Default constructor
  ShowerCalo() {}

  /// Default destructor
  ~ShowerCalo() {}

  double dEdx_median(int plane);
  double dEdx_LMA(int plane);
  double dEdx_modmean(int plane);
  double dEdx_meta(int plane);
  double dEdx_meta_err(int plane);

  int n_hits(int plane);

  int n_good_hits(int plane);

  double vertex_error();

  double slope_error();

public:

  const double dedx_dist_max = 3.5;

  bool _is_mc;

  int run;
  int subrun;
  int event;

  std::vector<Hit2D> _collection_hits;
  std::vector<Hit2D> _induction_hits;

  std::vector<double> _collection_dedx;
  std::vector<double> _induction_dedx;

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

  std::vector<double> dEdx_meta(int plane);
  std::vector<double> dEdx_meta_err(int plane);
  std::vector<double> n_hits(int plane);

  std::vector<double> vertex_error();
  std::vector<double> slope_error();

};

#endif
/** @} */ // end of doxygen group

