#ifndef SHOWERCALO_CXX
#define SHOWERCALO_CXX

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


  std::sort(cands.begin(), cands.end());
  if (cands.size() % 2 == 0) {
    return 0.5 * (cands.at(cands.size() / 2) + cands.at(cands.size() / 2  - 1));
  }
  else {
    return cands.at(cands.size() / 2);
  }

}


/*
    def dEdx_mean_no_outliers(self, dedx_v, dist_v, dist_cut=2.4, startindex=1):

        # Pick out values with dist >= 0, < dist_cut
        points_within_dist = []
        for val, d in zip(dedx_v[startindex:], dist_v[startindex:]):
            if d > 0 and d < dist_cut:
                points_within_dist.append(val)

        if len(points_within_dist) == 0:
            return 0.0

        mean = np.mean(points_within_dist)
        dev = np.std(points_within_dist)
        val = 0.0
        n = 0
        if dev == 0.0:
            return mean
        for dedx in points_within_dist:
            if abs(mean-dedx) < 2.0*dev and dedx < 10:
                val += dedx
                n += 1
        if n > 0:
            return val/n
        else:
            return 0.0
 */

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

  // Get the mean:
  double sum = std::accumulate(points_within_dist.begin(),
                               points_within_dist.end(),
                               0.0);

  double mean = sum / points_within_dist.size();

  // Now get the rms:
  double rms = 0.0;
  for (auto & val : points_within_dist){
    rms += val*val;
  }
  rms /= points_within_dist.size();
  rms = sqrt(rms);

  if (rms == 0){
    return mean;
  }

  // Now actually get the modified mean:
  double modmean = 0.0;
  int n = 0;
  for (auto & val: points_within_dist){
    if ( fabs(val - mean) < 2*rms && val < 10){
      modmean += val;
      n++;
    }
  }

  if (n == 0){
    return 0.0;
  }
  else{
    return modmean / n;
  }

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
        n_good_hits ++;
      }
    }
  }
  else if (plane == 1) {
    for (size_t index = 0; index < _collection_dedx.size(); index ++) {
      if (_collection_dist_3D.at(index) > 0 && _collection_dist_3D.at(index) < dedx_dist_max) {
        n_good_hits ++;
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

#endif
