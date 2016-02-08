#ifndef LARLITE_VERTEXCONDENSER_CXX
#define LARLITE_VERTEXCONDENSER_CXX

#include "VertexCondenser.h"
#include "LArUtil/GeometryHelper.h"
#include "DataFormat/endpoint2d.h"

namespace argovertex {

bool VertexCondenser::initialize() {

    //
    // This function is called in the beggining of event loop
    // Do all variable initialization you wish to do here.
    // If you have a histogram to fill in the event loop, for example,
    // here is a good place to create one on the heap (i.e. "new TH1D").
    //

    _minDist2d = 10.0;

    return true;
}

bool VertexCondenser::analyze(larlite::storage_manager* storage) {

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

    // Declare the output so that there is not an offset
    // Attempt to get the output at the front:
    auto out_endpoint2d_v =  storage -> get_data<larlite::event_endpoint2d>("tempName2");
    auto out_ass = storage->get_data<larlite::event_ass>(out_endpoint2d_v->name());

    // start by getting the endpoint 2ds and the associations if needed.
    auto endpoint2d_v =  storage -> get_data<larlite::event_endpoint2d>("tempName");

    // Now, loop over the endpoint2d and make groups based on the location
    //
    // For each end point, loop over the remaining end points.  If it's within a centimeter of another endpoint,
    // add them to a collection to be merged.
    //
    // if it's not close to anything, move on.
    //
    // Since the distance is symmetric, don't have to do n^2
    //
    std::vector < std::vector<size_t> > epColl(endpoint2d_v->size());

    auto geoHelper = larutil::GeometryHelper::GetME();

    for (size_t i = 0; i < endpoint2d_v ->size(); i ++) {
        for (size_t j = 0; j < endpoint2d_v ->size(); j ++) {

            if (i == j ){
                continue;
            }

            auto & point_i = endpoint2d_v -> at(i);
            auto & point_j = endpoint2d_v -> at(j);
            if (point_i.View() != point_j.View())
                continue;
            float dist = 0;
            dist += pow( (point_i.Wire() - point_j.Wire()) * geoHelper -> WireToCm(), 2);
            dist += pow( (point_i.DriftTime() - point_j.DriftTime()) * geoHelper -> WireToCm(), 2);

            std::cout << "dist is " << dist << std::endl;

            if (dist < _minDist2d) {
                epColl.at(i).push_back(j);
            }
        }
    }

    std::cout << "epColl is: \n";
    size_t index = 0;
    for (auto & vec : epColl) {
        std::cout << index << ": ";
        for (auto & ind : vec) {
            std::cout << ind << " ";
        }
        std::cout << std::endl;
        index ++;
    }
    std::cout << std::endl;

    // return true;

    // Now we know which ones are close together
    // Loop over and create a list of new endpoints

    std::vector<std::vector<size_t> > finalIndexColl(epColl.size());

    std::cout << finalIndexColl.size() << std::endl;

    for (size_t i = 0; i < epColl.size(); i ++) {

        std::cout << "i is "<< i << ", Final Index Coll is: \n";
        index = 0;
        for (auto & vec : finalIndexColl) {
            std::cout << index << ": ";
            for (auto & ind : vec) {
                std::cout << ind << " ";
            }
            std::cout << std::endl;
            index ++;
        }
        std::cout << std::endl;

        // If the first one has nothing in it's list, remove it
        if (epColl.at(i).size() == 0) {
            continue;
        }
        // Else, this one has something.
        // Push back finalIndexColl to store it, and follow till done.
        auto temp = condeseIndex(i,epColl);
        finalIndexColl.at(i).reserve(finalIndexColl.at(i).size() + temp.size());
        finalIndexColl.at(i).insert(finalIndexColl.at(i).end(), temp.begin(), temp.end());

    }

    std::cout << "Final Index Coll is: \n";
    index = 0;
    for (auto & vec : finalIndexColl) {
        std::cout << index << ": ";
        for (auto & ind : vec) {
            std::cout << ind << " ";
        }
        std::cout << std::endl;
        index ++;
    }
    std::cout << std::endl;

    return true;
}

std::vector<size_t>  VertexCondenser::condeseIndex(size_t index,
        std::vector<std::vector<size_t>> & input ) {

    std::vector<size_t> result;

    if (input.at(index).size() == 0) {
        std::cout << "Returning " << index << std::endl;
        result.push_back(index);
        return result;
    }
    else {
        for (auto & ind : input.at(index)) {
            std::cout << "Going into " << ind << std::endl;
            auto extra = condeseIndex(ind, input);
            result.reserve(result.size() + extra.size());
            result.insert(result.end(), extra.begin(), extra.end());
            input.at(ind).clear();
        }
    }

    return result;
}

bool VertexCondenser::finalize() {

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
