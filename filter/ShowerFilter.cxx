#ifndef ARGOFILTER_SHOWERFILTER_CXX
#define ARGOFILTER_SHOWERFILTER_CXX

#include "ShowerFilter.h"

namespace argofilter {

bool ShowerFilter::initialize() {

  //
  // This function is called in the beggining of event loop
  // Do all variable initialization you wish to do here.
  // If you have a histogram to fill in the event loop, for example,
  // here is a good place to create one on the heap (i.e. "new TH1D").
  //

  return true;
}

bool ShowerFilter::analyze(::larlite::storage_manager* storage) {

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


  // // Get the clusters from the event
  // auto ev_clus = storage->get_data<event_cluster>(_input_producer);
  // if (!ev_clus->size()) {

  //     print(msg::kWARNING, __FUNCTION__,
  //           Form("Skipping event %d since no cluster found...", ev_clus->event_id()));
  //     return false;
  // }


  // Loop over the clusters and look for showers
  //  Just look at collection plane for this pass
  //  Require that it pass the FANN cut, and require that there is a nice straight track at the front


  std::vector<::cluster::cluster_params> params_vec;
  _cru_helper.GenerateParams(storage, _input_producer, params_vec);


  for (auto & clust : params_vec) {
    _params_alg.FillParams(clust);
    
  }

  return true;
}

bool ShowerFilter::finalize() {

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
