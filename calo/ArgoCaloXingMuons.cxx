#ifndef LARLITE_ARGOCALOXINGMUONS_CXX
#define LARLITE_ARGOCALOXINGMUONS_CXX

#include "ArgoCaloXingMuons.h"
#include "LArUtil/DetectorProperties.h"
#include "LArUtil/Geometry.h"

namespace larlite {

bool ArgoCaloXingMuons::initialize() {

  //
  // This function is called in the beggining of event loop
  // Do all variable initialization you wish to do here.
  // If you have a histogram to fill in the event loop, for example,
  // here is a good place to create one on the heap (i.e. "new TH1D").
  //

  std::cout << "Got Here" << std::endl;

  _collection_dqdx_by_wire.resize(240);
  _collection_dedx_by_wire.resize(240);
  _induction_dqdx_by_wire.resize(240);
  _induction_dedx_by_wire.resize(240);
  _collection_dqdx_by_wire_count.resize(240);
  _induction_dqdx_by_wire_count.resize(240);

  _wire_calo_info.resize(2);
  _wire_calo_info.at(0).resize(240);
  _wire_calo_info.at(1).resize(240);


  _wire_corrections.resize(2, std::vector<double>(240, 1.0));

  unsigned int i = 0;
  for (auto & wci : _wire_calo_info[0]) {
    wci.plane = 0;
    wci.wire = i;
    wci._dQdx_area = new TH1F(Form("_dqdx_area_0_%i", i),
                              Form("_dqdx_area_0_%i", i),
                              100, 0, 400000);
    wci._dQdx_amp = new TH1F(Form("_dQdx_amp_0_%i", i),
                             Form("_dQdx_amp_0_%i", i),
                             100, 0, 400000);
    wci._dEdx_area = new TH1F(Form("_dEdx_area_0_%i", i),
                              Form("_dEdx_area_0_%i", i),
                              100, 0, 15);
    wci._dEdx_amp = new TH1F(Form("_dEdx_amp_0_%i", i),
                             Form("_dEdx_amp_0_%i", i),
                             100, 0, 15);
    i ++;
  }
  i = 0;
  for (auto & wci : _wire_calo_info[1]) {
    wci.plane = 1;
    wci.wire = i;
    wci._dQdx_area = new TH1F(Form("_dqdx_area_1_%i", i),
                              Form("_dqdx_area_1_%i", i),
                              100, 0, 400000);
    wci._dQdx_amp = new TH1F(Form("_dQdx_amp_1_%i", i),
                             Form("_dQdx_amp_1_%i", i),
                             100, 0, 400000);
    wci._dEdx_area = new TH1F(Form("_dEdx_area_1_%i", i),
                              Form("_dEdx_area_1_%i", i),
                              100, 0, 15);
    wci._dEdx_amp = new TH1F(Form("_dEdx_amp_1_%i", i),
                             Form("_dEdx_amp_1_%i", i),
                             100, 0, 15);
    i ++;
  }


  _collection_wire_dqdx =
    new TH1F("_collection_wire_dqdx",
             "_collection_wire_dqdx",
             240, 0, 240);
  _collection_wire_dedx =
    new TH1F("_collection_wire_dedx",
             "_collection_wire_dedx",
             240, 0, 240);
  _induction_wire_dqdx =
    new TH1F("_induction_wire_dqdx",
             "_induction_wire_dqdx",
             240, 0, 240);
  _induction_wire_dedx =
    new TH1F("_induction_wire_dedx",
             "_induction_wire_dedx",
             240, 0, 240);
  _collection_calo_dqdx =
    new TH1F("_collection_calo_dqdx",
             "_collection_calo_dqdx",
             100, 0, 4000);
  _induction_calo_dqdx =
    new TH1F("_induction_calo_dqdx",
             "_induction_calo_dqdx",
             100, 0, 1400);
  _collection_calo_dEdx =
    new TH1F("_collection_calo_dEdx",
             "_collection_calo_dEdx",
             100, 0, 10);
  _induction_calo_dEdx =
    new TH1F("_induction_calo_dEdx",
             "_induction_calo_dEdx",
             100, 0, 10);

  _collection_caloAlg_dqdx_area =
    new TH1F("_collection_caloAlg_dqdx_area",
             "_collection_caloAlg_dqdx_area",
             100, 0, 4000);
  _induction_caloAlg_dqdx_area =
    new TH1F("_induction_caloAlg_dqdx_area",
             "_induction_caloAlg_dqdx_area",
             100, 0, 1400);
  _collection_caloAlg_dEdx_area =
    new TH1F("_collection_caloAlg_dEdx_area",
             "_collection_caloAlg_dEdx_area",
             100, 0, 10);
  _induction_caloAlg_dEdx_area =
    new TH1F("_induction_caloAlg_dEdx_area",
             "_induction_caloAlg_dEdx_area",
             100, 0, 10);

  _collection_caloAlg_dqdx_amp =
    new TH1F("_collection_caloAlg_dqdx_amp",
             "_collection_caloAlg_dqdx_amp",
             100, 0, 4000);
  _induction_caloAlg_dqdx_amp =
    new TH1F("_induction_caloAlg_dqdx_amp",
             "_induction_caloAlg_dqdx_amp",
             100, 0, 1400);
  _collection_caloAlg_dEdx_amp =
    new TH1F("_collection_caloAlg_dEdx_amp",
             "_collection_caloAlg_dEdx_amp",
             100, 0, 10);
  _induction_caloAlg_dEdx_amp =
    new TH1F("_induction_caloAlg_dEdx_amp",
             "_induction_caloAlg_dEdx_amp",
             100, 0, 10);

  _calo_alg.setUseModBox(true);



  return true;
}

bool ArgoCaloXingMuons::analyze(storage_manager* storage) {

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

  // Do some tracks to make sure the longest
  // track actually crosses the detector

  // std::cout << "Longest index is " << longest_index << std::endl;
  // std::cout << "calo_ass_info.size() " << calo_ass_info.size() << std::endl;

  // Now look at the longest track and it's calorimetry info.
  // The tracks should be 1-2 with calorimetry (one per plane)

  larlite::calorimetry _induction_calo, _collection_calo;
  auto calo_indexes = calo_ass_info.at(longest_index);

  // std::cout << "Indexes.size(): " << calo_indexes.size() << std::endl;

  for (auto calo_index : calo_indexes) {

    // Get this calorimetry object:
    auto & this_calo = ev_calo -> at(calo_index);

    // Collection:
    if (this_calo.PlaneID().Plane == 1) {
      for (auto & val : this_calo.dEdx()) {
        _collection_calo_dEdx -> Fill(val);
      }
      for (auto & val : this_calo.dQdx()) {
        _collection_calo_dqdx -> Fill(val);
      }
    }
    // Induction:
    else if (this_calo.PlaneID().Plane == 0) {
      for (auto & val : this_calo.dQdx()) {
        _induction_calo_dqdx -> Fill(val);
      }
      for (auto & val : this_calo.dEdx()) {
        _induction_calo_dEdx -> Fill(val);
      }
    }




  }

  // Now look at the hits themselves:

  // How many hits?
  // How many space points?


  auto sps_vec_indexes = sps_ass_info.at(longest_index);
  // std::cout << "number of spacepoints: " << sps_vec_indexes.size() << std::endl;
  // std::cout << "number of traj points: " << ev_track->at(longest_index).NumberTrajectoryPoints() << std::endl;
  // std::cout << "number of hit association vectors: " << hit_ass_info.size() << std::endl;

  // std::cout << "First sps index: " << sps_vec_indexes.front() << std::endl;
  // std::cout << "First spt is associate with " << hit_ass_info.at(sps_vec_indexes[0]).size() << " hits." << std::endl;
  // std::cout << "Second spt is associate with " << hit_ass_info.at(sps_vec_indexes[1]).size() << " hits." << std::endl;

  // if (ev_calo -> at(calo_indexes.front()) )
  // Loop over the space points associated with the track and use their associated
  // hits to recompute dQ/dx, dE/dx

  auto track = ev_track->at(longest_index);

  auto geom = larutil::Geometry::GetME();

  for (size_t i_traj = 0; i_traj < sps_vec_indexes.size(); i_traj ++) {
    // Get the hits associated with this spacepoint:
    auto hits = hit_ass_info.at(sps_vec_indexes[i_traj]);
    // std::cout << "hits.size() is " << hits.size() << std::endl;
    auto hit = ev_hit -> at(hits[0]);
    // std::cout << "hit plane is " << hit.WireID().Plane << std::endl;

    // std::cout << "trajectory point at ("
    //           << ev_track->at(longest_index).LocationAtPoint(i_traj).X() << ", "
    //           << ev_track->at(longest_index).LocationAtPoint(i_traj).Y() << ", "
    //           << ev_track->at(longest_index).LocationAtPoint(i_traj).Z() << ") "
    //           << " associated to hit at ("
    //           << hit.WireID().Wire << ", " << hit.PeakTime() << ")."
    //           << std::endl;

    // Figure out what plane this hit goes with:


    // Compute the dQ/dx of this hit:
    double Q_area = hit.Integral() * area_q_corrections[hit.WireID().Plane];
    double Q_amp = hit.PeakAmplitude() * amp_q_corrections[hit.WireID().Plane];
    // Q_amp *= _calo_alg.LifetimeCorrection(hit.PeakTime());

    // Q_area /= area_q_corrections[hit.WireID().Plane];
    // Q_amp /= amp_q_corrections[hit.WireID().Plane];

    double pitch = track.PitchInView(hit.View(), i_traj);

    if (pitch == 0 || std::isnan(pitch)) {
      // std::cout << "Pitch is zero or NaN!" << std::endl;
      continue;
    }

    // std::cout << "hit.Integral() " << hit.Integral() << std::endl;
    // std::cout << "hit.PeakAmplitude() " << hit.PeakAmplitude() << std::endl;

    // double dEdx_area = _calo_alg.dEdx_AREA(Q_area, hit.PeakTime(), pitch, hit.WireID().Plane);
    // double dEdx_amp = _calo_alg.dEdx_AMP(Q_amp, hit.PeakTime(), pitch, hit.WireID().Plane);

    // Do the lifetime correction for each charge:
    Q_area *= _calo_alg.LifetimeCorrection(hit.PeakTime());
    Q_amp *= _calo_alg.LifetimeCorrection(hit.PeakTime());


    double Q_area_e = Q_area * area_e_corrections[hit.WireID().Plane];
    double Q_amp_e = Q_amp * amp_e_corrections[hit.WireID().Plane];

    //Do the correction:
    // larutil::LArProperties::GetME() -> BirksCorrection
    double dEdx_area = larutil::LArProperties::GetME() -> ModBoxCorrection(Q_area_e / pitch);
    double dEdx_amp = larutil::LArProperties::GetME() -> ModBoxCorrection(Q_amp_e / pitch);

    // std::cout << "Q_area: " << Q_area << ", " << "pitch: " << pitch << std::endl;
    // std::cout << "Q_amp: " << Q_amp << ", " << "pitch: " << pitch << std::endl;
    // std::cout << "dEdx_area: " << dEdx_area << std::endl;
    // std::cout << "dEdx_amp: " << dEdx_amp << std::endl;

    unsigned int plane = hit.WireID().Plane;
    unsigned int wire = hit.WireID().Wire;

    Q_area *= _wire_corrections[plane][wire];
    Q_amp *= _wire_corrections[plane][wire];
    Q_area_e *= _wire_corrections[plane][wire];
    Q_amp_e *= _wire_corrections[plane][wire];

    _wire_calo_info[plane][wire].Q_area.push_back(Q_area);
    _wire_calo_info[plane][wire].Q_amp.push_back(Q_amp);

    _wire_calo_info[plane][wire].Q_area_e.push_back(Q_area_e);
    _wire_calo_info[plane][wire].dQdx_area_e.push_back(Q_area_e / pitch);

    _wire_calo_info[plane][wire].Q_amp_e.push_back(Q_amp_e);
    _wire_calo_info[plane][wire].dQdx_amp_e.push_back(Q_amp_e / pitch);

    _wire_calo_info[plane][wire].dEdx_area.push_back(dEdx_area);
    _wire_calo_info[plane][wire].dEdx_amp.push_back(dEdx_amp);

    _wire_calo_info[plane][wire].n_hits ++;

    _wire_calo_info[plane][wire]._dQdx_area -> Fill(Q_area_e / pitch);
    _wire_calo_info[plane][wire]._dQdx_amp -> Fill(Q_amp_e / pitch);
    _wire_calo_info[plane][wire]._dEdx_area -> Fill(dEdx_area);
    _wire_calo_info[plane][wire]._dEdx_amp -> Fill(dEdx_amp);

    if (hit.WireID().Plane == 0) {
      _induction_caloAlg_dqdx_area -> Fill(Q_area / pitch);
      _induction_caloAlg_dEdx_area -> Fill(dEdx_area);
      _induction_caloAlg_dqdx_amp -> Fill(Q_amp / pitch);
      _induction_caloAlg_dEdx_amp -> Fill(dEdx_amp);
      _induction_dedx_by_wire[hit.WireID().Wire] += dEdx_area;
      _induction_dqdx_by_wire[hit.WireID().Wire] +=  Q_area / pitch;
      _induction_dqdx_by_wire_count[hit.WireID().Wire] +=  1;
    }
    else if (hit.WireID().Plane == 1) {
      _collection_caloAlg_dqdx_area -> Fill(Q_area / pitch);
      _collection_caloAlg_dEdx_area -> Fill(dEdx_area);
      _collection_caloAlg_dqdx_amp -> Fill(Q_amp / pitch);
      _collection_caloAlg_dEdx_amp -> Fill(dEdx_amp);
      _collection_dedx_by_wire[hit.WireID().Wire] += dEdx_area;
      _collection_dqdx_by_wire[hit.WireID().Wire] +=  Q_area / pitch;
      _collection_dqdx_by_wire_count[hit.WireID().Wire] +=  1;
      // if (hit.WireID().Wire == 43) {

      //   if (std::isnan(_collection_dqdx_by_wire[hit.WireID().Wire]) ) {
      //     std::cout << "Adding " << Q_area << " / " <<  pitch
      //               << " to get " << _collection_dqdx_by_wire[hit.WireID().Wire]
      //               << std::endl;
      //     exit(-1);
      //   }
      // }
    }

  }


  return true;
}

bool ArgoCaloXingMuons::finalize() {

  // This function is called at the end of event loop.
  // Do all variable finalization you wish to do here.
  // If you need, you can store your ROOT class instance in the output
  // file. You have an access to the output file through "_fout" pointer.
  //
  // Say you made a histogram pointer h1 to store. You can do this:
  //
  // if(_fout) { _fout->cd(); h1->Write(); }
  //
  // else
  //   print(MSG::ERROR,__FUNCTION__,"Did not find an output file pointer!!! File not opened?");
  //

  // Get the mean values:
  _induction_q_area_mean = _induction_caloAlg_dqdx_area -> GetMean();
  _induction_q_amp_mean = _induction_caloAlg_dqdx_amp -> GetMean();

  _collection_q_area_mean = _collection_caloAlg_dqdx_area -> GetMean();
  _collection_q_amp_mean = _collection_caloAlg_dqdx_amp -> GetMean();

  _induction_q_target_mean = _induction_calo_dqdx -> GetMean();
  _collection_q_target_mean = _collection_calo_dqdx -> GetMean();

  // Get the mean values:
  _induction_e_area_mean = _induction_caloAlg_dEdx_area -> GetMean();
  _induction_e_amp_mean = _induction_caloAlg_dEdx_amp -> GetMean();

  _collection_e_area_mean = _collection_caloAlg_dEdx_area -> GetMean();
  _collection_e_amp_mean = _collection_caloAlg_dEdx_amp -> GetMean();

  _induction_e_target_mean = _induction_calo_dEdx -> GetMean();
  _collection_e_target_mean = _collection_calo_dEdx -> GetMean();


  for (size_t wire = 0; wire < 240; wire ++) {

    // Figure out the wire-by-wire corrections:

    _collection_wire_dqdx->SetBinContent(wire + 1,
                                         _wire_calo_info[1][wire]._dQdx_area->GetMean() );
    _collection_wire_dedx->SetBinContent(wire + 1,
                                         _wire_calo_info[1][wire]._dEdx_area->GetMean() );
    _induction_wire_dqdx->SetBinContent(wire + 1,
                                        _wire_calo_info[0][wire]._dQdx_area->GetMean() );
    _induction_wire_dedx->SetBinContent(wire + 1,
                                        _wire_calo_info[0][wire]._dEdx_area->GetMean() );
  }

//   // Fit to those histograms above for the mean values
//   _collection_wire_dqdx -> Fit("pol0");
//   _induction_wire_dqdx -> Fit("pol0");

//   TF1 * c_fit = _collection_wire_dqdx->GetFunction("pol0");
//   TF1 * i_fit = _induction_wire_dqdx->GetFunction("pol0");
// // value of the first parameter
//   double c_p0 = c_fit -> GetParameter(0);
//   double i_p0 = i_fit -> GetParameter(0);

//   std::cout << "c_p0 is " << c_p0 << std::endl;
//   std::cout << "i_p0 is " << i_p0 << std::endl;

  double _collection_target, _induction_target;

  // for (size_t wire = 0; wire < 240; wire ++) {

  //   // Require at least 500 points
  //   if (_wire_calo_info[0][wire].n_hits > 500) {
  //     _wire_calo_info[0][wire]._correction_factor = c_p0 /
  //         _wire_calo_info[0][wire]._dQdx_area->GetMean();
  //     _wire_corrections[0][wire] = _wire_calo_info[0][wire]._correction_factor;
  //   }
  //   if (_wire_calo_info[1][wire].n_hits > 500) {
  //     _wire_calo_info[1][wire]._correction_factor = i_p0 /
  //         _wire_calo_info[1][wire]._dQdx_area->GetMean();
  //     _wire_corrections[1][wire] = _wire_calo_info[1][wire]._correction_factor;
  //   }
  // }

  // for (size_t wire = 0; wire < 240; wire ++) {

  //   // Figure out the wire-by-wire corrections:

  //   _collection_wire_dqdx->SetBinContent(wire + 1,
  //                                        _wire_calo_info[1][wire]._dQdx_area->GetMean() *
  //                                        _wire_calo_info[1][wire]._correction_factor );
  //   _induction_wire_dqdx->SetBinContent(wire + 1,
  //                                       _wire_calo_info[0][wire]._dQdx_area->GetMean() *
  //                                       _wire_calo_info[0][wire]._correction_factor );
  // }

  if (_fout) {
    _fout -> cd();
    _collection_calo_dqdx -> Write();
    _induction_calo_dqdx -> Write();
    _collection_calo_dEdx -> Write();
    _induction_calo_dEdx -> Write();
    _induction_caloAlg_dqdx_area -> Write();
    _induction_caloAlg_dEdx_area -> Write();
    _induction_caloAlg_dqdx_amp -> Write();
    _induction_caloAlg_dEdx_amp -> Write();
    _collection_caloAlg_dqdx_area -> Write();
    _collection_caloAlg_dEdx_area -> Write();
    _collection_caloAlg_dqdx_amp -> Write();
    _collection_caloAlg_dEdx_amp -> Write();
    _collection_wire_dqdx -> Write();
    _induction_wire_dqdx -> Write();
    _collection_wire_dedx -> Write();
    _induction_wire_dedx -> Write();

    // // Write out the histograms from the individual wires:
    // for (size_t plane = 0; plane < 2; plane ++) {
    //   for (size_t wire = 0; wire < 240; wire++) {
    //     _wire_calo_info[plane][wire]._dQdx_area -> Write();
    //   }
    // }

  }


  return true;
}

}
#endif
