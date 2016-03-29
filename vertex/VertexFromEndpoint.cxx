#ifndef LARLITE_VERTEXFROMENDPOINT_CXX
#define LARLITE_VERTEXFROMENDPOINT_CXX

#include "VertexFromEndpoint.h"
#include "LArUtil/Geometry.h"
#include "LArUtil/GeometryHelper.h"
#include "LArUtil/DetectorProperties.h"
#include "LArUtil/LArProperties.h"


namespace argovertex {

bool VertexFromEndpoint::initialize() {

    //
    // This function is called in the beggining of event loop
    // Do all variable initialization you wish to do here.
    // If you have a histogram to fill in the event loop, for example,
    // here is a good place to create one on the heap (i.e. "new TH1D").
    //
    auto geo = larutil::Geometry::GetME();
    double * origin;
    origin = new double[geo->Nviews()];
    int plane = 0;
    geo->PlaneOriginVtx(plane, origin);
    ind_plane_offset = origin[0];
    plane = 1;
    geo->PlaneOriginVtx(plane, origin);
    coll_plane_offset = origin[0];
    delete origin;

    return true;
}

bool VertexFromEndpoint::analyze(larlite::storage_manager* storage) {

    // This is pretty straight forward.
    //
    // Get the pfparticles, and the clusters associated to them
    // Get the endpoints that match the clusters.
    // Make a vertex out of the endpoints, and associate it
    // to the pfparticle

    // Space for output vertexes
    auto out_vertex_v = storage->get_data<larlite::event_vertex>("bootlegVertex");
    auto out_ass = storage->get_data<larlite::event_ass>(out_vertex_v->name());

    // Get the pfparticles
    auto ev_pfpart = storage -> get_data<larlite::event_pfpart>("bootlegMatched");
    larlite::event_cluster * ev_clus = nullptr;
    auto clus_ass = storage -> find_one_ass(ev_pfpart->id(), ev_clus, ev_pfpart->name());

    // Get the endpoint2ds:
    larlite::event_endpoint2d * ev_endpoint = nullptr;
    auto endpoint_ass = storage -> find_one_ass(ev_clus->id(), ev_endpoint, ev_clus->name());

    // Get the hits associated with the clusters too
    larlite::event_hit * ev_hit = nullptr;
    auto hit_ass = storage -> find_one_ass(ev_clus->id(), ev_hit, ev_clus->name());


    // set event ID through storage manager
    storage->set_id(ev_hit->run(),
                    ev_hit->subrun(),
                    ev_hit->event_id());


    auto detProp = larutil::DetectorProperties::GetME();

    larlite::AssSet_t vertex_ass;

    // Loop over the particles to make vertexes
    for (size_t i_pfpart = 0; i_pfpart < ev_pfpart -> size(); i_pfpart ++) {


        std::vector<unsigned int > &  clust_indexes = clus_ass.at(i_pfpart);
        // for (int i = 0; i < clust_indexes.size(); ++i)
        // {
        //     std::cout << "clust index is " << clust_indexes[i] << std::endl;
        // }

        // Should be exactly two clusters
        if (clust_indexes.size() != 2) {
            std::cout << "Not exactly two clusters!" << std::endl;
            continue;
        }

        // need to know the collection cluster and induction cluster
        // as well as the end points for those clusters

        unsigned int coll_clust_index, ind_clust_index;
        coll_clust_index = clust_indexes.front();
        ind_clust_index = clust_indexes.back();

        // Check if it's right, and if it's not swap it:
        if (ev_hit->at(hit_ass[coll_clust_index].front()).WireID().Plane == 0 ) {
            std::swap(coll_clust_index, ind_clust_index);
        }

        // std::cout << "Coll index is " << coll_clust_index << std::endl;
        // std::cout << "Ind index is " << ind_clust_index << std::endl;

        // Because of the way the clusters were created, the endpoint 2d indexes
        // are 1-1 with the clusters

        // Get the endpoint2ds:
        larlite::endpoint2d & coll_endpoint2d = ev_endpoint->at(endpoint_ass.at(coll_clust_index)[0]);
        larlite::endpoint2d & ind_endpoint2d = ev_endpoint->at(endpoint_ass.at(ind_clust_index)[0]);

        // std::cout << "Coll endpoint plane is " << coll_endpoint2d.View()
        //           << " at time " << coll_endpoint2d.DriftTime()
        //           << std::endl;
        // std::cout << "Ind endpoint plane is " << ind_endpoint2d.View()
        //           << " at time " << ind_endpoint2d.DriftTime()
        //           << std::endl;

        // Make a vertex:

        double xyz[3];

        // // Use this method:
        // from geometry
        auto geom = larutil::Geometry::GetME();
        auto geomHelper = larutil::GeometryHelper::GetME();
        geom -> IntersectionPoint(ind_endpoint2d.Wire(),  coll_endpoint2d.Wire(),
                                  0,
                                  1,
                                  xyz[1], xyz[2]);

        xyz[0] = (ind_endpoint2d.DriftTime() - detProp ->TriggerOffset())
                 * geomHelper->TimeToCm()
                 + ind_plane_offset;
        xyz[0] += (coll_endpoint2d.DriftTime() - detProp ->TriggerOffset())
                  * geomHelper->TimeToCm()
                  + coll_plane_offset;

        xyz[0] *= 0.5;
        out_vertex_v->push_back(::larlite::vertex(xyz));
        // std::cout << "Vertex is (" << xyz[0]
        //           << ", " << xyz[1]
        //           << ", " << xyz[2]
        //           << ")\n";

        vertex_ass.push_back(std::vector<unsigned int>(1,i_pfpart));

    }

  out_ass->set_association(ev_pfpart->id(), out_vertex_v->id(), vertex_ass);

    return true;
}

bool VertexFromEndpoint::finalize() {

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
