#ifndef SHOWERCALO_CXX
#define SHOWERCALO_CXX

#include <math.h>
#include "ShowerCalo.h"

double ShowerCalo::dEdx_median(int plane) {
  // Loop over the dE/dx points in the specified plane and compute the dE/dx using the median method

  std::vector<double> cands;
  if (plane == 0) {
    for (size_t index = 0; index < _induction_dedx.size(); index ++) {
      if (_induction_dist_3D.at(index) > 0 && _induction_dist_3D.at(index) < dedx_dist_max) {
        cands.push_back(_induction_dedx.at(index));
      }
    }
  }
  else if (plane == 1) {
    for (size_t index = 0; index < _collection_dedx.size(); index ++) {
      if (_collection_dist_3D.at(index) > 0 && _collection_dist_3D.at(index) < dedx_dist_max) {
        cands.push_back(_collection_dedx.at(index));
      }
    }
  }

  if (cands.size() < 3) {
    return 0;
  }

  if (_drop_first_hit) {
    cands.erase(cands.begin());
  }



  std::sort(cands.begin(), cands.end());
  if (cands.size() % 2 == 0) {
    return 0.5 * (cands.at(cands.size() / 2) + cands.at(cands.size() / 2  - 1));
  }
  else {
    return cands.at(cands.size() / 2);
  }

}

double ShowerCalo::dEdx_modmean(int plane) {
  // Loop over the dE/dx points in the specified plane and compute the dE/dx using the median method

  std::vector<double> * cands = 0;
  std::vector<double> * dists = 0;
  if (plane == 0) {
    cands = & _induction_dedx;
    dists = & _induction_dist_3D;
  }
  else if (plane == 1) {
    cands = & _collection_dedx;
    dists = & _collection_dist_3D;
  }

  std::vector<double> points_within_dist;
  for (size_t i = 0; i < cands->size(); i++) {
    if (dists->at(i) > 0 && dists->at(i) < dedx_dist_max) {
      points_within_dist.push_back(cands->at(i));
    }
  }

  if (points_within_dist.size() == 0) {
    return 0.0;
  }

  if (_drop_first_hit) {
    points_within_dist.erase(points_within_dist.begin());
  }

  // Get the mean:
  double sum = std::accumulate(points_within_dist.begin(),
                               points_within_dist.end(),
                               0.0);

  double mean = sum / points_within_dist.size();

  // Now get the rms:
  double rms = 0.0;
  for (auto & val : points_within_dist) {
    rms += val * val;
  }
  rms /= points_within_dist.size();
  rms = sqrt(rms);

  if (rms == 0) {
    return mean;
  }

  // Now actually get the modified mean:
  double modmean = 0.0;
  int n = 0;
  for (auto & val : points_within_dist) {
    if ( fabs(val - mean) < 2 * rms && val < 10) {
      modmean += val;
      n++;
    }
  }

  if (n == 0) {
    return 0.0;
  }
  else {
    return modmean / n;
  }

}


double ShowerCalo::dEdx_mean(int plane) {
  // Loop over the dE/dx points in the specified plane and compute the dE/dx using the median method

  std::vector<double> * cands = 0;
  std::vector<double> * dists = 0;
  if (plane == 0) {
    cands = & _induction_dedx;
    dists = & _induction_dist_3D;
  }
  else if (plane == 1) {
    cands = & _collection_dedx;
    dists = & _collection_dist_3D;
  }

  std::vector<double> points_within_dist;
  for (size_t i = 0; i < cands->size(); i++) {
    if (dists->at(i) > 0 && dists->at(i) < dedx_dist_max) {
      points_within_dist.push_back(cands->at(i));
    }
  }

  if (points_within_dist.size() == 0) {
    return 0.0;
  }

  if (_drop_first_hit) {
    points_within_dist.erase(points_within_dist.begin());
  }

  // Get the mean:
  double sum = std::accumulate(points_within_dist.begin(),
                               points_within_dist.end(),
                               0.0);

  if (points_within_dist.size() != 0) {
    return sum / points_within_dist.size();
  }

  return 0;

}

