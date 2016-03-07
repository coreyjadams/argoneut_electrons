#ifndef ARGONEUTPARAMSALG_CXX
#define ARGONEUTPARAMSALG_CXX

#include "ArgoneutParamsAlg.h"




namespace argoutils {

ArgoneutParamsAlg::ArgoneutParamsAlg() {

    std::cout << "Initializing argoneut params!" << std::endl;

    // In the constructor, create, initialize and attach a set of algs
    ::cluster::GetAverages        * averages
        = new ::cluster::GetAverages();
    averages -> SetVerbose(false);
    attachAlg(averages);

    ::cluster::FillPolygon        * polygon
        = new ::cluster::FillPolygon();
    polygon -> SetVerbose(false);
    polygon -> setFrac(0.99);
    attachAlg(polygon);



    ::argoutils::dEdxGetRoughStartPoint * roughStartPoint
        = new ::argoutils::dEdxGetRoughStartPoint();
    roughStartPoint -> SetVerbose(false);
    attachAlg(roughStartPoint);

    ::cluster::FindShoweringPoint * showeringpoint
        = new ::cluster::FindShoweringPoint();
    showeringpoint -> SetVerbose(false);
    attachAlg(showeringpoint);

    ::argoutils::dEdxSelectStartPoint   * selectStartPoint
        = new ::argoutils::dEdxSelectStartPoint();
    selectStartPoint -> SetVerbose(false);
    attachAlg(selectStartPoint);

    ::cluster::RefineStartPoints  * refineStartPoint
        = new ::cluster::RefineStartPoints();
    refineStartPoint -> SetVerbose(false);
    attachAlg(refineStartPoint);

    ::cluster::FillGeomParams     * fillGeomParams
        = new ::cluster::FillGeomParams();
    fillGeomParams -> SetVerbose(false);
    attachAlg(fillGeomParams);

    SetDebug(false);
    SetVerbose(false);

}

} // cluster

#endif
