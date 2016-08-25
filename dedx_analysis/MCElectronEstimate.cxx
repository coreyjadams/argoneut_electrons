#ifndef LARLITE_MCELECTRONESTIMATE_CXX
#define LARLITE_MCELECTRONESTIMATE_CXX

#include "MCElectronEstimate.h"
#include "DataFormat/mctruth.h"
#include "DataFormat/potsummary.h"
#include "DataFormat/mcshower.h"

namespace larlite {

bool MCElectronEstimate::initialize() {

  //
  // This function is called in the beggining of event loop
  // Do all variable initialization you wish to do here.
  // If you have a histogram to fill in the event loop, for example,
  // here is a good place to create one on the heap (i.e. "new TH1D").
  //


  pot = 0;
  subrun = -1;
  _deposited_energy.clear();
  _true_energy.clear();
  _pdg.clear();
  _theta.clear();
  _phi.clear();

  return true;
}

bool MCElectronEstimate::begin_subrun(storage_manager* data)
{
  auto potsum = data->get_subrundata<larlite::potsummary>("generator");
  pot += potsum->totpot;
  return true;
}

bool MCElectronEstimate::analyze(storage_manager* storage) {
  event_mctruth * event_truth = storage -> get_data<event_mctruth>("generator");


  // std::cout << "event_mctruth size: " << event_truth -> size() << std::endl;

  // Determine the pdg code:
  auto truth = event_truth -> at(0);
  if ( truth.GetNeutrino().CCNC() == 1) {
    // std::cout << "This event is CC!\n";
    // std::cout << "This event is NC!\n";
    return false;
  }

  event_mcshower * ev_shower = storage -> get_data<event_mcshower>("mcreco");

  mcshower shower;
  for (auto & sh : *ev_shower) {
    if (abs(sh.PdgCode()) == 11 && sh.MotherTrackID() == 1) {
      shower = sh;
      break;
    }

  }


  auto neutrino = truth.GetNeutrino().Nu();
  auto lep = truth.GetNeutrino().Lepton();

  if (!isActive(neutrino.Trajectory().front().Position())) return false;




  if (truth.GetNeutrino().CCNC() == 0) {
    float trueE = neutrino.Trajectory().front().E();
    float lepE  = lep.Trajectory().front().E();
    float depE  = shower.DetProfile().E();


    // Fill in the vectors:
    _deposited_energy.push_back(depE);
    _true_energy.push_back(lepE);
    _pdg.push_back(neutrino.PdgCode());

    _theta.push_back(shower.StartDir().Theta());
    _phi.push_back(shower.StartDir().Phi());



  }

  return true;
}

bool MCElectronEstimate::finalize() {

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


  return true;
}

bool MCElectronEstimate::isActive(const TLorentzVector & vertex, double cut) {
  // Argoneut specific
  if (vertex.X() > 23.5 - cut || vertex.X() < -23.5 + cut) return false;
  if (vertex.Y() > 20 - cut || vertex.Y() < -20 + cut) return false;
  if (vertex.Z() > 90 - 25 || vertex.Z() < 0 + cut) return false;

  return true;
}

}
#endif
