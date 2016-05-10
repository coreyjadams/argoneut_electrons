#ifndef LARLITE_ARGOXINGMUONS2D_CXX
#define LARLITE_ARGOXINGMUONS2D_CXX

#include "ArgoXingMuons2D.h"
#include "LArUtil/DetectorProperties.h"
#include "LArUtil/Geometry.h"

namespace larlite {

bool ArgoXingMuons2D::initialize() {


  _wire_corrections.resize(2, std::vector<double>(240, 1.0));


  _wire_calo_info.resize(2);
  _wire_calo_info.at(0).resize(240);
  _wire_calo_info.at(1).resize(240);



  _collection_muons_median_dedx.clear();
  _collection_muons_median_dqdx.clear();
  _induction_muons_median_dedx.clear();
  _induction_muons_median_dqdx.clear();

  _collection_muons_average_dedx.clear();
  _collection_muons_average_dqdx.clear();
  _induction_muons_average_dedx.clear();
  _induction_muons_average_dqdx.clear();

  _collection_muons_mode_dedx.clear();
  _collection_muons_mode_dqdx.clear();
  _induction_muons_mode_dedx.clear();
  _induction_muons_mode_dqdx.clear();

  _calo_alg.setUseModBox(true);



  return true;
}

bool ArgoXingMuons2D::analyze(storage_manager* storage) {

  // Need to get the tracks out of the event, as well as the associated
  // clusters and hits.


  std::string _track_producer = "pmtrack";

  // get the tracks:
  auto ev_track = storage->get_data<event_track>(_track_producer);

  if (!ev_track)
    return false;
  if (ev_track -> size() == 0)
    return false;

  // Get the calorimetry objects associated with the tracks:
  larlite::event_calorimetry * ev_calo = nullptr;
  auto const& calo_ass_info
    = storage->find_one_ass(ev_track->id(), ev_calo, "calo");

  // std::cout << "ev_calo.size() " << ev_calo -> size() << std::endl;


  // Get the calorimetry objects associated with the tracks:
  larlite::event_spacepoint * ev_sps = nullptr;
  auto const& sps_ass_info
    = storage->find_one_ass(ev_track->id(), ev_sps, _track_producer);

  // Get the calorimetry objects associated with the tracks:
  larlite::event_hit * ev_hit = nullptr;
  auto const& hit_ass_info
    = storage->find_one_ass(ev_sps->id(), ev_hit, ev_sps->id().second);

  // std::cout << "Number of hits: " << ev_hit -> size() << std::endl;

  // Look only at the longest tracks in the event:

  float length = 0;
  size_t longest_index;
  size_t index = 0;
  for (auto & track : * ev_track) {
    // std::cout << "index " << index << ", length " << track.Length() << std::endl;
    if (track.Length() > length) {
      length = track.Length();
      longest_index = index;
    }
    index ++;
  }


  // Only use the longest track for calorimetry
  larlite::calorimetry _induction_calo, _collection_calo;

  // Now look at the hits themselves:

  // How many hits?
  // How many space points?


  auto sps_vec_indexes = sps_ass_info.at(longest_index);

  // Loop over the space points associated with the track and use their associated
  // hits to recompute dQ/dx, dE/dx

  auto track = ev_track->at(longest_index);

  auto geom = larutil::Geometry::GetME();

  // Keep track of the hits for collection and induction
  std::vector<double> coll_dedx;
  std::vector<double> coll_dqdx;
  std::vector<double> ind_dedx;
  std::vector<double> ind_dqdx;

  for (size_t i_traj = 0; i_traj < sps_vec_indexes.size(); i_traj ++) {
    // Get the hits associated with this spacepoint:
    auto hits = hit_ass_info.at(sps_vec_indexes[i_traj]);
    // std::cout << "hits.size() is " << hits.size() << std::endl;
    auto hit = ev_hit -> at(hits[0]);


    // Figure out what plane this hit goes with:


    // Compute the dQ/dx of this hit:
    double Q_area = hit.Integral();

    double pitch = track.PitchInView(hit.View(), i_traj);

    if (pitch == 0 || std::isnan(pitch)) {
      // std::cout << "Pitch is zero or NaN!" << std::endl;
      continue;
    }


    // Do the lifetime correction for each charge:
    Q_area *= _calo_alg.LifetimeCorrection(hit.PeakTime());


    unsigned int plane = hit.WireID().Plane;
    unsigned int wire = hit.WireID().Wire;

    Q_area *= _wire_corrections[plane][wire];
    double dEdx_area = larutil::LArProperties::GetME() -> ModBoxCorrection(Q_area / pitch);

    if (dEdx_area > 8.0){
      continue;
    }

    _wire_calo_info[plane][wire].Q_area.push_back(Q_area);
    // _wire_calo_info[plane][wire].Q_amp.push_back(Q_amp);

    // _wire_calo_info[plane][wire].Q_area_e.push_back(Q_area_e);
    _wire_calo_info[plane][wire].dQdx_area_e.push_back(Q_area / pitch);

    // _wire_calo_info[plane][wire].Q_amp_e.push_back(Q_amp_e);
    // _wire_calo_info[plane][wire].dQdx_amp_e.push_back(Q_amp_e / pitch);
// 
    _wire_calo_info[plane][wire].dEdx_area.push_back(dEdx_area);
    // _wire_calo_info[plane][wire].dEdx_amp.push_back(dEdx_amp);

    _wire_calo_info[plane][wire].n_hits ++;



    if (hit.WireID().Plane == 0) {
      ind_dqdx.push_back(Q_area / pitch);
      ind_dedx.push_back(dEdx_area);
    }
    else if (hit.WireID().Plane == 1) {
      coll_dqdx.push_back(Q_area / pitch);
      coll_dedx.push_back(dEdx_area);
    }

  }

  if (coll_dedx.size() < 50 ||
      ind_dedx.size() < 50) {
    return false;
  }

  // std::cout << "n_coll: " << coll_dedx.size() << ", n_ind: " << ind_dedx.size()  << std::endl;

  _collection_muons_average_dedx.push_back(
    std::accumulate(coll_dedx.begin(),
                    coll_dedx.end(),
                    0.0) / coll_dedx.size());
  // _collection_muons_average_dqdx.push_back(
  //   std::accumulate(coll_dqdx.begin(),
  //                   coll_dqdx.end(),
  //                   0.0) / coll_dqdx.size());
  _induction_muons_average_dedx.push_back(
    std::accumulate(ind_dedx.begin(),
                    ind_dedx.end(),
                    0.0) / ind_dedx.size());
  // _induction_muons_average_dqdx.push_back(
  //   std::accumulate(ind_dqdx.begin(),
  //                   ind_dqdx.end(),
  //                   0.0) / ind_dqdx.size());

  _collection_muons_mode_dedx.push_back(getMode(coll_dedx, 1.25, 2.5, 50));
  // _collection_muons_mode_dqdx.push_back(getMode(coll_dqdx,1.25,2.5,50));
  _induction_muons_mode_dedx.push_back(getMode(ind_dedx, 1.25, 2.5, 50));
  // _induction_muons_mode_dqdx.push_back(getMode(ind_dqdx,1.25,2.5,50));



  // Get the medians for the vectors now:
  std::nth_element(coll_dedx.begin(),
                   coll_dedx.begin() +
                   coll_dedx.size() / 2,
                   coll_dedx.end());

  // Now add the values to the vectors:
  _collection_muons_median_dedx.push_back(
    coll_dedx.at(
      coll_dedx.size() / 2));

  // // Get the medians for the vectors now:
  // std::nth_element(coll_dqdx.begin(),
  //                  coll_dqdx.begin() +
  //                  coll_dqdx.size() / 2,
  //                  coll_dqdx.end());

  // // Now add the values to the vectors:
  // _collection_muons_median_dqdx.push_back(
  //   coll_dqdx.at(
  //     coll_dqdx.size() / 2));

  // Get the medians for the vectors now:
  std::nth_element(ind_dedx.begin(),
                   ind_dedx.begin() +
                   ind_dedx.size() / 2,
                   ind_dedx.end());

  // Now add the values to the vectors:
  _induction_muons_median_dedx.push_back(
    ind_dedx.at(
      ind_dedx.size() / 2));

  // // Get the medians for the vectors now:
  // std::nth_element(ind_dqdx.begin(),
  //                  ind_dqdx.begin() +
  //                  ind_dqdx.size() / 2,
  //                  ind_dqdx.end());

  // // Now add the values to the vectors:
  // _induction_muons_median_dqdx.push_back(
  //   ind_dqdx.at(
  //     ind_dqdx.size() / 2));

  return true;
}

double ArgoXingMuons2D::getMode(const std::vector<double> & invec, float lowbin, float highbin, int n_bins) {

  std::vector<int> histogram;
  std::vector<double> lowerBounds;
  std::vector<double> upperBounds;

  histogram.resize(n_bins);
  lowerBounds.resize(n_bins);
  upperBounds.resize(n_bins);

  double bin_width = (highbin - lowbin) / n_bins;

  for (int i = 0; i < n_bins; i ++) {
    lowerBounds[i] = lowbin + i * bin_width;
    upperBounds[i] = lowbin + (i + 1) * bin_width;
  }

  // Fill in the values from the input vector to the histogram:
  //
  for (auto & val : invec) {
    int bin = 0;
    if (val < lowerBounds[0] || val > upperBounds.back())
      continue;
    while (bin < n_bins) {
      if (val > lowerBounds[bin] && val < upperBounds[bin]) {
        histogram.at(bin) ++;
        break;
      }
      else {
        bin ++;
      }
    }
  }


  // Get the most probable value:
  int max_count = 0;
  int max_bin = 0;
  for (size_t i = 0; i < histogram.size(); i ++) {
    if (histogram[i] > max_count) {
      max_count = histogram[i];
      max_bin = i;
    }
  }
  // for (size_t i = 0; i < histogram.size(); i ++) {
  //   std::cout << lowerBounds[i] << " - " << upperBounds[i] << ":\t"
  //             << histogram[i] << std::endl;
  // }
  // std::cout << "mode is " << (upperBounds[max_bin + 1] + lowerBounds[max_bin]) / 2 << std::endl;

  return (upperBounds[max_bin + 1] + lowerBounds[max_bin]) / 2;
}

bool ArgoXingMuons2D::finalize() {

  return true;
}

}
#endif
