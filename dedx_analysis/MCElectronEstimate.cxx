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

    nue_NeutrinoEnergy = new TH1F("nue_NeutrinoEnergy","nue_NeutrinoEnergy",100,0,20);
    nue_ElectronEnergy = new TH1F("nue_ElectronEnergy","nue_ElectronEnergy",100,0,20);
    num_NeutrinoEnergy = new TH1F("num_NeutrinoEnergy","num_NeutrinoEnergy",100,0,20);
    num_ElectronEnergy = new TH1F("num_ElectronEnergy","num_ElectronEnergy",100,0,20);
    nue_deposit_Energy = new TH1F("nue_deposit_Energy","nue_deposit_Energy",100,0,20);
    num_deposit_Energy = new TH1F("num_deposit_Energy","num_deposit_Energy",100,0,20);

    nue_trueNu_trueLep = new TH2F("nue_trueNu_trueLep","nue_trueNu_trueLep",100,0,20,100,0,20);
    num_trueNu_trueLep = new TH2F("num_trueNu_trueLep","num_trueNu_trueLep",100,0,20,100,0,20);
    nue_depost_trueLep = new TH2F("nue_depost_trueLep","nue_depost_trueLep",100,0,20,100,0,20);
    num_depost_trueLep = new TH2F("num_depost_trueLep","num_depost_trueLep",100,0,20,100,0,20);




    pot = 0;
    _n_electrons = 0;
    _n_positrons = 0;
    _n_muons = 0;
    _n_antimuons = 0;

    subrun = -1;

    return true;
  }
  
  bool MCElectronEstimate::analyze(storage_manager* storage) {
  
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
  
    // Some histograms for Filling Information
    
    event_mctruth * event_truth = storage -> get_data<event_mctruth>("generator");

    int this_subrun = event_truth -> subrun();
    if (this_subrun != subrun){
      potsummary * event_pot = storage -> get_data<potsummary>("generator");
      pot += event_pot -> totpot;    
      subrun = this_subrun;
    }


    // std::cout << "event_mctruth size: " << event_truth -> size() << std::endl; 

    // Determine the pdg code:
    auto truth = event_truth -> at(0);
    if ( truth.GetNeutrino().CCNC() == 1){
      // std::cout << "This event is CC!\n";
      // std::cout << "This event is NC!\n";
      return false;
    }

    event_mcshower * ev_shower = storage -> get_data<event_mcshower>("mcreco");

    mcshower shower;
    for (auto & sh : *ev_shower){
      if (abs(sh.PdgCode()) == 11 && sh.MotherTrackID() == 1){
        shower = sh;
        break;
      }

    }


    auto neutrino = truth.GetNeutrino().Nu();
    auto lep = truth.GetNeutrino().Lepton();

    if (!isActive(neutrino.Trajectory().front().Position())) return false;

    switch (neutrino.PdgCode()){
      case 14:
        _n_muons ++;
        break;
      case -14:
        _n_antimuons ++;
        break;
      case 12:
        _n_electrons ++;
        break;
      case -12:
        _n_positrons ++;
        break;
      default:
        break;

    }




    if (truth.GetNeutrino().CCNC() == 0){
      float trueE = neutrino.Trajectory().front().E();
      float lepE  = lep.Trajectory().front().E();
      float depE  = shower.DetProfile().E();
      if  (abs(neutrino.PdgCode() ) == 12){
        nue_NeutrinoEnergy -> Fill(trueE);
        nue_ElectronEnergy -> Fill(lepE);
        nue_deposit_Energy -> Fill(depE);
        nue_trueNu_trueLep -> Fill(trueE,lepE);
        nue_depost_trueLep -> Fill(lepE, depE);
      }
      else if (abs(neutrino.PdgCode() ) == 14 ){
        num_NeutrinoEnergy -> Fill(trueE);
        num_ElectronEnergy -> Fill(lepE);
        num_deposit_Energy -> Fill(depE);
        num_trueNu_trueLep -> Fill(trueE,lepE);
        num_depost_trueLep -> Fill(lepE, depE);
      }
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

    const float runningPOT = 1.2e20;
  
    float _n_electrons_scaled =  _n_electrons * (runningPOT / pot); 
    float _n_positrons_scaled =  _n_positrons * (runningPOT / pot); 
    float _n_muons_scaled     =  _n_muons     * (runningPOT / pot); 
    float _n_antimuons_scaled =  _n_antimuons * (runningPOT / pot); 

    std::cout << "\n\n\nOutput from MCElectronEstimate:\n";

    // // some useful numbers to calculate:
    std::cout << "Electrons: " << _n_electrons << " scaled to: "
              <<  _n_electrons_scaled << std::endl;
    std::cout << "Positrons: " << _n_positrons << " scaled to: "
              <<  _n_positrons_scaled << std::endl;
    std::cout << "Muons    : " << _n_muons     << " scaled to: "
              <<  _n_muons_scaled     << std::endl;
    std::cout << "Antimuons: " << _n_antimuons << " scaled to: "
              <<  _n_antimuons_scaled << std::endl;
    std::cout << "Total POT: " << pot << std::endl;


    if (_fout){
      _fout -> cd();
      nue_NeutrinoEnergy->Write();
      nue_ElectronEnergy->Write();
      nue_deposit_Energy->Write();
      nue_trueNu_trueLep->Write();
      nue_depost_trueLep->Write();
      num_NeutrinoEnergy->Write();
      num_ElectronEnergy->Write();
      num_deposit_Energy->Write();
      num_trueNu_trueLep->Write();
      num_depost_trueLep->Write();
    }

    return true;
  }

  bool MCElectronEstimate::isActive(const TLorentzVector & vertex){
    // Argoneut specific
    if (vertex.X() > 23.5 || vertex.X() < -23.5) return false;
    if (vertex.Y() > 20 || vertex.Y() < -20) return false;
    if (vertex.Z() > 90 || vertex.Z() < 0) return false;
  
    return true;
  }

}
#endif
