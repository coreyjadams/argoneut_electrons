#ifndef LARLITE_DEDXSHOWERANA_CXX
#define LARLITE_DEDXSHOWERANA_CXX

#include "dEdxShowerAna.h"
#include "LArUtil/GeometryHelper.h"
#include "LArUtil/DetectorProperties.h"
#include "LArUtil/LArProperties.h"
#include "TPrincipal.h"

namespace larlite {

bool dEdxShowerAna::initialize() {

    //
    // This function is called in the beginning of event loop
    // Do all variable initialization you wish to do here.
    // If you have a histogram to fill in the event loop, for example,
    // here is a good place to create one on the heap (i.e. "new TH1D").
    //

    geom = larutil::Geometry::GetME();


    argoutils::BGCFileReader bgcreader;
    // bgcreader.setInputFile("/data_linux/dedx_files/ScanFileandrzejs.root");
    // bgcreader.setInputFile("/data_linux/dedx_files/ScanFilejasaadi_antineutrino.root");
    bgcreader.setInputFile("/data_linux/dedx_files/ScanFilejasaadi_neutrino.root");
    bgcreader.readData();

    bgcdata = bgcreader.getData();

    if (_fout) {

        _fout -> cd();
        tree = new TTree("dedx_ana", "dedx_ana");

        tree->Branch("run", &run);
        tree->Branch("event", &event);
        tree->Branch("axis3D", &fDCosStart);

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

bool dEdxShowerAna::analyze(storage_manager* storage) {


    // Clear out all the data:


    // Get the bootleg clusters
    auto ev_clus = storage -> get_data<larlite::event_cluster> ("bootleg");

    // if (ev_clus -> size() != 2) {
    //     std::cout << "Skipping event because there are " << ev_clus -> size() << " clusters." << std::endl;
    //     return false;
    // }

    // Get the hits associated with the clusters too
    larlite::event_hit * ev_hit = nullptr;
    auto hit_ass = storage -> find_one_ass(ev_clus->id(), ev_hit, ev_clus->name());

    // Get the run and event number:
    run = ev_clus->run();
    event = ev_clus->event_id();

    // Here's the plan:
    //
    // For events with more than two clusters, have to make the matches between clusters.
    // For events with exactly two clusters, use that pair as the match
    //      Store the matches in the same way so the rest of the module is the same
    //
    // Use the start point from bgcluster as the start point in each plane.
    // Populate a list of sorted hits by getting the one closest
    //   to the start point, then the one closest to that, then the one closest
    //   to that ... etc.  Cap it at 15ish?  10?
    // Need to get the axis of all the hits.
    //   If there are a lot of hits, use Cluster Params to get the principal direction
    //   If there aren't many hits, use TPrincipal on the list of selected hits.
    // Use Geometry helper to find the 3D axis that matches the two plane lines.
    // Calculate the pitch to use in each plane to dx calculations.
    // Save out the list of hits, pitch, charge, etc. for ttree based analysis.


    // Skip if this run and event aren't in the data set ...
    if (bgcdata.find(run) == bgcdata.end()) {
        std::cout << "Run " << run << " not found, bailing." << std::endl;
        return false;

    }
    else {
        if (bgcdata[run].find(event) == bgcdata[run].end()) {
            std::cout << "Event " << event << " not found, bailing." << std::endl;
            return false;
        }
    }

    // for (size_t i_shower = 0; i_shower < ev_shower ->size(); i_shower ++) {
    // auto shower = ev_shower->at(i_shower);
    // auto clust_indexes = clust_ass.at(i_shower);
    // Get the bootleg data for this event:
    auto _this_bgcdata = bgcdata[run][event];
    auto geomHelper = larutil::GeometryHelper::GetME();
    auto LArProp = larutil::LArProperties::GetME();
    auto detprop = larutil::DetectorProperties::GetME();

    // storing matches in pairs first=induction, second=collection
    // use a vector of matches to keep track of multi shower events
    // need matches for the cluster data as well as the bgcluster data
    std::vector<std::pair<int, int> > _bg_cluster_match_indexes;

    // Matching is done just based on bgc info.
    _bg_cluster_match_indexes = makeMatches(_this_bgcdata);

    // if no matches are found, exit
    if (_bg_cluster_match_indexes.size() == 0) {
        return false;
    }

    std::cout << "Run " << run << " event " << event << "." << std::endl;

    // Note: the file that makes larlite clusters makes them
    // in the order of the bootleg graph cluster files.

    // Now, loop over the matches and make output sets.

    for (auto & pair : _bg_cluster_match_indexes) {


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


        // Fill in the clusters:
        auto & ind_hit_set = hit_ass.at(pair.first);
        auto & coll_hit_set = hit_ass.at(pair.second);

        argoutils::BGCData ind_bgcluster = _this_bgcdata.at(pair.first);
        argoutils::BGCData coll_bgcluster = _this_bgcdata.at(pair.second);

        // Just verify that neither of these are junk:
        if (ind_bgcluster.starthit[0] == 0 || ind_bgcluster.starthit[1] == 0 ) {
            continue;
        }
        if (coll_bgcluster.starthit[0] == 0 || coll_bgcluster.starthit[1] == 0 ) {
            continue;
        }

        // Here, get the N closest hits to the start point from the cluster (sorted):
        std::vector<size_t> ind_close_hit_indexes;
        std::vector<size_t> coll_close_hit_indexes;
        // std::cout << "ev_hit -> size(): " << ev_hit -> size() << std::endl;
        // std::cout << "ind_hit_set.size(): " << ind_hit_set.size() << std::endl;
        // for (auto & index : ind_hit_set)
            // std::cout << "index: " << index << std::endl;
        // std::cout << "ind_close_hit_indexes.size(): " << ind_close_hit_indexes.size() << std::endl;
        getClosestHits(ev_hit, ind_hit_set, ind_close_hit_indexes, ind_bgcluster.starthit);
        getClosestHits(ev_hit, coll_hit_set, coll_close_hit_indexes, coll_bgcluster.starthit);

        // std::cout << "Found " << close_hit_indexes.size() << " hits." << std::endl;
        unwindVectors(ev_hit, ind_close_hit_indexes);
        unwindVectors(ev_hit, coll_close_hit_indexes);


        // Use TPrincipal to calculate the direction in the plane of this list of hits
        TPrincipal c_fPrincipal(2, "D");
        TPrincipal i_fPrincipal(2, "D");
        for (auto & index : coll_close_hit_indexes) {
            double data[2];
            data[0] = ev_hit->at(index).WireID().Wire * geomHelper -> WireToCm();
            data[1] = ev_hit->at(index).PeakTime() * geomHelper -> TimeToCm();
            c_fPrincipal.AddRow(data);
        }
        for (auto & index : ind_close_hit_indexes) {
            double data[2];
            data[0] = ev_hit->at(index).WireID().Wire * geomHelper -> WireToCm();
            data[1] = ev_hit->at(index).PeakTime() * geomHelper -> TimeToCm();
            i_fPrincipal.AddRow(data);
        }


        i_fPrincipal.MakePrincipals();
        c_fPrincipal.MakePrincipals();
        double i_slope = ((* i_fPrincipal.GetEigenVectors())[1][0]) / ((* i_fPrincipal.GetEigenVectors())[0][0]);
        double c_slope = ((* c_fPrincipal.GetEigenVectors())[1][0]) / ((* c_fPrincipal.GetEigenVectors())[0][0]);

        // std::cout << "Slope is " << slope << std::endl;
        double _tau = LArProp->ElectronLifetime();
        double timetick = detprop->SamplingRate() * 1.e-3;

        // Figure out the pitch:
        // induction_pitch = getPitch(shower.Direction(), plane);
        induction_starthit.push_back(ind_bgcluster.starthit.front());
        induction_starthit.push_back(ind_bgcluster.starthit.back());
        induction_slope = i_slope;
        // Fill in the hit information:
        for (auto & index : ind_close_hit_indexes) {
            auto & hit = ev_hit->at(index);
            induction_lifetime_corr.push_back(exp(hit.PeakTime() * timetick / _tau));
            induction_hittimes.push_back(hit.PeakTime());
            induction_hitwires.push_back(hit.WireID().Wire);
            induction_hitcharges.push_back(hit.Integral());
            induction_hitpeaks.push_back(hit.PeakAmplitude());
        }
        // collection_pitch = getPitch(shower.Direction(), plane);
        collection_starthit.push_back(coll_bgcluster.starthit.front());
        collection_starthit.push_back(coll_bgcluster.starthit.back());
        collection_slope = c_slope;
        for (auto & index : coll_close_hit_indexes) {
            auto & hit = ev_hit->at(index);
            collection_lifetime_corr.push_back(exp(hit.PeakTime() * timetick / _tau));
            collection_hittimes.push_back(hit.PeakTime());
            collection_hitwires.push_back(hit.WireID().Wire);
            collection_hitcharges.push_back(hit.Integral());
            collection_hitpeaks.push_back(hit.PeakAmplitude());
        }



        // What's left is to get the 3D Axis from the 2D slopes.
        double phi, theta;
        double induction_angle = atan(induction_slope);
        double collection_angle = atan(collection_slope);
        geomHelper->Get3DAxisN(0, 1,
                               induction_angle, collection_angle,
                               phi, theta);

        TVector3 startDir;
        fDCosStart.resize(3);
        fDCosStart[1] = sin(theta);
        fDCosStart[0] = cos(theta) * sin(phi);
        fDCosStart[2] = cos(theta) * cos(phi);
        startDir[1] = sin(theta);
        startDir[0] = cos(theta) * sin(phi);
        startDir[2] = cos(theta) * cos(phi);
        // std::cout << "Direction is found to be: ("
        //           <<  fDCosStart[0] << ", "
        //           <<  fDCosStart[1] << ", "
        //           <<  fDCosStart[2] << ") " << std::endl;

        // Assign the pitch to collection and induction planes:

        induction_pitch = getPitch(startDir, 0);
        collection_pitch = getPitch(startDir, 1);
        // Compute the error on the pitch, which is generated from the uncertainty in
        // the 3D axis projection compared to the 2D slope.
        double induction_3D_slope = geomHelper->Slope_3Dto2D(startDir, 0);
        double collection_3D_slope = geomHelper->Slope_3Dto2D(startDir, 1);

        collection_pitch_err = fabs((collection_3D_slope - collection_slope) / collection_slope);
        induction_pitch_err = fabs((induction_3D_slope - induction_slope) / induction_slope);


        // std::cout << "Collection pitch is: " << collection_pitch << " +/- " << collection_pitch_err
        // << "\nInduction pitch is " << induction_pitch <<  " +/- " << induction_pitch_err << std::endl;

        tree -> Fill();

    }



    return true;
}

std::vector<std::pair<int, int> > dEdxShowerAna::makeMatches(
    const std::vector<argoutils::BGCData> & bgcdata) {

    // This function will take the bgcluster data and make matches across planes.
    // If there are only two clusters, it's very easy
    //
    // If there are multiple clusters, match against the start point time
    //
    // If the start point times are close, use the ordering of start times to match.

    // matches are <induction, collection>
    std::vector<std::pair<int, int> > returnMatches;

    // If there are more than 6 clusters, just abandon hope.


    // Start by sorting into collection and induction clusters.
    std::vector<int> collection_indexes, induction_indexes;
    for (size_t i = 0; i < bgcdata.size(); i++) {
        if (bgcdata.at(i).currplane == 0) {
            induction_indexes.push_back(i);
        }
        if (bgcdata.at(i).currplane == 1) {
            collection_indexes.push_back(i);
        }
    }

    if (collection_indexes.size() != induction_indexes.size()) {
        // bail
        return returnMatches;
    }

    // If it's 1-1, easy:
    if (collection_indexes.size() == 1) {
        returnMatches.push_back(std::make_pair(induction_indexes[0], collection_indexes[0]));
        return returnMatches;
    }

    // When there are multiple clusters, order them in time in each plane and then use that
    // as the matches.  ordering based on start point.

    std::vector<int> ordered_indexes;
    // Order the collection clusters:
    while (collection_indexes.size() > 0) {
        double lowest_time = 5000;
        int lowest_index_index;
        for (size_t i = 0; i < collection_indexes.size(); i++) {
            if (bgcdata.at(collection_indexes.at(i)).starthit[1] < lowest_time ) {
                lowest_index_index = i;
            }
        }
        ordered_indexes.push_back(collection_indexes.at(lowest_index_index));
        collection_indexes.erase(collection_indexes.begin() + lowest_index_index);
    }

    collection_indexes = ordered_indexes;

    ordered_indexes.clear();
    // Order the induction clusters:
    while (induction_indexes.size() > 0) {
        double lowest_time = 5000;
        int lowest_index_index;
        for (size_t i = 0; i < induction_indexes.size(); i++) {
            if (bgcdata.at(induction_indexes.at(i)).starthit[1] < lowest_time ) {
                lowest_index_index = i;
            }
        }
        ordered_indexes.push_back(induction_indexes.at(lowest_index_index));
        induction_indexes.erase(induction_indexes.begin() + lowest_index_index);
    }

    induction_indexes = ordered_indexes;

    // Now, zip the matches together into pairs:
    for (size_t i = 0; i < induction_indexes.size(); i ++) {
        returnMatches.push_back(std::make_pair(induction_indexes[i], collection_indexes[i]));
    }

    return returnMatches;

}

std::vector<std::pair<int, int> > dEdxShowerAna::matchBCG_to_LL(
    const std::vector<std::pair<int, int> > & bgcmatches,
    const std::vector<argoutils::BGCData> & this_bgcdata,
    larlite::event_hit * ev_hit,
    const std::vector<std::vector<unsigned int > > & hit_ass  ) {

    // The clusters were made, originally, from the bgcluster info.
    //
    // There should be not so hard.  Every cluster is build from a bgcluster originally anyways.

    // Basically, go through the bgcmatches and, for each cluster in each pair,
    // find the set of hits that matches.
    // Can use the plane to cut half clusters out, and then use the wires to match.
    // If the number of hits matches exactly, though, that's quite good too.

    std::vector<std::pair<int, int> > returnMatches;

    std::cout << "bgcmatches.size() is " << bgcmatches.size()  << std::endl;
    std::cout << "hit_ass.size() is " << hit_ass.size()  << std::endl;

    if (2 * bgcmatches.size() != hit_ass.size()) {
        //bail
        std::cout << "Bailing!" << std::endl;
        return returnMatches;
    }

    // Just compare the number of hits in bgcluster data and hit_sets
    for (size_t i = 0; i < this_bgcdata.size(); i++) {
        std::cout << "bgcluster.size(): " << this_bgcdata.at(i).hitwire.size()
                  << " vs. hit_set.size(): " << hit_ass.at(i).size() << std::endl;
    }


    // for (size_t i_pair = 0; i_pair < bgcmatches.size(); i_pair ++) {

    //     // Do the induction cluster first.
    //     auto & i_bgcluster = this_bgcdata.at(bgcmatches.at(i_pair).first);
    //     auto & c_bgcluster = this_bgcdata.at(bgcmatches.at(i_pair).second);

    //     unsigned int c_best_match_index;
    //     unsigned int i_best_match_index;
    //     double c_best_score = 999;
    //     double i_best_score = 999;

    //     // We have a bgcluster, and need to match it to a set of hits.
    //     for (auto & hit_set : hit_ass) {
    //         auto plane = ev_hit->at(hit_set.front()).WireID().Plane;
    //         std::cout << "plane is " << plane << std::endl;
    //         if (plane == 0) {
    //             double score = fabs(1.0 * hit_set.size() - i_bgcluster.hitwire.size())
    //                            / i_bgcluster.hitwire.size();
    //             score = fabs(1 - score);
    //             if (score < i_best_score) {
    //                 i_best_score = score;
    //             }
    //             std::cout << "Trying to match " << hit_set.size() << " against "
    //                       << i_bgcluster.hitwire.size() << " hits." << std::endl;
    //         }
    //         if (plane == 1) {
    //             double score = fabs(1.0 * hit_set.size() - c_bgcluster.hitwire.size())
    //                            / c_bgcluster.hitwire.size();
    //             score = fabs(1 - score);
    //             if (score < c_best_score) {
    //                 c_best_score = score;
    //             }
    //             std::cout << "Trying to match " << hit_set.size() << " against "
    //                       << c_bgcluster.hitwire.size() << " hits." << std::endl;
    //         }
    //     }
    // }

    return returnMatches;

}




void dEdxShowerAna::getClosestHits(larlite::event_hit * ev_hit, std::vector<unsigned int>  hits_to_consider,
                                   std::vector<size_t> & close_hit_indexes,
                                   std::vector<double> starthit) {

    // this function loops over the hits, and finds the hit closest to a target point.
    // When it's found the closest hit, it adds it to the list of indexes and moves the target
    // to the hit just found.
    //
    // Once a hit is in the list of indexes, it can't be considered any more of course.

    close_hit_indexes.clear();

    auto geomHelper = larutil::GeometryHelper::GetME();

    std::vector<double> target_point = starthit;

    unsigned int n_targethits = 15;

    if (hits_to_consider.size() < n_targethits){
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
            dist +=  pow((1.0*ev_hit->at(index).WireID().Wire - target_point[0]) * geomHelper->WireToCm(), 2);
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

void dEdxShowerAna::unwindVectors(larlite::event_hit * ev_hit, std::vector<size_t> & close_hit_indexes) {

    // This function unwinds the vectors based on wire number so that they are,
    // at least, in an order.
    //
    // It compares the first and last hit and sorts in ascending or descending order accordingly

    // It runs right after finding the close hits, so that the actual sorted variables
    // are the close hit indexes.

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

double dEdxShowerAna::getPitch(const TVector3 & dir3D, int pl ) {

    double pitch = 0.4;


    double angleToVert = geom -> WireAngleToVertical((::larlite::geo::View_t) pl) - 0.5 * TMath::Pi();
    double cosgamma = TMath::Abs(TMath::Sin(angleToVert) * dir3D.Y() +
                                 TMath::Cos(angleToVert) * dir3D.Z());

    if (cosgamma > 0) pitch = pitch / cosgamma;

    return pitch;
}
bool dEdxShowerAna::finalize() {

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
