#ifndef LARLITE_JOINTRACKS_CXX
#define LARLITE_JOINTRACKS_CXX

#include <list>

#include "JoinTracks.h"
#include "DataFormat/hit.h"
#include "DataFormat/cluster.h"
#include "LArUtil/Geometry.h"


namespace larlite {

bool JoinTracks::initialize() {

  //
  // This function is called in the beggining of event loop
  // Do all variable initialization you wish to do here.
  // If you have a histogram to fill in the event loop, for example,
  // here is a good place to create one on the heap (i.e. "new TH1D").
  //

  return true;
}

bool JoinTracks::analyze(storage_manager* storage) {

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
  auto out_cluster_v = storage->get_data<event_cluster>(_output_producer);
  auto out_ass = storage->get_data<event_ass>(out_cluster_v->name());
  // std::cout << "Initial id is " << out_cluster_v -> event_id() << std::endl;


  // set event ID through storage manager
  storage->set_id(storage->get_data<event_cluster>(_input_producer)->run(),
                  storage->get_data<event_cluster>(_input_producer)->subrun(),
                  storage->get_data<event_cluster>(_input_producer)->event_id());

  // Get all of the clusters from this event:
  auto ev_clus = storage->get_data<event_cluster>(_input_producer);
  if (!ev_clus) {
    std::cout << "ev_clus is == 0, returning.\n";
    return false;
  }

  // std::cout << "reset  id is " << out_cluster_v -> event_id() << std::endl;

  if (!ev_clus->size()) {

    print(msg::kWARNING, __FUNCTION__,
          Form("Skipping event %d since no cluster found...", ev_clus->event_id()));
    return false;
  }

  // Get the associations between the clusters and hits:
  event_hit* ev_hit = nullptr;
  auto const& hit_index_v = storage->find_one_ass(
                              ev_clus->id(),
                              ev_hit,
                              ev_clus->id().second );
  if (!ev_hit || ev_hit->size() < 1)
    return false;

  // For every cluster, compare to the others in the plane
  // Find their closest approach, and see if they have compatible slopes.
  //
  // Going to pay special attention to known gaps, and allow gaps up to N wires
  // if there are no hits on the wires in between.


  // Make a list of wires with hits on them:
  std::vector<std::vector<size_t> > _wiremap;
  auto geom = larutil::Geometry::GetME();
  _wiremap.resize(geom -> Nplanes());

  for (auto & hit : * ev_hit) {
    size_t plane = geom -> ChannelToPlane(hit.Channel());
    size_t wire = geom -> ChannelToWire(hit.Channel());
    _wiremap[plane][wire] ++;
  }

  // Now loop over the clusters, and try to see if any have gaps to bridge
  for (size_t i_clust = 0; i_clust < ev_clus->size(); i_clust ++) {
    for (size_t j_clust = i_clust + 1; j_clust < ev_clus->size(); j_clust ++) {

      // Skip if the clusters aren't on the same plane, of course:
      if (ev_clus->at(i_clust).Plane() != ev_clus->at(j_clust).Plane()) {
        continue;
      }

      // Figure out how close these clusters come to each other,
      // and the wires on either side of the closest approach
      size_t index_i, index_j;
      auto _n_wires_gap = getClosestApproach(ev_hit,
                                             hit_index_v[i_clust],
                                             hit_index_v[j_clust],
                                             index_i,
                                             index_j
                                            );
    }
  }

  return true;
}

size_t JoinTracks::getClosestApproach(larlite::event_hit * ev_hit,
                                      const std::vector<unsigned int> & hit_index_i,
                                      const std::vector<unsigned int> & hit_index_j,
                                      size_t & index_i,
                                      size_t & index_j
                                     ) {
  // First, determine the upper and lower wire bounds of each cluster
  // if they aren't [lower_i, upper_i, lower_j,upper_j] (or i <-> j)
  // then they are overlapping, and return -1

  auto geom = larutil::Geometry::GetME();

  unsigned int lower_i(geom->ChannelToWire(ev_hit->at(hit_index_i.front()).Channel()));
  unsigned int lower_j(geom->ChannelToWire(ev_hit->at(hit_index_j.front()).Channel()));
  unsigned int upper_i(geom->ChannelToWire(ev_hit->at(hit_index_i.back()).Channel()));
  unsigned int upper_j(geom->ChannelToWire(ev_hit->at(hit_index_i.back()).Channel()));

  for (auto index : hit_index_i) {
    unsigned int wire
      = geom->ChannelToWire(ev_hit->at(hit_index_i.at(index)).Channel());
    if (wire < lower_i) {
      lower_i = wire;
    }
    if (wire > upper_i) {
      upper_i = wire;
    }
  }

  for (auto index : hit_index_j) {
    unsigned int wire
      = geom->ChannelToWire(ev_hit->at(hit_index_j.at(index)).Channel());
    if (wire < lower_j) {
      lower_j = wire;
    }
    if (wire > upper_j) {
      upper_j = wire;
    }
  }

  // Now check against overlap:
  if (lower_j < lower_i) {
    if (upper_j >= lower_i)
      return -1;
  }
  else { // lower_i <= lower_j
    if (upper_i >= lower_j)
      return -1;
  }

  // At this point, the wires regions don't overlap;
  // Figure out how many wires apart they are:
  unsigned int _wire_gap(0);
  if (lower_j < lower_i) {
    _wire_gap = lower_i - upper_j;
  }
  else { // lower_i <= lower_j
    _wire_gap = lower_j - upper_i;
  }

  // if (_wire_gap ){}

  return -1;

}


bool JoinTracks::finalize() {

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

  // std::cout << "Finalized!!\n\n\n\n";

  return true;
}

}
#endif
