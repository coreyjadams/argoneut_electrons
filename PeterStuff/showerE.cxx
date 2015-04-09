#ifndef LARLITE_SHOWERE_CXX
#define LARLITE_SHOWERE_CXX

#include "showerE.h"
#include "DataFormat/storage_manager.h"
#include "TVector3.h"
#include "math.h"


namespace larlite {

    //
    // This function is called in the beggining of event loop
    // Do all variable initialization you wish to do here.
    // If you have a histogram to fill in the event loop, for example,
    // here is a good place to create one on the heap (i.e. "new TH1D"). 
    //

  // TH1D *fShowerHist = nullptr;
  // TH2D *fVertex = nullptr;

  bool showerE::initialize() {

	// fShowerHist = new TH1D("fShowerHist","Energy Distribution", 100,0,1500);
	// fVertex     = new TH2D("fVertex", "Vertex", 2, 0, 100, 2, 0, 100);

    fHeCount = new TH1D("fHeCount","Electron Shower Event Count", 20,0,20);
    fHmcE = new TH1D("fHmcE","Vertex Energy",100,0,400);
    fH3dE = new TH1D("fH3dE","3d Energy",100,0,400);
    eCount = 0;

// 
    return true;
  }
 
  bool showerE::analyze(storage_manager* storage) {

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

    // auto TruthHandle = storage->get_data<larlite::event_mctruth>("generator");

	auto MCShowerHandle = storage->get_data<larlite::event_mcshower>("mcreco");
    auto ShowerHandle = storage->get_data<larlite::event_shower>("shower3d");

	// auto VertexHandle = storage->get_data<larlite::event_vertex>("feature");

	for(unsigned int i=0; i < MCShowerHandle->size(); i++){
	
        if(MCShowerHandle->at(i).PdgCode() == 11) eCount++;
        fHeCount->Fill(MCShowerHandle->at(i).PdgCode());
	}

    if(MCShowerHandle->size() != 0) fHmcE->Fill(MCShowerHandle->at(0).DetProfile().E());
    if(ShowerHandle->size() != 0) fH3dE->Fill(ShowerHandle->at(0).Energy();
	
	// for(unsigned int j=0; j<VertexHandle->size(); j++){

	// 	fVertex->Fill(VertexHandle->at(j).X(), VertexHandle->at(j).Y());
	// }
 
    return true;
  }

  bool showerE::finalize() {

    //
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

	// fShowerHist->Write();
	// fVertex->Write();
    fHeCount->Write();
    fHmcE->Write();
    fH3dE->Write();
 
    return true;
  }

}
#endif
