#ifndef LARLITE_MCSHOWERANA_CXX
#define LARLITE_MCSHOWERANA_CXX

#include "MCShowerAna.h"
#include "DataFormat/mctruth.h"
#include "DataFormat/mcshower.h"

namespace larlite {

  bool MCShowerAna::initialize() {

    //
    // This function is called in the beggining of event loop
    // Do all variable initialization you wish to do here.
    // If you have a histogram to fill in the event loop, for example,
    // here is a good place to create one on the heap (i.e. "new TH1D"). 
    //

    return true;
  }
  
  bool MCShowerAna::analyze(storage_manager* storage) {
  
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


    event_mctruth * event_truth = storage -> get_data<event_mctruth>("generator");
    auto truth = event_truth -> at(0);
    if ( truth.GetNeutrino().CCNC() == 1){
      return false;
    }


    auto neutrino = truth.GetNeutrino().Nu();
    auto lep = truth.GetNeutrino().Lepton();
  


    // Only look at electron neutrino events?

    // get the mc showers:
    event_mcshower * ev_shower = storage -> get_data<event_mcshower>("mcreco");
    
    std::cout << "This event has " << ev_shower->size() << " showers.\n";
    for (auto & shower : * ev_shower){
      if (shower.DetProfile().E() < 10) continue;
        std::cout << "\tPDG: ........ " << shower.PdgCode() << "\n"
                  << "\tTrack ID: ... " << shower.TrackID() << "\n"
                  << "\tMother ID: .. " << shower.MotherTrackID() << "\n"
                  << "\tTotal E: .... " << shower.MotherStart().E() << "\n"
                  << "\tDep E: ...... " << shower.DetProfile().E() << "\n"
                  << std::endl;
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
  
    return true;
  }

}
#endif
