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


    argoutils::BGCFileReader bcgreader;
    bcgreader.setInputFile("/home/cadams/larlite/UserDev/argoneut_electrons/ScanFileandrzejs.root");
    // bcgreader.setInputFile("/home/cadams/larlite/UserDev/argoneut_electrons/ScanFilejasaadi_neutrino.root");
    bcgreader.readData();

    bcgdata = bcgreader.getData();

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
        tree->Branch("c_pitch", &collection_pitch);
        tree->Branch("c_pitch_err", &collection_pitch_err);
        tree->Branch("i_hittimes", &induction_hittimes);
        tree->Branch("i_hitwires", &induction_hitwires);
        tree->Branch("i_hitcharges", &induction_hitcharges);
        tree->Branch("i_hitpeaks", &induction_hitpeaks);
        tree->Branch("i_starthit", &induction_starthit);
        tree->Branch("i_lifetime_corr", &induction_lifetime_corr);
        tree->Branch("i_pitch_err", &induction_pitch_err);
        tree->Branch("i_pitch", &induction_pitch);
    }

    return true;
}

bool dEdxShowerAna::analyze(storage_manager* storage) {


    // Clear out all the data:

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
    collection_slope = 0.0;
    collection_pitch = 0.0;
    induction_slope = 0.0;
    induction_pitch = 0.0;
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

    auto ev_clus = storage -> get_data<larlite::event_cluster> ("bootleg");

    if (ev_clus -> size() != 2){
        std::cout << "Skipping event because there are " << ev_clus -> size() << " clusters." << std::endl;
        return false;
    }

    larlite::event_hit * ev_hit = nullptr;
    auto hit_ass = storage -> find_one_ass(ev_clus->id(), ev_hit, ev_clus->name());

    // Get the run and event number:
    run = ev_clus->run();
    event = ev_clus->event_id();

    // Here's the plan:
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



    // for (size_t i_shower = 0; i_shower < ev_shower ->size(); i_shower ++) {
    // auto shower = ev_shower->at(i_shower);
    // auto clust_indexes = clust_ass.at(i_shower);
    // Get the bootleg data for this event:
    auto _this_bgcdata = bcgdata[run][event];
    auto geomHelper = larutil::GeometryHelper::GetME();
    auto LArProp = larutil::LArProperties::GetME();
    auto detprop = larutil::DetectorProperties::GetME();

    // Look at the first cluster for this shower:
    for (size_t i_clust = 0; i_clust <  ev_clus->size(); i_clust ++ ) {
        auto hit_indexes = hit_ass.at(i_clust);
        argoutils::BCGData this_bgcluster;
        int plane = ev_hit -> at(hit_indexes.front()).WireID().Plane;
        // Figure out which bgc goes with this cluster:
        if (bcgdata.find(run) != bcgdata.end()) {
            if (bcgdata[run].find(event) != bcgdata[run].end()) {
                for (auto bgc : bcgdata[run][event]) {
                    if (bgc.currplane == plane) {
                        this_bgcluster = bgc;
                        break;
                    }
                }
            }
        }
        // // Now we have the right bgc.
        // // Print out the start point from BGC:
        // std::cout << "Start Point is: (" << this_bgcluster.starthit.front() << ", "
        //           << this_bgcluster.starthit.back() << ") "
        //           << std::endl;

        // std::cout << "End Point is: (" << this_bgcluster.endhit.front() << ", "
        //           << this_bgcluster.endhit.back() << ") "
        //           << std::endl;

        // At this point, we have the list of hits, the start point,
        // and the direction, and the plane



        // Here, get the N closest hits to the start point from the cluster (sorted):
        std::vector<size_t> close_hit_indexes;
        getClosestHits(ev_hit, hit_indexes, close_hit_indexes, this_bgcluster.starthit);

        // std::cout << "Found " << close_hit_indexes.size() << " hits." << std::endl;
        unwindVectors(ev_hit, close_hit_indexes);

        // // Print out the hits:
        // for (unsigned int i_hit = 0; i_hit < close_hit_indexes.size(); i_hit++) {
        //     auto & hit = ev_hit -> at(close_hit_indexes[i_hit]);
        //     std::cout << "close hit hit at (" << hit.WireID().Wire
        //               << ", " << hit.PeakTime() << ")" << std::endl;
        // }

        // Use TPrincipal to calculate the direction in the plane of this list of hits
        TPrincipal fPrincipal(2, "D");
        for (auto & index : close_hit_indexes) {
            double data[2];
            data[0] = ev_hit->at(index).WireID().Wire * geomHelper -> WireToCm();
            data[1] = ev_hit->at(index).PeakTime() * geomHelper -> TimeToCm();
            fPrincipal.AddRow(data);
        }

        fPrincipal.MakePrincipals();
        double slope = ((* fPrincipal.GetEigenVectors())[1][0]) / ((* fPrincipal.GetEigenVectors())[0][0]);

        // std::cout << "Slope is " << slope << std::endl;
        double _tau = LArProp->ElectronLifetime();
        double timetick = detprop->SamplingRate() * 1.e-3;

        // Figure out the pitch:
        if (plane == 0) {
            // induction_pitch = getPitch(shower.Direction(), plane);
            induction_starthit.push_back(this_bgcluster.starthit.front());
            induction_starthit.push_back(this_bgcluster.starthit.back());
            induction_slope = slope;
            // Fill in the hit information:
            for (auto & index : close_hit_indexes) {
                auto & hit = ev_hit->at(index);
                induction_lifetime_corr.push_back(exp(hit.PeakTime() * timetick / _tau));
                induction_hittimes.push_back(hit.PeakTime());
                induction_hitwires.push_back(hit.WireID().Wire);
                induction_hitcharges.push_back(hit.Integral());
                induction_hitpeaks.push_back(hit.PeakAmplitude());
            }
        }
        if (plane == 1) {
            // collection_pitch = getPitch(shower.Direction(), plane);
            collection_starthit.push_back(this_bgcluster.starthit.front());
            collection_starthit.push_back(this_bgcluster.starthit.back());
            collection_slope = slope;
            for (auto & index : close_hit_indexes) {
                auto & hit = ev_hit->at(index);
                collection_lifetime_corr.push_back(exp(hit.PeakTime() * timetick / _tau));
                collection_hittimes.push_back(hit.PeakTime());
                collection_hitwires.push_back(hit.WireID().Wire);
                collection_hitcharges.push_back(hit.Integral());
                collection_hitpeaks.push_back(hit.PeakAmplitude());
            }
        }

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

    return true;
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


    while (close_hit_indexes.size() < n_targethits && close_hit_indexes.size() != hits_to_consider.size()) {

        unsigned int closest_index = -1;
        double shortest_dist = 999;
        double dist = 0;
        for (auto & index : hits_to_consider ) {
            dist = pow((ev_hit->at(index).PeakTime() - target_point[1]) * geomHelper->TimeToCm(), 2);
            dist +=  pow((ev_hit->at(index).WireID().Wire - target_point[0]) * geomHelper->WireToCm(), 2);
            if (dist < shortest_dist) {
                // Check that this hit isn't already used:
                bool used = false;
                for (auto & used_index : close_hit_indexes) {
                    if (index == used_index) {
                        used = true;
                        break;
                    }
                }
                if (!used) {
                    closest_index = index;
                    shortest_dist = dist;
                }
            }
        }

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
