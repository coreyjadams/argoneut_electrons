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

  // Initialize the neural network for the track/shower separation
  std::string path = getenv("LARLITE_USERDEVDIR");
  path.append("/argoneut_electrons/utils/fann_training/trackShowerAnn.dat");
  ts.setFannFileName(path);
  //ts.setFannFileName("/uboone/app/users/npereira/larlite/UserDev/argoneut_electrons/utils/fann_training/trackShowerAnn.dat") ;

  // ts.setFannFileName("/Users/ah673/WorkArea/Root6LArLite/UserDev/Argoneut/utils/fann_training/trackShowerAnn.dat") ;
  ts.init();

  _input_producer = "ccMergedFinal";
  _min_hits = 100;

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


  // Holder for new clusters:
  auto out_cluster_v = storage->get_data<larlite::event_cluster>(_output_producer);
  auto out_ass = storage->get_data<larlite::event_ass>(out_cluster_v->name());


  // set event ID through storage manager
  storage->set_id(storage->get_data<larlite::event_cluster>(_input_producer)->run(),
                  storage->get_data<larlite::event_cluster>(_input_producer)->subrun(),
                  storage->get_data<larlite::event_cluster>(_input_producer)->event_id());



  // Get all of the clusters from this event:
  auto ev_clus = storage->get_data<larlite::event_cluster>(_input_producer);
  if (!ev_clus) {
    std::cout << "ev_clus is == 0, returning.\n";
    return false;
  }


  ::larlite::event_hit* ev_hit = nullptr;
  auto const& hit_index_v = storage->find_one_ass(ev_clus->id(), ev_hit, _input_producer);


  if (!ev_hit) {
    std::cout << "Did not find hit data product"
              << "!" << std::endl;
    return false;
  }


  if (!hit_index_v.size())
    return false;

  // Loop over the clusters and look for showers
  //  Just look at collection plane for this pass
  //  Require that it pass the FANN cut, and require that there is a nice straight track at the front


  std::vector<::cluster::cluster_params> params_vec;

  _cru_helper.GenerateParams(storage, _input_producer, params_vec);


  larlite::AssSet_t hit_ass;

  size_t index = 0;
  for (auto & clust : params_vec) {

    // skip non collection clusters
    if (clust.plane_id.Plane != 1){
      index ++;
      continue;
    }

    // Skip clusters that are too small:
    if (clust.hit_vector.size() < _min_hits){
      index ++;
      continue;
    }

    _params_alg.FillParams(clust);

    if (ts.trackOrShower(clust) == argoutils::TrackShower::kShower ) {
      // std::cout << "Event " << storage -> event_id() << "\n";
      // clust.PrintFANNVector();
      // clust.Report();

      // if (clust.multi_hit_wires / clust.N_Wires < 0.1)
      out_cluster_v->push_back(ev_clus->at(index));
      hit_ass.push_back(hit_index_v.at(index));
    }
    index ++;

  }
  out_ass->set_association(out_cluster_v->id(), ev_hit->id(), hit_ass);


  if (out_cluster_v->size() == 0) {
    return false;
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
