#ifndef LARLITE_MCSHOWERANA_CXX
#define LARLITE_MCSHOWERANA_CXX

#define _USE_MATH_DEFINES

#include <cmath>
#include <iostream>
#include "MCShowerAna.h"
#include "DataFormat/mctruth.h"
#include "DataFormat/mcshower.h"
#include "DataFormat/shower.h"
#include "LArUtil/Geometry.h"

namespace argoana {

bool MCShowerAna::initialize() {

  //
  // This function is called in the beggining of event loop
  // Do all variable initialization you wish to do here.
  // If you have a histogram to fill in the event loop, for example,
  // here is a good place to create one on the heap (i.e. "new TH1D").
  //

  // Histograms to compare vertex information, dE/dx, direction:
  vertex_X = new TH1F("vertex_x", "X vertex True - reco", 100, -25, 25);
  vertex_Y = new TH1F("vertex_y", "Y vertex True - reco", 100, -25, 25);
  vertex_Z = new TH1F("vertex_z", "Z vertex True - reco", 100, -25, 25);
  vertex_abs = new TH1F("vertex_abs", "Abs vertex True - reco", 100, -25, 25);

  direction_X = new TH1F("direction_x", "X direction True - reco", 100, -25, 25);
  direction_Y = new TH1F("direction_y", "Y direction True - reco", 100, -25, 25);
  direction_Z = new TH1F("direction_z", "Z direction True - reco", 100, -25, 25);
  direction_angle = new TH1F("direction_angle", "Angle Between True and reco", 100, -3.14, 3.14);

  dEdx = new TH1F("dEdx", "Reconstructed dEdx", 50, -2, 20);
  dEdx_fid = new TH1F("dEdx_fid", "Reconstructed dEdx", 50, -2, 20);


  _output_tree = new TTree("showerRecoQuality","showerRecoQuality");

  // items going into the output tree:
  _output_tree->Branch("vertex_x_true", &vertex_x_true);
  _output_tree->Branch("vertex_y_true", &vertex_y_true);
  _output_tree->Branch("vertex_z_true", &vertex_z_true);

  _output_tree->Branch("vertex_x_reco", &vertex_x_reco);
  _output_tree->Branch("vertex_y_reco", &vertex_y_reco);
  _output_tree->Branch("vertex_z_reco", &vertex_z_reco);

  _output_tree->Branch("direction_x_true", &direction_x_true);
  _output_tree->Branch("direction_y_true", &direction_y_true);
  _output_tree->Branch("direction_z_true", &direction_z_true);

  _output_tree->Branch("direction_x_reco", &direction_x_reco);
  _output_tree->Branch("direction_y_reco", &direction_y_reco);
  _output_tree->Branch("direction_z_reco", &direction_z_reco);


  return true;
}



bool MCShowerAna::analyze(larlite::storage_manager* storage) {
  auto geom = larutil::Geometry::GetME();

  //
  // Do your event-by-event analysis here. This function is called for
  // each event in the loop. You have "storage" pointer which contains
  // event-wise data. To see what is available, check the "Manual.pdf":
  //
  // http://microboone-docdb.fnal.gov:8080/cgi-bin/ShowDocument?docid=3183
  //
  // Or you can refer to Base/DataFormatConstants.hh for available data type
  // enum values. Here is one example of getting PMT waveform collection.
  //
  // event_fifo* my_pmtfifo_v = (event_fifo*)(storage->get_data(DATA::PMFIFO));
  //
  // if( event_fifo )
  //
  //   std::cout << "Event ID: " << my_pmtfifo_v->event_id() << std::endl;
  //


  larlite::event_mctruth * event_truth = storage -> get_data<larlite::event_mctruth>("generator");
  auto truth = event_truth -> at(0);
  if ( truth.GetNeutrino().CCNC() == 1) {
    return false;
  }


  auto & true_shower = event_truth->front().GetParticles().front();

  TVector3 trueVertex = true_shower.Position().Vect();
  TVector3 trueDirection = true_shower.Momentum().Vect();


  // Get the showers
  larlite::event_shower * reco_shower = storage -> get_data<larlite::event_shower>("showerreco");

  if (reco_shower -> size() == 0) return false;

  // Normalize the direction:
  TVector3 trueDirNorm = trueDirection;
  trueDirNorm *= 1.0 / trueDirNorm.Mag();





  // std::cout << "This event (" << storage -> event_id() << ") has " << reco_shower->size() << " reco showers.\n";
  //std::cout << "PDG of the outgoing lepton is " << lep.PdgCode() << std::endl;

  for (auto & shower : * reco_shower) {
    TVector3 start = shower.ShowerStart();
    TVector3 dir   = shower.Direction();
    std::vector<double> dedx = shower.dEdx_v();
    std::vector<double> energy = shower.Energy_v();

    vertex_x_true = trueVertex.X();
    vertex_y_true = trueVertex.Y();
    vertex_z_true = trueVertex.Z();
    vertex_x_reco = start.X();
    vertex_y_reco = start.Y();
    vertex_z_reco = start.Z();
    direction_x_true = trueDirNorm.X();
    direction_y_true = trueDirNorm.Y();
    direction_z_true = trueDirNorm.Z();
    direction_x_reco = dir.X();
    direction_y_reco = dir.Y();
    direction_z_reco = dir.Z();

    // Fill in the histograms:
    vertex_X      -> Fill(trueVertex.X() - start.X());
    vertex_Y      -> Fill(trueVertex.Y() - start.Y());
    vertex_Z      -> Fill(trueVertex.Z() - start.Z());
    // vertex_abs    -> Fill();
    direction_X   -> Fill(trueDirNorm.X() - dir.X());
    direction_Y   -> Fill(trueDirNorm.Y() - dir.Y());
    direction_Z   -> Fill(trueDirNorm.Z() - dir.Z());
    float theta = dir.Dot(trueDirNorm) / (dir.Mag() * trueDirNorm.Mag());
    theta = acos(theta);
    // std::cout << "Theta is " << theta << std::endl;
    direction_angle -> Fill(theta);
    dEdx          -> Fill(dedx[1]);
    if (isFiducial(start)) {
      dEdx_fid -> Fill(dedx[1]);
      if (dedx[1] > 1.5 && dedx[1] < 5.0) _good_event_list.push_back(storage->event_id());
    }
    _output_tree -> Fill();
  }

  return true;
}

bool MCShowerAna::finalize() {

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

  if (_fout) {
    _fout -> cd();
    vertex_X      -> Write();
    vertex_Y      -> Write();
    vertex_Z      -> Write();
    vertex_abs    -> Write();
    direction_X   -> Write();
    direction_Y   -> Write();
    direction_Z   -> Write();
    direction_angle -> Write();
    dEdx          -> Write();
    dEdx_fid      -> Write();
    _output_tree  -> Write();
  }

  // std::cout << "Printing good events: \n";
  // for (auto event : _good_event_list) {
  //   std::cout << event << '\n';
  // }


  return true;
}

bool MCShowerAna::isFiducial(const TVector3 & vertex) {
  // Argoneut specific
  if (vertex.X() > 23.5 || vertex.X() < -23.5) return false;
  if (vertex.Y() > 20 || vertex.Y() < -20) return false;
  if (vertex.Z() > 90 || vertex.Z() < 0) return false;

  return true;
}

}
#endif
