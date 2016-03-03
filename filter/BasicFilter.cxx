#ifndef ARGOFILTER_BASICFILTER_CXX
#define ARGOFILTER_BASICFILTER_CXX

#include "BasicFilter.h"

namespace argofilter {

bool BasicFilter::initialize() {

  //
  // This function is called in the beggining of event loop
  // Do all variable initialization you wish to do here.
  // If you have a histogram to fill in the event loop, for example,
  // here is a good place to create one on the heap (i.e. "new TH1D").
  //

  // _input_producer = "ccMergedFinal";

  return true;
}

bool BasicFilter::analyze(::larlite::storage_manager* storage) {

  std::cout << "_input_producer is " << _input_producer << std::endl;


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


  // Do the filtering here.  Look at things like the number of clusters,
  // the average size of each cluster, and the number of unclustered hits.

  // Only looking at the collection plane since induction noise is an issue
  // 
  
  int _n_clusters = 0;
  int _n_hits = 0;
  int _n_hits_clustered = 0;

  for (size_t i_clust = 0; i_clust < ev_clus -> size(); i_clust ++){
    // Skip not collection plane
    if (ev_clus -> at(i_clust).Plane().Plane != 1){
      continue;
    }

    _n_clusters ++;
    _n_hits_clustered += ev_clus -> at(i_clust).NHits();

  }

  for (auto & hit : * ev_hit){
    if (hit.SignalType() == ::larlite::geo::kCollection){
      _n_hits ++;
    }
  }

  // std::cout << "_n_hits: " << _n_hits << std::endl;
  // std::cout << "_n_hits_clustered: " << _n_hits_clustered << std::endl;
  // std::cout << "_n_clusters: " << _n_clusters << std::endl;

  // std::cout << "Percent of hits clustered: " << 1.0*_n_hits_clustered / _n_hits << std::endl;

  // Some requirements for this filter:
  // At least 50 hits total. (Rejects largely empty events)
  // No more than 85% of the hits are already clustered (rejects tracks only)
  // No less than 4 clusters
  // 
  // maybe, require some small clusters?

  if (_n_hits < 50){
    return false;
  }
  if (1.0*_n_hits_clustered/_n_hits_clustered > .85){
    return false;
  }

  
  return true;
}

bool BasicFilter::finalize() {

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