double ShowerCalo::dEdx_LMA(int plane) {
  std::vector<double> * cands;
  if (plane == 0) {
    cands = & _induction_dedx;
  }
  else if (plane == 1) {
    cands = & _collection_dedx;
  }

  // Loop through the events and calculate the moving average of 3 poitns
  // at a time.  Keep the lowest value of this.


  const int n = 3;

  if (cands->size() <= n) {
    return 0.0;
  }


  double lma = 9999.0;

  for (size_t i = 0; i < cands->size() - n; i ++) {
    double _this_lma = 0.0;
    for (size_t _n = 0; _n < n; _n ++) {
      _this_lma += cands->at(i + _n);
    }
    _this_lma *= 1.0 / n;
    if (_this_lma < lma) {
      lma = _this_lma;
    }
  }

  return lma;

}



double ShowerCalo::joint_dEdx() {

  // Loop over the dE/dx points in the specified plane and compute the dE/dx using the median method

  std::vector < std::pair<double, double> > cands;
  for (size_t index = 0; index < _induction_dedx.size(); index ++) {
    if (_induction_dist_3D.at(index) > 0 && _induction_dist_3D.at(index) < dedx_dist_max) {
      cands.push_back(
        std::make_pair(_induction_dedx.at(index),
                       _induction_dist_3D.at(index)) );
    }
  }

  for (size_t index = 0; index < _collection_dedx.size(); index ++) {
    if (_collection_dist_3D.at(index) > 0 && _collection_dist_3D.at(index) < dedx_dist_max) {
      cands.push_back(
        std::make_pair(_collection_dedx.at(index),
                       _collection_dist_3D.at(index)) );
    }
  }

  if (cands.size() < 3) {
    return 0;
  }

  // Sort the distances to be in order:
  if (_drop_first_hit) {
    cands.erase(cands.begin());
    std::sort(cands.begin(), cands.end(), sort_second());
  }

  // for (auto & pair : cands) {
  //   std::cout << "    Dist : " << pair.second <<
  // }


  // Sort by dE/dx:
  std::sort(cands.begin(), cands.end(), sort_first());




  if (cands.size() % 2 == 0) {
    return 0.5 * (cands.at(cands.size() / 2).first + cands.at(cands.size() / 2  - 1).first);
  }
  else {
    return cands.at(cands.size() / 2).first;
  }


}

double ShowerCalo::dEdx_best_median() {
  return dEdx_median(best_plane());
}

double ShowerCalo::dEdx_best_LMA() {
  return dEdx_LMA(best_plane());
}

double ShowerCalo::dEdx_best_modmean() {
  return dEdx_modmean(best_plane());
}

double ShowerCalo::dEdx_best_meta() {
  return dEdx_meta(best_plane());
}


double ShowerCalo::distance(int plane) {

  if (plane == 1) {
    return _collection_dist;
  }
  else if (plane == 0) {
    return _induction_dist;
  }
  else return 0;
}


double ShowerCalo::pitch(int plane) {

  if (plane == 1) {
    return _collection_pitch;
  }
  else if (plane == 0) {
    return _induction_pitch;
  }
  else return 0;
}

double ShowerCalo::dEdx_meta(int plane) {
  // Computes the average of the 3 dEdx metrics
  return (dEdx_LMA(plane) + dEdx_modmean(plane) + dEdx_median(plane) ) / 3.0;
}
double ShowerCalo::dEdx_meta_err(int plane) {

  // Get the meta dE/dx, compute RMS of it
  double meta_val = dEdx_meta(plane);

  double rms = 0.0;

  double diff  = 0.0;

  diff = (meta_val - dEdx_modmean(plane));
  rms += diff * diff;
  diff = (meta_val - dEdx_LMA(plane) );
  rms += diff * diff;
  diff = (meta_val - dEdx_median(plane));
  rms += diff * diff;


  rms /= 3.0;

  return sqrt(rms);

}

