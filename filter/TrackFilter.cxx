#ifndef LARLITE_TRACKFILTER_CXX
#define LARLITE_TRACKFILTER_CXX

#include "TrackFilter.h"

#include "DataFormat/track.h"

namespace argofilter {

bool TrackFilter::initialize() {

    //
    // This function is called in the beggining of event loop
    // Do all variable initialization you wish to do here.
    // If you have a histogram to fill in the event loop, for example,
    // here is a good place to create one on the heap (i.e. "new TH1D").
    //

    _n_tracks_hist = new TH1D("_n_tracks_hist","Number of Tracks",20,0,20);

    return true;
}

bool TrackFilter::analyze(larlite::storage_manager* storage) {

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


    // Get all of the tracks from the event
    larlite::event_track * ev_track
        = storage -> get_data<larlite::event_track>(_track_producer);

    // Get all of the clusters:
    larlite::event_cluster * ev_cluster
        = storage -> get_data<larlite::event_cluster>(_cluster_producer);


    // Do analysis to determine to keep or reject this track:
    

    // Fill any analysis histograms:
    _n_tracks_hist -> Fill(ev_track -> size());


    // Keep this event:
    bool keep = true;
    if (keep){
        return true;
    }
    else{
        return false;
    }

}

bool TrackFilter::finalize() {

    // This function is called at the end of event loop.
    // Do all variable finalization you wish to do here.
    // If you need, you can store your ROOT class instance in the output
    // file. You have an access to the output file through "_fout" pointer.
    //
    // Say you made a histogram pointer h1 to store. You can do this:
    //
    if(_fout) { 
        _fout->cd(); 
        _n_tracks_hist->Write(); 
    }
    
    else
      print(larlite::msg::kERROR,__FUNCTION__,"Did not find an output file pointer!!! File not opened?");
    

    return true;
}

}
#endif
