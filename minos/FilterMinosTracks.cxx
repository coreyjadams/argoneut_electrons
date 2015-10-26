#ifndef LARLITE_FILTERMINOSTRACKS_CXX
#define LARLITE_FILTERMINOSTRACKS_CXX

#include "FilterMinosTracks.h"
#include "DataFormat/minos.h"
#include "DataFormat/track.h"
#include "DataFormat/mctruth.h"

namespace larlite {

bool FilterMinosTracks::initialize() {

  SetBoundary(1.) ;

  _ana_tree = 0;
  PrepareTTree();

  return true;
}

void FilterMinosTracks::PrepareTTree() {
  if (!_ana_tree) {
    _ana_tree = new TTree("ana_tree", "");
    _ana_tree->Branch("_outGoingLepton", &_outGoingLepton, "outGoingLepton/I");
    _ana_tree->Branch("_pdgRejected", &_pdgRejected, "pdgRejected/I");
    _ana_tree->Branch("_pdgKept", &_pdgKept, "pdgKept/I");
    _ana_tree->Branch("_rejX", &_rejX, "rejX/D");
    _ana_tree->Branch("_rejY", &_rejY, "rejY/D");
    _ana_tree->Branch("_rejZ", &_rejZ, "rejZ/D");
    _ana_tree->Branch("_keptX", &_keptX, "keptX/D");
    _ana_tree->Branch("_keptY", &_keptY, "keptY/D");
    _ana_tree->Branch("_keptZ", &_keptZ, "keptZ/D");

  }
}

void FilterMinosTracks::Reset() {
  _outGoingLepton = -99;
  _pdgRejected  = -99;
  _pdgKept    = -99;
  _rejX     = -999;
  _rejY     = -999;
  _rejZ     = -999;
  _keptX      = -999;
  _keptY      = -999;
  _keptZ      = -999;

}

bool FilterMinosTracks::analyze(storage_manager* storage) {


  auto evt_minos = storage->get_data<larlite::event_minos>("minos");
  auto evt_track = storage->get_data<larlite::event_track>("ct");
  auto evt_mctruth = storage->get_data<larlite::event_mctruth>("generator");


  if (evt_minos->size() == 0 || evt_track->size() == 0 ) {
    //std::cout<<"No minos or no argoneut tracks..."<<std::endl;
    return -1;
  }


  auto ass_info = evt_minos->association(evt_track->id());
  //evt_minos->list_association() ;

  for (auto const & track_indices : ass_info ) {
//    std::cout<<"track indices size: "<<track_indices.size()<<std::endl;
    for (size_t track_index = 0; track_index < track_indices.size(); track_index++) {
      auto X = evt_mctruth->at(track_index).GetNeutrino().Lepton().Trajectory().at(0).X() ;
      auto Y = evt_mctruth->at(track_index).GetNeutrino().Lepton().Trajectory().at(0).Y() ;
      auto Z = evt_mctruth->at(track_index).GetNeutrino().Lepton().Trajectory().at(0).Z() ;

      if ( X < 0 || X > 47 || Y < -20 || Y > 20 || Z < 0 || Z > 90) {
        //    std::cout<<"Outside active volume..."<<std::endl;
        return false;
      }

//        std::cout<<"track index: "<<track_index<<std::endl;
      auto argo_track = evt_track->at(track_index) ;
      _outGoingLepton = evt_mctruth->at(track_index).GetNeutrino().Lepton().PdgCode() ;


      std::vector<double> larStart(3, 0), larEnd(3, 0);
      larStart[0] = argo_track.vertex_at(0)[0];
      larStart[1] = argo_track.vertex_at(0)[1];
      larStart[2] = argo_track.vertex_at(0)[2];


      if (fabs(larStart[0]) < fdBoundary ||
          fabs(47. - larStart[0]) < fdBoundary ||
          fabs(larStart[1] + 20.) < fdBoundary ||
          fabs(20. - larStart[1]) < fdBoundary || 
          fabs(larStart[2]) < fdBoundary || 
          fabs(90 - larStart[2]) < fdBoundary) 
      {
        //    std::cout<<"Starting on a boundary..."<<std::endl;
        _keptX = evt_mctruth->at(track_index).GetNeutrino().Lepton().Trajectory().at(0).X() ;
        _keptY = evt_mctruth->at(track_index).GetNeutrino().Lepton().Trajectory().at(0).Y() ;
        _keptZ = evt_mctruth->at(track_index).GetNeutrino().Lepton().Trajectory().at(0).Z() ;
        _pdgKept = evt_mctruth->at(track_index).GetNeutrino().Lepton().PdgCode() ;
        //    std::cout<<"PDG and Kept indices: "<<", "<<_pdgKept<<", "<<_keptX<<", "<<_keptY<<", "<<_keptZ<<std::endl;

        _ana_tree->Fill();
        Reset() ;
        return true;
      }
      else {
        _rejX = evt_mctruth->at(track_index).GetNeutrino().Lepton().Trajectory().at(0).X() ;
        _rejY = evt_mctruth->at(track_index).GetNeutrino().Lepton().Trajectory().at(0).Y() ;
        _rejZ = evt_mctruth->at(track_index).GetNeutrino().Lepton().Trajectory().at(0).Z() ;
        _pdgRejected = evt_mctruth->at(track_index).GetNeutrino().Lepton().PdgCode() ;
        //    std::cout<<"PDG and Rej indices: "<<_pdgRejected<<", "<<_rejX<<", "<<_rejY<<", "<<_rejZ<<std::endl;

        _ana_tree->Fill() ;
        Reset() ;
        return false;

      }

    }

  }

//  std::cout<<"Not starting on a boundary...get rid of it"<<std::endl;
  return false;
}

bool FilterMinosTracks::finalize() {
  if (_fout) {
    _fout->cd();
    _ana_tree->Write();
  }

  else
    print(larlite::msg::kERROR, __FUNCTION__, "Did not find an output file pointer!!! File not opened?");

  delete _ana_tree ;



  return true;
}

}
#endif