int ShowerCalo::n_hits(int plane) {
  if (plane == 0) {
    return _induction_dedx.size();
  }
  else if (plane == 1) {
    return _collection_dedx.size();
  }
  else {
    return 0;
  }
}

int ShowerCalo::n_good_hits(int plane) {
  int n_good_hits = 0;
  if (plane == 0) {
    for (size_t index = 0; index < _induction_dedx.size(); index ++) {
      if (_induction_dist_3D.at(index) > 0 && _induction_dist_3D.at(index) < dedx_dist_max) {
        if (!_drop_first_hit || (_drop_first_hit && _induction_dist_3D.at(index) > _induction_pitch) ) {
          n_good_hits ++;
        }
      }
    }
  }
  else if (plane == 1) {
    for (size_t index = 0; index < _collection_dedx.size(); index ++) {
      if (_collection_dist_3D.at(index) > 0 && _collection_dist_3D.at(index) < dedx_dist_max) {
        if (!_drop_first_hit || (_drop_first_hit && _collection_dist_3D.at(index) > _collection_pitch) ) {
          n_good_hits ++;
        }

      }
    }
  }
  else {
    return 0;
  }
  return n_good_hits;
}

double ShowerCalo::vertex_error() {
  return (_other_start_point - _3D_start_point).Mag();

}

double ShowerCalo::slope_error() {
  return _collection_projection_error + _induction_projection_error;
}

double ShowerCalo::mc_true_energy() {
  if (!_is_mc) {
    return -1;
  }
  else {
    return _true_energy;
  }
}

double ShowerCalo::mc_deposited_energy() {
  if (!_is_mc) {
    return -1;
  }
  else {
    return _true_deposited_energy;
  }
}

double ShowerCalo::mc_x_resolution() {
  return _true_start_point.X() - _3D_start_point.X();
}
double ShowerCalo::mc_y_resolution() {
  return _true_start_point.Y() - _3D_start_point.Y();
}
double ShowerCalo::mc_z_resolution() {
  return _true_start_point.Z() - _3D_start_point.Z();
}

double ShowerCalo::mc_3D_vertex_resolution() {
  if (!_is_mc) {
    return -1;
  }
  else {
    return (_true_start_point - _3D_start_point).Mag();
  }
}

double ShowerCalo::mc_3D_angle_resolution() {
  const static double rad_to_deg = (180.0 / 3.14159265);
  if (!_is_mc) {
    return -1;
  }
  else {
    if (_true_direction.Mag() == 0 ||
        _3D_direction.Mag() == 0) {
      return -1;
    }
    double angle = _true_direction.Dot(_3D_direction);
    angle /= (_true_direction.Mag());
    angle /= (_3D_direction.Mag());

    if (angle != angle) {
      std::cout << "True Direction: ("
                << _true_direction.X() << ", "
                << _true_direction.Y() << ", "
                << _true_direction.Z() << ")\n ";
      std::cout << "Reco Direction: ("
                << _3D_direction.X() << ", "
                << _3D_direction.Y() << ", "
                << _3D_direction.Z() << ")\n ";

    }

    return rad_to_deg * acos(angle);
  }
}


std::vector<double>  ShowerCalo::good_dedx_hits(int plane, bool box) {
  std::vector<double> _good_dedx_hits;
  if (plane == 0) {
    for (size_t index = 0; index < _induction_dedx.size(); index ++) {
      if (_induction_dist_3D.at(index) > 0 && _induction_dist_3D.at(index) < dedx_dist_max) {
        if (!_drop_first_hit || (_drop_first_hit && _induction_dist_3D.at(index) > _induction_pitch) ) {
          if (_induction_dedx[index] < 8.0) {
            if (box)
              _good_dedx_hits.push_back(_induction_dedx_box[index]);
            else
              _good_dedx_hits.push_back(_induction_dedx[index]);
          }
        }
      }
    }
  }
  else if (plane == 1) {
    for (size_t index = 0; index < _collection_dedx.size(); index ++) {
      if (_collection_dist_3D.at(index) > 0 && _collection_dist_3D.at(index) < dedx_dist_max) {
        if (!_drop_first_hit || (_drop_first_hit && _collection_dist_3D.at(index) > _collection_pitch) ) {
          if (_collection_dedx[index] < 8.0) {
            if (box)
              _good_dedx_hits.push_back(_collection_dedx[index]);
            else
              _good_dedx_hits.push_back(_collection_dedx[index]);
          }
        }

      }
    }
  }
  return _good_dedx_hits;
}

