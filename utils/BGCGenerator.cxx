#ifndef LARLITE_BGCGENERATOR_CXX
#define LARLITE_BGCGENERATOR_CXX

#include "BGCGenerator.h"
#include "LArUtil/Geometry.h"
#include "LArUtil/GeometryHelper.h"

namespace argoutils {

void BGCGenerator::setBGCFile(std::string s) {

  _bfr.setInputFile("/data_linux/dedx_files/ScanFilejasaadi_neutrino.root");
  _bfr.readData();
  std::cout << _clusterData.size() << std::endl;
  _bfr.setInputFile("/data_linux/dedx_files/ScanFilejasaadi_antineutrino.root");
  _bfr.readData();
  std::cout << _clusterData.size() << std::endl;
  _bfr.setInputFile("/data_linux/dedx_files/ScanFileandrzejs.root");
  _bfr.readData();
  std::cout << _clusterData.size() << std::endl;
  _clusterData = _bfr.getData();
  std::cout << _clusterData.size() << std::endl;
}


bool BGCGenerator::initialize() {

  //
  // This function is called in the beggining of event loop
  // Do all variable initialization you wish to do here.
  // If you have a histogram to fill in the event loop, for example,
  // here is a good place to create one on the heap (i.e. "new TH1D").
  //

  return true;
}

bool BGCGenerator::analyze(larlite::storage_manager* storage) {

  // get all the hits
  auto ev_hit = storage->get_data<larlite::event_hit>("gaushit");


  // Going to make new clusters here based off
  // of input from the bootleg graph cluster


  auto out_cluster_v = storage->get_data<larlite::event_cluster>("bootleg");
  // auto out_pfpart_v = storage->get_data<larlite::event_pfpart>("bootleg");
  auto out_endpoint2d_v = storage->get_data<larlite::event_endpoint2d>("bootleg");
  auto ev_ass = storage->get_data<larlite::event_ass>(out_cluster_v->name());

  // std::cout << "Initial id is " << out_cluster_v -> event_id() << std::endl;

  // set event ID through storage manager
  storage->set_id(ev_hit->run(),
                  ev_hit->subrun(),
                  ev_hit->event_id());


  int run = ev_hit->run();
  int event = ev_hit->event_id();
  larlite::AssSet_t hit_ass;
  larlite::AssSet_t clust_endp_ass;

  // Now, check to see if this event is actually in the data from BGC:
  if (_clusterData.find( run ) != _clusterData.end() ) {
    // In this case, it has an entry for this run.
    if (_clusterData[run].find(event) != _clusterData[run].end()) {
      // in this case, it has this run AND this event.
      // Do the cluster building:
      auto & bg_clusters = _clusterData[run][event];
      // std::cout << "This event has " << bg_clusters.size()
      //           << " clusters." << std::endl;

      // Now go through the hit information and find the real hits
      // that best match:
      size_t i = 0;
      for (auto & bgclust : bg_clusters) {
        std::vector<unsigned int> hit_indexes = buildCluster(bgclust, ev_hit);
        // for (size_t i = 0; i < bgclust.hitwire.size(); i ++) {
        //   std::cout << "Matched the bgc hit at (" << bgclust.hitwire.at(i)
        //             << ", " << bgclust.hittime.at(i) << ") "
        //             << " to the larsoft hit at ("
        //             << ev_hit->at(hit_indexes.at(i)).WireID().Wire
        //             << ", " << ev_hit->at(hit_indexes.at(i)).PeakTime() << ")" << std::endl;
        // }
        // Build and endpoint 2d too
        out_endpoint2d_v->push_back(larlite::endpoint2d(
                                      bgclust.starthit[1], // double    driftTime,
                                      bgclust.starthit[0], // unsigned int      wire,
                                      1.0, // double    strength,
                                      i, // int       id,
                                      larlite::geo::View_t(bgclust.currplane), // geo::View_t view,
                                      0.0// double    totalQ
                                    ) );
        out_cluster_v->push_back(larlite::cluster());
        hit_ass.push_back(hit_indexes);
        clust_endp_ass.push_back(std::vector<unsigned int>(1, i) );
        i ++;

      }

      ev_ass->set_association(out_cluster_v->id(), ev_hit->id(), hit_ass);
      ev_ass->set_association(out_cluster_v->id(), out_endpoint2d_v->id(), clust_endp_ass);

    }
    else return false;
  }
  else return false;






  return true;
}

std::vector<unsigned int>  BGCGenerator::buildCluster(
  const BGCData &  bgclust,
  larlite::event_hit * ev_hit) {


  std::vector<unsigned int> returnVec;

  // For each hit in BGCluster, find the closest hit.
  // Require the same wire, and best match in time.
  // Oh and the same plane, obviously.

  // A lot of these bootleg clusters have one hit that is
  // really far in front of the rest.
  // So, keeping track of the wire of the 3 leftmost hits.
  // (and their indexes)
  // If the wire gap is really huge after all the hits are added,
  // drop that single hit.

  double lowest_wire = 239;
  double second_lowest_wire = 240;
  size_t lowest_wire_index, second_lowest_wire_index;

  for (size_t i = 0; i < bgclust.hitwire.size(); i++) {
    double _wire = bgclust.hitwire.at(i);
    double _time = bgclust.hittime.at(i);
    int plane = bgclust.currplane;
    // This is the part that finds the best hit:
    unsigned int index = 0;
    std::vector<unsigned int> matches;
    for (auto & hit : * ev_hit) {
      if (hit.WireID().Plane != plane) {
        index++;
        continue;
      }
      if (hit.WireID().Wire == _wire) {
        // Check that the start and end of this hit is
        // at least containing the _time
        if (_time > hit.StartTick() && _time < hit.EndTick()) {
          // Also make sure it's within the RMS of the hit
          if ( fabs(hit.PeakTime() - _time) < hit.RMS() )
            matches.push_back(index);
        }
      }
      index ++;
    }

    // std::cout << "Got " << matches.size() << " candidates" << std::endl;
    if (matches.size() == 0 )
      continue;

    // Now check all the plausible matches to see which has the best time.
    // If there is just one match, use it:
    if ( matches.size() == 1 ) {
      returnVec.push_back(matches.at(0));
      if (ev_hit->at(matches.at(0)).WireID().Wire < lowest_wire) {
        second_lowest_wire = lowest_wire;
        lowest_wire = ev_hit->at(matches.at(0)).WireID().Wire;
        second_lowest_wire_index = lowest_wire_index;
        lowest_wire_index = matches.at(0);
      }
      continue;
    }
    else {
      // forced to make a choice between two or more
      float _closest_dist(999);
      unsigned int best_match;

      for (auto _match_index : matches) {
        // std::cout << "Comparing " << _time << " to "
        //           << ev_hit -> at(_match_index).PeakTime() << " +/- "
        //           << ev_hit -> at(_match_index).RMS()
        //           << " on wire " <<  ev_hit -> at(_match_index).WireID().Wire
        //           << std::endl;
        float dist = fabs(_time - ev_hit -> at(_match_index).PeakTime());
        if (dist < _closest_dist) {
          best_match = _match_index;
          _closest_dist = dist;
        }
      }
      returnVec.push_back(best_match);
      if (ev_hit->at(best_match).WireID().Wire < lowest_wire) {
        second_lowest_wire = lowest_wire;
        lowest_wire = ev_hit->at(best_match).WireID().Wire;
        second_lowest_wire_index = lowest_wire_index;
        lowest_wire_index = best_match;
      }
    }


  }

  // // Now check out the wire gap:
  // if (second_lowest_wire - lowest_wire > 10) {
  //   // Then remove the lowest wire's hit from the collection:
  //   returnVec.erase(std::remove(returnVec.begin(),
  //                               returnVec.end(),
  //                               lowest_wire_index),
  //                   returnVec.end());
  // }

  // std::cout << "Finished, returning " << returnVec.size() << " hits." << std::endl;

  return returnVec;

}


bool BGCGenerator::finalize() {

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
