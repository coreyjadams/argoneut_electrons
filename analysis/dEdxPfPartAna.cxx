#ifndef LARLITE_DEDXPFPARTANA_CXX
#define LARLITE_DEDXPFPARTANA_CXX

#include "dEdxPfPartAna.h"
#include "LArUtil/GeometryHelper.h"
#include "LArUtil/DetectorProperties.h"
#include "LArUtil/LArProperties.h"
#include "TPrincipal.h"

namespace argoana {

dEdxPfPartAna::dEdxPfPartAna()
{ _name = "dEdxPfPartAna"; _fout = 0;}

bool dEdxPfPartAna::initialize() {

    //
    // This function is called in the beginning of event loop
    // Do all variable initialization you wish to do here.
    // If you have a histogram to fill in the event loop, for example,
    // here is a good place to create one on the heap (i.e. "new TH1D").
    //

    geom = larutil::Geometry::GetME();



    if (_fout) {

        _fout -> cd();
        tree = new TTree("dedx_ana", "dedx_ana");

        tree->Branch("run", &run);
        tree->Branch("event", &event);
        tree->Branch("axis3D", &fDCosStart);
        tree->Branch("displacement",&displacement);

        tree->Branch("c_hittimes", &collection_hittimes);
        tree->Branch("c_hitwires", &collection_hitwires);
        tree->Branch("c_hitcharges", &collection_hitcharges);
        tree->Branch("c_hitpeaks", &collection_hitpeaks);
        tree->Branch("c_starthit", &collection_starthit);
        tree->Branch("c_lifetime_corr", &collection_lifetime_corr);
        tree->Branch("c_dist_from_start", &collection_dist_from_start);
        tree->Branch("c_pitch", &collection_pitch);
        tree->Branch("c_pitch_err", &collection_pitch_err);
        tree->Branch("i_hittimes", &induction_hittimes);
        tree->Branch("i_hitwires", &induction_hitwires);
        tree->Branch("i_hitcharges", &induction_hitcharges);
        tree->Branch("i_hitpeaks", &induction_hitpeaks);
        tree->Branch("i_starthit", &induction_starthit);
        tree->Branch("i_lifetime_corr", &induction_lifetime_corr);
        tree->Branch("i_dist_from_start", &induction_dist_from_start);
        tree->Branch("i_pitch_err", &induction_pitch_err);
        tree->Branch("i_pitch", &induction_pitch);
    }

    return true;
}

bool dEdxPfPartAna::analyze(larlite::storage_manager* storage) {

    // Clear out all the data:


    // Get the endpoint2ds:
    // auto ev_endpoint = storage -> get_data<larlite::event_endpoint2d>("bootleg");


    // Get the pfparticles
    auto ev_pfpart = storage -> get_data<larlite::event_pfpart>("bootlegMatched");
    larlite::event_cluster * ev_clus = nullptr;
    auto clus_ass = storage -> find_one_ass(ev_pfpart->id(), ev_clus, ev_pfpart->name());

    auto ev_vertex = storage -> get_data<larlite::event_vertex>("bootlegVertex");

    // Get the showers associated with the clusters
    larlite::event_shower * ev_shower = nullptr;
    auto shower_ass = storage -> find_one_ass(ev_pfpart->id(),ev_shower,ev_pfpart->name());

    // larlite::event_vertex * ev_vertex = nullptr;
    // auto vertex_ass = storage -> find_one_ass(ev_pfpart->id(), ev_vertex, ev_pfpart->name());

    // Get the hits associated with the clusters too
    larlite::event_hit * ev_hit = nullptr;
    auto hit_ass = storage -> find_one_ass(ev_clus->id(), ev_hit, ev_clus->name());

    // Get the run and event number:
    run = ev_pfpart->run();
    event = ev_pfpart->event_id();


    auto geomHelper = larutil::GeometryHelper::GetME();
    auto LArProp = larutil::LArProperties::GetME();
    auto detprop = larutil::DetectorProperties::GetME();

    // std::cout << "Run " << run << " event " << event << "." << std::endl;

    // /// Generate: Params vector from event storage by specifying pfpart type
    // std::vector<cluster3D::cluster3D_params> _params3D_v;
    // try {
    //     _cru3D_helper.GenerateParams3D(storage,
    //                                    ev_pfpart->name(),
    //                                    _params3D_v);
    // }
    // catch ( ... ) {
    //     std::cout << "Caught exception on run " << run << ", event " << event << std::endl;
    //     return false;
    // }

    std::vector<cluster::cluster_params> _params_v;

    _cru_helper.GenerateParams(storage,
                               ev_clus->name(),
                               _params_v);

    auto geoHelper = larutil::GeometryHelper::GetME();

    for (size_t i_pfpart = 0; i_pfpart < ev_pfpart -> size(); i_pfpart ++) {

        // _params3D_alg.FillParams(_params3D_v.at(i_pfpart));


        collection_hittimes.clear();
        collection_hitwires.clear();
        collection_hitcharges.clear();
        collection_hitpeaks.clear();
        collection_starthit.clear();
        induction_hittimes.clear();
        induction_hitwires.clear();
        induction_hitcharges.clear();
        induction_hitpeaks.clear();
        induction_starthit.clear();
        fDCosStart.clear();
        collection_lifetime_corr.clear();
        induction_lifetime_corr.clear();
        collection_dist_from_start.clear();
        induction_dist_from_start.clear();
        collection_slope = 0.0;
        collection_pitch = 0.0;
        induction_slope = 0.0;
        induction_pitch = 0.0;
        displacement = 0;

        std::vector<unsigned int > &  clust_indexes = clus_ass.at(i_pfpart);

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

        // Fill the params of the clusters:
        _params_alg.FillParams(_params_v.at(coll_clust_index));
        _params_alg.FillParams(_params_v.at(ind_clust_index));

        // getClosestHits(_params_v.at(i_clust),close_hit_indexes,ev_vertex->at(i_pfpart));
        auto vertex = ev_vertex->at(i_pfpart);
        double xyz[3];
        vertex.XYZ(xyz);
        auto coll_start_point = geoHelper->Point_3Dto2D(xyz, _params_v.at(coll_clust_index).plane_id.Plane);
        Hit2D startingHit;
        startingHit.w = coll_start_point.w;
        startingHit.t = coll_start_point.t;

        float slope = coll_start_point.t - _params_v.at(coll_clust_index).mean_y;
        slope /= coll_start_point.w - _params_v.at(coll_clust_index).mean_x;

        Hit2D averagePoint;


        std::vector<unsigned int> coll_close_hit_indexes =
            geoHelper -> SelectLocalPointList( _params_v.at(coll_clust_index).hit_vector,
                                               startingHit,
                                               4.0,
                                               0.5,
                                               slope,
                                               averagePoint);

        coll_start_point = findClosestHit(_params_v.at(coll_clust_index).hit_vector,
                                          coll_close_hit_indexes,
                                          coll_start_point);

        // Now do the induction plane:

        auto ind_start_point = geoHelper->Point_3Dto2D(xyz, _params_v.at(ind_clust_index).plane_id.Plane);
        startingHit.w = ind_start_point.w;
        startingHit.t = ind_start_point.t;
        slope = ind_start_point.t - _params_v.at(ind_clust_index).mean_y;
        slope /= ind_start_point.w - _params_v.at(ind_clust_index).mean_x;


        // Here, get the N closest hits to the start point from the cluster (sorted):
        std::vector<unsigned int> ind_close_hit_indexes =
            geoHelper -> SelectLocalPointList( _params_v.at(ind_clust_index).hit_vector,
                                               startingHit,
                                               4.0,
                                               0.5,
                                               slope,
                                               averagePoint);

        // Find the closest hit to the start point, to use as the actual start point
        ind_start_point = findClosestHit(_params_v.at(ind_clust_index).hit_vector,
                                         ind_close_hit_indexes,
                                         ind_start_point);


        // Get the spacepoints associated with each particle, and use
        // their 3D axis for the 3D axis of the shower.

        // unwindVectors(ev_hit, ind_close_hit_indexes);
        // unwindVectors(ev_hit, coll_close_hit_indexes);

        // std::cout << "Run " << run << ", event " << event
        //           << ", start point in coll is (" <<  coll_start_point.w
        //           << ", " << coll_start_point.t << ") "
        //           << std::endl;


        // TVector3 startDir = _params3D_v.at(i_pfpart).principal_dir;

        // Get the direction from the shower too:
        auto shower = ev_shower -> at(shower_ass.at(i_pfpart).front());
        TVector3 startDir = shower.Direction();

        // Calculate the displacement:
        displacement = pow(xyz[0] - shower.ShowerStart().X(),2);
        displacement += pow(xyz[1] - shower.ShowerStart().Y(),2);
        displacement += pow(xyz[2] - shower.ShowerStart().Z(),2);
        displacement = sqrt(displacement);

        // std::cout << "Slope is " << slope << std::endl;
        double _tau = LArProp->ElectronLifetime();
        double timetick = detprop->SamplingRate() * 1.e-3;

        // Figure out the pitch:
        // induction_pitch = getPitch(shower.Direction(), plane);
        induction_starthit.push_back(ind_start_point.w / geoHelper -> WireToCm());
        induction_starthit.push_back(ind_start_point.t / geoHelper -> TimeToCm());
        // induction_slope = i_slope;
        // Fill in the hit information:
        for (auto & i : ind_close_hit_indexes) {
            size_t index = hit_ass.at(ind_clust_index).at(i);
            auto & hit = ev_hit->at(index);
            induction_lifetime_corr.push_back(exp(hit.PeakTime() * timetick / _tau));
            induction_hittimes.push_back(hit.PeakTime());
            induction_hitwires.push_back(hit.WireID().Wire);
            induction_hitcharges.push_back(hit.Integral());
            induction_hitpeaks.push_back(hit.PeakAmplitude());
        }
        // collection_pitch = getPitch(shower.Direction(), plane);
        collection_starthit.push_back(coll_start_point.w / geoHelper -> WireToCm());
        collection_starthit.push_back(coll_start_point.t / geoHelper -> TimeToCm());
        // collection_slope = c_slope;
        for (auto & i : coll_close_hit_indexes) {
            size_t index = hit_ass.at(coll_clust_index).at(i);
            auto & hit = ev_hit->at(index);
            collection_lifetime_corr.push_back(exp(hit.PeakTime() * timetick / _tau));
            collection_hittimes.push_back(hit.PeakTime());
            collection_hitwires.push_back(hit.WireID().Wire);
            collection_hitcharges.push_back(hit.Integral());
            collection_hitpeaks.push_back(hit.PeakAmplitude());
        }


        // Assign the pitch to collection and induction planes:

        induction_pitch = getPitch(startDir, 0);
        collection_pitch = getPitch(startDir, 1);
        // Compute the error on the pitch, which is generated from the uncertainty in
        // the 3D axis projection compared to the 2D slope.
        double induction_3D_slope = geomHelper->Slope_3Dto2D(startDir, 0);
        double collection_3D_slope = geomHelper->Slope_3Dto2D(startDir, 1);

        fDCosStart.resize(3);
        fDCosStart[0] = startDir.X();
        fDCosStart[1] = startDir.Y();
        fDCosStart[2] = startDir.Z();

        collection_pitch_err = fabs((collection_3D_slope - collection_slope) / collection_slope);
        induction_pitch_err = fabs((induction_3D_slope - induction_slope) / induction_slope);


        // std::cout << "Collection pitch is: " << collection_pitch << " +/- " << collection_pitch_err
        // << "\nInduction pitch is " << induction_pitch <<  " +/- " << induction_pitch_err << std::endl;

        tree -> Fill();

    }



    return true;
}

Point2D dEdxPfPartAna::findClosestHit(std::vector<Hit2D> hit_vector,
                                      std::vector<unsigned int> hit_indexes,
                                      Point2D start_point) {

    float shortest_dist = 999;
    size_t shortest_index = 0;
    for (auto & index : hit_indexes) {
        double dist = pow(hit_vector.at(index).w - start_point.w, 2);
        dist += pow(hit_vector.at(index).t - start_point.t, 2);
        // std::cout << "Dist is " << dist << std::endl;
        if (dist < shortest_dist) {
            shortest_dist = dist;
            shortest_index = index;
        }
    }

    Point2D returnP;
    returnP.w = hit_vector.at(shortest_index).w;
    returnP.t = hit_vector.at(shortest_index).t;
    return returnP;

}

void dEdxPfPartAna::getClosestHits(larlite::event_hit * ev_hit, std::vector<unsigned int>  hits_to_consider,
                                   std::vector<size_t> & close_hit_indexes,
                                   const larlite::endpoint2d & starthit) {

    // this function loops over the hits, and finds the hit closest to a target point.
    // When it's found the closest hit, it adds it to the list of indexes and moves the target
    // to the hit just found.
    //
    // Once a hit is in the list of indexes, it can't be considered any more of course.

    close_hit_indexes.clear();

    auto geomHelper = larutil::GeometryHelper::GetME();

    std::vector<double> target_point;
    target_point.push_back(starthit.Wire());
    target_point.push_back(starthit.DriftTime());

    unsigned int n_targethits = 15;

    if (hits_to_consider.size() < n_targethits) {
        n_targethits = hits_to_consider.size();
    }

    // std::cout << "target is (" << target_point[0] << ", " << target_point[1] << ")" << std::endl;

    while (close_hit_indexes.size() < n_targethits && close_hit_indexes.size() != hits_to_consider.size()) {

        unsigned int closest_index = -1;
        double shortest_dist = 999;
        double dist = 0;
        // std::cout << "hits_to_consider.size() is " << hits_to_consider.size() << std::endl;
        // for (auto & index : hits_to_consider)
        //     std::cout << index << " ";
        // std::cout << "close_hit_indexes.size() is " << close_hit_indexes.size() << std::endl;
        // for (auto & index : close_hit_indexes)
        //     std::cout << index << " ";


        for (auto & index : hits_to_consider ) {
            // std::cout << "Working on index " << index << std::endl;
            dist = pow((ev_hit->at(index).PeakTime() - target_point[1]) * geomHelper->TimeToCm(), 2);
            dist +=  pow((1.0 * ev_hit->at(index).WireID().Wire - target_point[0]) * geomHelper->WireToCm(), 2);
            // std::cout << "dist is " << dist << std::endl;
            if (dist < shortest_dist) {
                // Check that this hit isn't already used:
                bool used = false;
                for (auto & used_index : close_hit_indexes) {
                    if (index == used_index) {
                        used = true;
                        // std::cout << "hit is used" << std::endl;
                        break;
                    }
                }
                if (!used) {
                    closest_index = index;
                    shortest_dist = dist;
                }
            }
        }

        if (closest_index > 10000)
            return;
        // std::cout << "Finished and found ci == " << closest_index << std::endl;
        // Add the closest hit:
        close_hit_indexes.push_back(closest_index);
        target_point[0] = ev_hit->at(closest_index).WireID().Wire;
        target_point[1] = ev_hit->at(closest_index).PeakTime();
    }


    return;
}

void dEdxPfPartAna::unwindVectors(larlite::event_hit * ev_hit, std::vector<size_t> & close_hit_indexes) {

    // This function unwinds the vectors based on wire number so that they are,
    // at least, in an order.
    //
    // It compares the first and last hit and sorts in ascending or descending order accordingly

    // It runs right after finding the close hits, so that the actual sorted variables
    // are the close hit indexes.

    if (close_hit_indexes.size() == 0) {
        return;
    }

    // Compare the wire of the first and last hit.
    if (ev_hit -> at(close_hit_indexes.front()) < ev_hit -> at(close_hit_indexes.back())) {
        //Sort ascending
        // Don't need a complicated sorting algorithm here.
        bool sorted = false;
        while (! sorted) {
            bool broke = false;
            for (size_t i = 0; i < close_hit_indexes.size() - 1; i ++) {
                if (ev_hit->at(close_hit_indexes.at(i)).WireID().Wire
                        > ev_hit->at(close_hit_indexes.at(i + 1)).WireID().Wire )  {
                    unsigned int temp = close_hit_indexes.at(i + 1);
                    close_hit_indexes.at(i + 1) = close_hit_indexes.at(i);
                    close_hit_indexes.at(i) = temp;
                    broke = true;
                    break;
                }
            }
            if (! broke)
                sorted = true;
        }

    }

}

double dEdxPfPartAna::getPitch(const TVector3 & dir3D, int pl ) {

    double pitch = 0.4;


    double angleToVert = geom -> WireAngleToVertical((::larlite::geo::View_t) pl) - 0.5 * TMath::Pi();
    double cosgamma = TMath::Abs(TMath::Sin(angleToVert) * dir3D.Y() +
                                 TMath::Cos(angleToVert) * dir3D.Z());

    if (cosgamma > 0) pitch = pitch / cosgamma;

    return pitch;
}
bool dEdxPfPartAna::finalize() {

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

    if (_fout) {
        _fout -> cd();
        tree -> Write();
    }

    return true;
}

}
#endif
