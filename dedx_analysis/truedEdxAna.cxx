#ifndef LARLITE_TRUEDEDXANA_CXX
#define LARLITE_TRUEDEDXANA_CXX

#include "truedEdxAna.h"

#include "LArUtil/GeometryHelper.h"

#include "DataFormat/mcshower.h"
#include "DataFormat/simch.h"

namespace larlite {

bool truedEdxAna::initialize() {

    //
    // This function is called in the beggining of event loop
    // Do all variable initialization you wish to do here.
    // If you have a histogram to fill in the event loop, for example,
    // here is a good place to create one on the heap (i.e. "new TH1D").
    //

    return true;
}

struct sort_first {
  bool operator()(const std::pair<double, double> &left, const std::pair<double, double> &right) {
    return left.first < right.first;
  }
};

bool truedEdxAna::analyze(storage_manager* storage) {

    // Get the simch data product:

    auto ev_simch = storage->get_data<larlite::event_simch>("largeant");
    auto ev_mcshower = storage->get_data<larlite::event_mcshower>("mcinfo");

    if (! ev_simch || ev_simch->size() == 0) {
        return false;
    }


    if (! ev_mcshower || ev_mcshower->size() == 0) {
        return false;
    }

    // There should be only one mcshower:
    if (ev_mcshower -> size() > 1) {
        std::cerr << "Found too many mcshowers!" << std::endl;
        exit(-1);
    }

    auto geomHelper = larutil::GeometryHelper::GetME();

    auto mcshower = ev_mcshower->front();

    // First step: determine the total deposited energy in this event

    double depE = mcshower.DetProfile().E();

    // get the start point and start direction, because we'll be using those
    // to construct the "true" dE/dx.

    TVector3 startPoint = mcshower.DetProfile().Position().Vect();
    TVector3 startDir   = mcshower.DetProfile().Momentum().Vect();
    startDir *= 1.0 / startDir.Mag();

    std::vector<double> cumulative_dedx;
    std::vector<double> sliding_dEdx_1cm;
    std::vector<double> sliding_dEdx_2cm;
    std::vector<double> sliding_dEdx_3cm;

    // We'll go for up to 20cm in dE/dx calculations.
    // Update the cumulative dE/dx

    std::vector<std::pair<double,double> > _distance_and_E;


    // Compare to the total energy deposited in the simch info:
    double simch_depE = 0.0;
    for (auto & simid : * ev_simch) {
        if (simid.Channel() < 240)
            continue;
        auto const& all_ide = simid.TrackIDsAndEnergies(0, 2040);

        for (auto const& ide : all_ide) {

            // We need to check if this deposition is within the cylinder defined.

            // Really, that can be most easily translated into the required metrics
            // by knowing how far along the direction it is from the start point, as well
            // as the perpendicular distance.

            TVector3 thisPoint(ide.x, ide.y, ide.z);

            double forward_distance
                = geomHelper -> DistanceAlongLine3D(startPoint, startDir, thisPoint);
            double perpendicular_distance
                = geomHelper -> DistanceToLine3D(startPoint, startDir, thisPoint);

            if (perpendicular_distance > 1 ){
                continue;
            }

            if (forward_distance < 20 && forward_distance > 0){
                _distance_and_E.push_back(std::make_pair(forward_distance, ide.energy));
            }

            // std::cout << "3dpoint : [" << ide.x << ", " << ide.y << ", " << ide.z << "]" << std::endl;
            // _data.push_back( SimChannel3D(ide.x, ide.y, ide.z) );
            simch_depE += ide.energy;
        }
    }

    std::sort(_distance_and_E.begin(), _distance_and_E.end(), sort_first());



    std::cout << "Event " << ev_simch -> event_id() << std::endl;
    std::cout << "  Mcshower reports " << depE << std::endl;
    std::cout << "  simch reports " << simch_depE << std::endl;

    for (auto & pair : _distance_and_E){
        std::cout << "    Distance " << pair.first << ", E " << pair.second << std::endl;
    }


    return true;
}




bool truedEdxAna::finalize() {

    return true;
}

}
#endif
