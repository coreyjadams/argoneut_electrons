#ifndef LARLITE_REMOVETRACKHITS_CXX
#define LARLITE_REMOVETRACKHITS_CXX

#include <list>
#include <algorithm>

#include "RemoveTrackHits.h"
#include "DataFormat/hit.h"
#include "DataFormat/cluster.h"
#include "DataFormat/track.h"

namespace argomerge {

bool RemoveTrackHits::initialize() {

  //
  // This function is called in the beginning of event loop
  // Do all variable initialization you wish to do here.
  // If you have a histogram to fill in the event loop, for example,
  // here is a good place to create one on the heap (i.e. "new TH1D").
  //
  // _input_producer = "";
  return true;
}

bool RemoveTrackHits::analyze(::larlite::storage_manager* storage) {

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

  // Get the tracks for this event:
  auto ev_track = storage->get_data<larlite::event_track>(_track_producer);

  storage -> set_id(1, 0, ev_track->event_id());
  // Holder for new clusters that are not tracks:
  auto out_cluster_v = storage->get_data<larlite::event_cluster>(_output_producer);
  auto ev_ass = storage->get_data<larlite::event_ass>(out_cluster_v->name());

  // Get all of the clusters from this event:
  ::larlite::event_cluster * ev_clus = storage->get_data<larlite::event_cluster>(_cluster_producer);

  // Get the associated clusters to this track:
  ::larlite::event_cluster * ev_clus2 = nullptr;
  auto & clust_index_v = storage->find_one_ass(ev_track->id(), ev_clus2, _track_producer);
  if (!ev_clus) {
    // std::cout << "Bailing because no cluster pointer.\n";
    return false;
  }
  if (!ev_clus -> size()) {
    // std::cout << "Bailing because no clusters\n";
    return false;
  }
  // std::cout << "clust_index_v.size() " << clust_index_v.size() << std::endl;
  // std::cout << "ev_clus->size() " << ev_clus->size() << std::endl;

  // Get all of the hits from this event:
  // ::larlite::event_hit* ev_hit = nullptr;
  // auto & hit_index_v = storage->find_one_ass(ev_track->id(), ev_hit, _track_producer);
  // if (!ev_hit) {
  //   std::cout << "Bailing because hit pointer.\n";
  //   return false;
  // }
  // if (!ev_hit -> size()){
  //   std::cout << "Bailing because no hits.\n";
  //   return false;
  // }
  // std::cout << "hit_index_v.size() " << hit_index_v.size() << std::endl;
  // std::cout << "ev_hit->size() " << ev_hit->size() << std::endl;
  ::larlite::event_hit* ev_hit_2 = nullptr;
  auto & hit_index_v_from_clusts = storage->find_one_ass(ev_clus->id(), ev_hit_2, ev_clus->name());
  if (!ev_hit_2) {
    // std::cout << "Bailing because hit pointer 2.\n";
    return false;
  }
  if (!ev_hit_2 -> size()) {
    // std::cout << "Bailing because no hits 2.\n";
    return false;
  }
  // Verify there are clusters
  // auto ev_clus = storage->get_data<larlite::event_cluster>(_input_producer);



  // We need to know how many hits are clustered in this event...
  // Get the association between the clusters and the hits so we
  // can sum up the number of hits

  // std::cout << "hit_index_v_from_clusts.size() " << hit_index_v_from_clusts.size() << std::endl;

  // In the end, this algorithm will remove tracks from a set of hits/clusters.
  // The goal is to keep the original clusters without the ones associated to tracks.
  // So, we will make new clusters and keep their associations to hits (and, if they
  // have it, tracks).
  //

  std::vector<size_t> bad_cluster_index;




  // auto geom = larutil::Geometry::GetME();

  // std::cout << "Detector Limits: \n"
  //           << "\tX: " << -geom -> DetHalfWidth() << " to "
  //           << geom -> DetHalfWidth() << "\n"
  //           << "\tY: " << -geom -> DetHalfHeight() << " to "
  //           << geom -> DetHalfHeight() << "\n"
  //           << "\tZ:  " << 0 << " to "
  //           << geom -> DetLength() << "\n";


  // // Now, look at each each cluster made by the track algorithm and determine whether
  // // or not to filter out the hits and clusters associated with it
  for (size_t i_track = 0; i_track < ev_track -> size(); i_track ++ ) {
    // This track is accessible as ev_track -> at(i_track)
    // The cluster(s) associated with this track are accessible as
    // a list of indexes.  Get the indexes as clust_index_v.at(i_track),
    // and use the indexes in ev_clus->at(index_from_clus_index_v.at(i_track))
    // Similarly, hits associated with this track (which are the hits associated
    // with the cluster, I believe) are accessible as
    // ev_hit->at(index_from_hit_index_v.at(i_track))

    // // Get the clusters associated with this track:
    auto & this_clust_set = clust_index_v.at(i_track);

    // // For each cluster, get

    // // Get the hits associated with this track:
    // auto & this_hit_set = hit_index_v.at(i_track);

    // std::cout << "this_clust_set.size() " << this_clust_set.size() << std::endl;
    // std::cout << "this_hit_set.size() " << this_hit_set.size() << std::endl;

    // // Now, Look at this track in detail.

    // // Check the rms of the hits associated for this track
    // std::vector<float> rmsByPlane;
    // // rmsByPlane.resize(geom -> Nviews());
    // if (this_clust_set.size() != this_hit_set.size()){
    //   std::cerr << "Error, wrong number of associations\n";
    //   exit(-1);
    // }
    // else{
    //   // for (size_t i_clust = 0; i_clust < this_clust_set.size(); i_clust ++){

    //   // }
    // }

    // Check how close this track is to the edges.
    //
    // If both start and end are outside of a fiducial volume, reject it.

    // std::cout << "\tTrack Start: ("
    //           << ev_track -> at(i_track).Vertex().X() << ", "
    //           << ev_track -> at(i_track).Vertex().Y() << ", "
    //           << ev_track -> at(i_track).Vertex().Z()
    //           << ")\n\tTrack end: ("
    //           << ev_track -> at(i_track).End().X() << ", "
    //           << ev_track -> at(i_track).End().Y() << ", "
    //           << ev_track -> at(i_track).End().Z() << ")"
    //           << std::endl;
    if ( ! isFiducial(ev_track -> at(i_track).Vertex() ) ) {
      if ( ! isFiducial(ev_track -> at(i_track).End() ) ) {

        // This track appears to be crossing.
        // Verify that it's complete (uses most of the hits from
        // the clusters associated to it.)
        // Check for each plane
        //
        bool complete = true;
        for (auto clust_index : clust_index_v.at(i_track)) {
          // Exit this
          if ( ! isComplete(ev_track->at(i_track), ev_clus -> at(clust_index))) {
            complete = false;
            // std::cout << "Incomplete, kept\n";
          }
        }

        // std::cout << "\t\trejected!\n";
        if (complete) {
          for (auto index : this_clust_set) {
            bad_cluster_index.push_back(index);
            // std::cout << "Rejected\n";
          }
        }
      }
    }



  }

  // Just to test the framework: make new clusters (with associated hits) without
  // all of the clusters/hits that are associated to tracks:

  // Loop Over the clusters and keep the ones with multiple hits
  // in the new set of clusters and associations

  ::larlite::AssSet_t hit_ass;
  // auto ass_info = ev_clus->association(ev_hit->id());



  for (size_t i = 0; i < ev_clus -> size(); i++) {

    // // Check if this cluster is associated to a track
    // bool skip_this_cluster = false;
    // for (auto & vec : clust_index_v){
    //   auto it = std::find(vec.begin(), vec.end(), i);
    //   if (it != vec.end())
    //     skip_this_cluster = true;
    // }
    // if (skip_this_cluster)
    //   continue;
    auto it = std::find(bad_cluster_index.begin(), bad_cluster_index.end(), i);
    if (it != bad_cluster_index.end()) {
      continue;
    }
    else {
      // save this cluster and keep it in the new set of clusters
      hit_ass.push_back(hit_index_v_from_clusts.at(i));
      out_cluster_v -> push_back(ev_clus -> at(i));
      if ( ! out_cluster_v -> back().IsMergedCluster()) {
        out_cluster_v -> back().set_is_merged(true);
        out_cluster_v -> back().set_original_producer(_cluster_producer);
        std::vector<unsigned short> orig_index(1, i);
        out_cluster_v -> back().set_original_indexes(orig_index);
      }

    }

  }
  ev_ass->set_association(out_cluster_v->id(), ev_hit_2->id(), hit_ass);
  // storage -> set_id(1, 0, ev_track->event_id());

  return true;
}

bool RemoveTrackHits::isFiducial(const TVector3 & point) {
  float cut_x = 2.0;
  float cut_y = 2.0;
  float cut_z = 2.0;
  auto geom = larutil::Geometry::GetME();

  // std::cout << "Fiducial limits: \n"
  //           << "X: " << - geom -> DetHalfWidth() + cut_x << " to "
  //           <<  geom -> DetHalfWidth() + cut_x << "\n"
  //           << "Y: " << - geom -> DetHalfHeight() + cut_y << " to "
  //           << geom -> DetHalfHeight() - cut_y << "\n"
  //           << "Z: " << cut_z << " to "
  //           << geom -> DetLength() - cut_z << "\n";

  if (point.X() <  + cut_x) {
    // std::cout << "Rejected, x too small\n";
    return false;
  }
  if (point.X() > 2 * geom -> DetHalfWidth() - cut_x) {
    // std::cout << "Rejected, x too big\n";
    return false;
  }
  if (point.Y() < - geom -> DetHalfHeight() + cut_y) {
    // std::cout << "Rejected, y too small\n";
    return false;
  }
  if (point.Y() > geom -> DetHalfHeight() - cut_y) {
    // std::cout << "Rejected, y too big\n";
    return false;
  }
  if (point.Z() < cut_z) {
    // std::cout << "Rejected, z too small\n";
    return false;
  }
  if (point.Z() > geom -> DetLength() - cut_z) {
    // std::cout << "Rejected, z too big\n";
    return false;
  }

  return true;
}

bool RemoveTrackHits::isComplete(const ::larlite::track & _track, const ::larlite::cluster & _cluster) {
  // How many points does this track have compared to the number of hits?
  // std::cout << "_track.NumberTrajectoryPoints()" << _track.NumberTrajectoryPoints() << std::endl;
  // std::cout << "_cluster.NHits()" << _cluster.NHits() << std::endl;
  if ((float)_track.NumberTrajectoryPoints() / _cluster.NHits() > 0.5)
    return true;
  else
    return false;

}



bool RemoveTrackHits::finalize() {

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
