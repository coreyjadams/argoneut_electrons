#ifndef ARGOUTIL_SINGLESENDPOINTCHEATER_CXX
#define ARGOUTIL_SINGLESENDPOINTCHEATER_CXX

#include "SinglesEndpointCheater.h"

#include "DataFormat/cluster.h"
#include "DataFormat/mctruth.h"
#include "DataFormat/mcshower.h"
#include "DataFormat/endpoint2d.h"
#include "DataFormat/hit.h"

#include "LArUtil/GeometryHelper.h"
#include "LArUtil/DetectorProperties.h"
#include "LArUtil/Geometry.h"

namespace argoutils {

bool SinglesEndpointCheater::initialize() {

    //
    // This function is called in the beggining of event loop
    // Do all variable initialization you wish to do here.
    // If you have a histogram to fill in the event loop, for example,
    // here is a good place to create one on the heap (i.e. "new TH1D").
    //

    return true;
}

bool SinglesEndpointCheater::analyze(larlite::storage_manager* storage) {


    // This class takes in the clusters and mcparticles, and makes an endpoint2d
    //
    // The endpoint2d is the cheated endpoint2d projected from the 3D vertex

    // Get the mctruth:
    auto * ev_truth = storage -> get_data<larlite::event_mctruth>("generator");

    auto * ev_mcshower = storage -> get_data<larlite::event_mcshower>("mcinfo");

    // Get the clusters:
    auto * ev_clust = storage -> get_data<larlite::event_cluster>("mergeall");

    // Get the hits associated with the clusters:
    larlite::event_hit * ev_hit = nullptr;
    auto const& hit_index_v = storage->find_one_ass(ev_clust->id(), ev_hit, ev_clust->name());

    auto * ev_endpoint = storage -> get_data<larlite::event_endpoint2d>("cheater");

    auto ev_ass = storage->get_data<larlite::event_ass>(ev_clust->name());

    // set event ID through storage manager
    storage->set_id(ev_truth->run(),
                    ev_truth->subrun(),
                    ev_truth->event_id());

    auto geoHelper = larutil::GeometryHelper::GetME();
    auto detProp = larutil::DetectorProperties::GetME();
    auto geom = larutil::Geometry::GetME();

    // Need the biggest cluster in each plane to associate the endpoint too
    // Assume at least 50 hits per cluster
    std::vector<int> biggest_clust_index;
    std::vector<int> biggest_clust_size;

    biggest_clust_index.resize(geom->Nplanes());
    biggest_clust_size.resize(geom->Nplanes());

    for (size_t clust_index = 0; clust_index < ev_clust -> size(); clust_index ++) {
        int n_hits = hit_index_v.at(clust_index).size();
        int plane = ev_hit->at(hit_index_v.at(clust_index).front()).WireID().Plane;
        if (n_hits > biggest_clust_size[plane]) {
            biggest_clust_size[plane] = n_hits;
            biggest_clust_index[plane] = clust_index;
        }
    }

    // Loop over the mctruth info:
    // std::cout << "This event has " << ev_mcshower -> size() << " mc shower objects." << std::endl;


    larlite::AssSet_t clust_endp_ass;


    auto shower = ev_mcshower->front();

    // std::cout << "This truth has " << truth.NParticles() << " particles." << std::endl;
    // std::cout << "Position[0] is ("
    //           << shower.DetProfile().Position().X()  << ", "
    //           << shower.DetProfile().Position().Y()  << ", "
    //           << shower.DetProfile().Position().Z()  << ", "
    //           << ")\n";

    // std::cout << std::endl;
    // std::cout << "Event " << ev_truth->event_id() << std::endl;
    // std::cout << "shower.DaughterTrackID().size(): " << shower.DaughterTrackID().size() << std::endl;


    if (shower.DaughterTrackID().size() > 50) {
        return false;
    }


    TVector3 vertex = shower.DetProfile().Position().Vect();

    if (! geoHelper -> Point_isInTPC(vertex)) {
        return false;
    }

    // Get the position in each plane:
    for (size_t plane = 0; plane < geom -> Nplanes(); plane ++) {
        try {
            auto point2D = geoHelper->Point_3Dto2D(vertex, plane);
        }
        catch (...) {
            return false;
        }
        // std::cout << "  Plane " << plane << ", vertex at ("
        //           << point2D.w  << ", "
        //           << point2D.t << ")\n";
        int wire = 0;
        try {
            wire = geom -> NearestWire(vertex, plane);
        }
        catch (...) {
            return false;
        }
        double drifttime = detProp->ConvertXToTicks(vertex.X(), plane);
        ev_endpoint->emplace_back(larlite::endpoint2d( drifttime,
                                  wire,
                                  1.0,
                                  0,
                                  larlite::geo::View_t(plane),
                                  0.0));
        clust_endp_ass.push_back(std::vector<unsigned int>(1, biggest_clust_index[plane]) );

    }


    ev_ass->set_association(ev_clust->id(), ev_endpoint->id(), clust_endp_ass);

    return true;
}

bool SinglesEndpointCheater::finalize() {

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