std::vector<double> ShowerCalo::best_dedx_hits(bool box) {

  return good_dedx_hits(best_plane(), box);

}


double ShowerCalo::best_dedx() {
  // Find out the best plane:
  return dEdx_median(best_plane());

}

int ShowerCalo::best_plane() {

  return _best_plane;
}




std::vector<double> shower_collection::dEdx_median(int plane) {
  std::vector<double> _result;
  _result.reserve(this->size());
  for (auto & item : *this ) {
    _result.push_back(item.dEdx_median(plane));
  }
  return _result;
}
std::vector<double> shower_collection::dEdx_modmean(int plane) {
  std::vector<double> _result;
  _result.reserve(this->size());
  for (auto & item : *this ) {
    _result.push_back(item.dEdx_modmean(plane));
  }
  return _result;
}
std::vector<double> shower_collection::dEdx_LMA(int plane) {
  std::vector<double> _result;
  _result.reserve(this->size());
  for (auto & item : *this ) {
    _result.push_back(item.dEdx_LMA(plane));
  }
  return _result;
}
std::vector<double> shower_collection::dEdx_meta(int plane) {
  std::vector<double> _result;
  _result.reserve(this->size());
  for (auto & item : *this ) {
    _result.push_back(item.dEdx_meta(plane));
  }
  return _result;
}
std::vector<double> shower_collection::best_dedx() {
  std::vector<double> _result;
  _result.reserve(this->size());
  for (auto & item : *this ) {
    _result.push_back(item.best_dedx());
  }
  return _result;
}
std::vector<double> shower_collection::joint_dedx() {
  std::vector<double> _result;
  _result.reserve(this->size());
  for (auto & item : *this ) {
    _result.push_back(item.joint_dEdx());
  }
  return _result;
}

std::vector<double> shower_collection::dEdx_meta_err(int plane) {
  std::vector<double> _result;
  _result.reserve(this->size());
  for (auto & item : *this ) {
    _result.push_back(item.dEdx_meta_err(plane));
  }
  return _result;
}
std::vector<double> shower_collection::n_hits(int plane) {
  std::vector<double> _result;
  _result.reserve(this->size());
  for (auto & item : *this ) {
    _result.push_back(item.n_hits(plane));
  }
  return _result;
}

std::vector<double> shower_collection::n_good_hits(int plane) {
  std::vector<double> _result;
  _result.reserve(this->size());
  for (auto & item : *this ) {
    _result.push_back(item.n_good_hits(plane));
  }
  return _result;
}


std::vector<double> shower_collection::vertex_error() {
  std::vector<double> _result;
  _result.reserve(this->size());
  for (auto & item : *this ) {
    _result.push_back(item.vertex_error());
  }
  return _result;
}
std::vector<double> shower_collection::slope_error() {
  std::vector<double> _result;
  _result.reserve(this->size());
  for (auto & item : *this ) {
    _result.push_back(item.slope_error());
  }
  return _result;
}

void shower_collection::set_drop_first_hit(bool b) {
  for (auto & item : *this ) {
    item.set_drop_first_hit(b);
  }
}

std::vector<double> shower_collection::all_dedx_hits(int plane) {
  std::vector<double> _result;
  _result.reserve(this->size() * 10);
  for (auto & item : *this ) {
    auto hits = item.good_dedx_hits(plane);
    // std::cout << "Got " << hits.size() << " hits." << std::endl;
    _result.insert(_result.end(),
                   hits.begin(),
                   hits.end());
  }
  // std::cout << "Returning " << _result.size() << " hits" << std::endl;
  return _result;
}

std::vector<double> shower_collection::all_dedx_hits_box(int plane) {
  std::vector<double> _result;
  _result.reserve(this->size() * 10);
  for (auto & item : *this ) {
    auto hits = item.good_dedx_hits(plane, true);
    // std::cout << "Got " << hits.size() << " hits." << std::endl;
    _result.insert(_result.end(),
                   hits.begin(),
                   hits.end());
  }
  // std::cout << "Returning " << _result.size() << " hits" << std::endl;
  return _result;
}

std::vector<double> shower_collection::best_dedx_hits_box() {
  std::vector<double> _result;
  _result.reserve(this->size() * 10);
  for (auto & item : *this ) {
    auto hits = item.best_dedx_hits(true);
    // std::cout << "Got " << hits.size() << " hits." << std::endl;
    _result.insert(_result.end(),
                   hits.begin(),
                   hits.end());
  }
  // std::cout << "Returning " << _result.size() << " hits" << std::endl;
  return _result;
}


std::vector<double> shower_collection::distance(int plane) {
  std::vector<double> _result;
  _result.reserve(this->size());
  for (auto & item : *this ) {
    _result.push_back(item.distance(plane));
  }
  return _result;
}

std::vector<double> shower_collection::pitch(int plane) {
  std::vector<double> _result;
  _result.reserve(this->size());
  for (auto & item : *this ) {
    _result.push_back(item.pitch(plane));
  }
  return _result;
}

std::vector<double> shower_collection::mc_3D_vertex_resolution() {
  std::vector<double> _result;
  _result.reserve(this->size());
  for (auto & item : *this ) {
    _result.push_back(item.mc_3D_vertex_resolution());
  }
  return _result;
}

std::vector<double> shower_collection::mc_3D_angle_resolution() {
  std::vector<double> _result;
  _result.reserve(this->size());
  for (auto & item : *this ) {
    _result.push_back(item.mc_3D_angle_resolution());
  }
  return _result;
}

std::vector<double> shower_collection::mc_true_energy() {
  std::vector<double> _result;
  _result.reserve(this->size());
  for (auto & item : *this ) {
    _result.push_back(item.mc_true_energy());
  }
  return _result;
}
std::vector<double> shower_collection::mc_deposited_energy() {
  std::vector<double> _result;
  _result.reserve(this->size());
  for (auto & item : *this ) {
    _result.push_back(item.mc_deposited_energy());
  }
  return _result;
}

std::vector<double> shower_collection::mc_x_resolution() {
  std::vector<double> _result;
  _result.reserve(this->size());
  for (auto & item : *this ) {
    _result.push_back(item.mc_x_resolution());
  }
  return _result;
}

std::vector<double> shower_collection::mc_y_resolution() {
  std::vector<double> _result;
  _result.reserve(this->size());
  for (auto & item : *this ) {
    _result.push_back(item.mc_y_resolution());
  }
  return _result;
}

std::vector<double> shower_collection::mc_z_resolution() {
  std::vector<double> _result;
  _result.reserve(this->size());
  for (auto & item : *this ) {
    _result.push_back(item.mc_z_resolution());
  }
  return _result;
}

std::vector<double> shower_collection::dEdx_best_median() {
  std::vector<double> _result;
  _result.reserve(this->size());
  for (auto & item : *this ) {
    _result.push_back(item.dEdx_best_median());
  }
  return _result;
}

std::vector<double> shower_collection::dEdx_best_modmean() {
  std::vector<double> _result;
  _result.reserve(this->size());
  for (auto & item : *this ) {
    _result.push_back(item.dEdx_best_modmean());
  }
  return _result;
}

std::vector<double> shower_collection::dEdx_best_LMA() {
  std::vector<double> _result;
  _result.reserve(this->size());
  for (auto & item : *this ) {
    _result.push_back(item.dEdx_best_LMA());
  }
  return _result;
}


#endif
