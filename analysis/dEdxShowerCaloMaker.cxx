#ifndef LARLITE_DEDXSHOWERCALOMAKER_CXX
#define LARLITE_DEDXSHOWERCALOMAKER_CXX

#include "dEdxShowerCaloMaker.h"

namespace argoana {

bool dEdxShowerCaloMaker::initialize() {


  ps_alg = new protoshower::ProtoShowerAlgArgoNeuT();
  ps_helper.setProtoShowerAlg(ps_alg);

  return true;
}

bool dEdxShowerCaloMaker::analyze(larlite::storage_manager* storage) {


  // Get the showers for this event
  larlite::event_shower * ev_shower = storage -> get_data<larlite::event_shower>(_producer_name);


  // Get the pfparticles
  larlite::event_pfpart * ev_pfpart = nullptr;
  auto pfpart_ass = storage -> find_one_ass(ev_shower->id(), ev_pfpart, ev_shower->name());

  TVector3 _other_vertex;

  if (_is_mc) {
    // Get the mcshower start point:
    larlite::event_mcshower * ev_mcshower = storage->get_data<larlite::event_mcshower>("mcinfo");
    _other_vertex = ev_mcshower -> front().DetProfile().Position().Vect();
  }
  else {
    // Get the vertex from the "bootlegVertex producer used in data"
    auto * ev_vertex = storage->get_data<larlite::event_vertex>("bootlegVertex");
    _other_vertex.SetX(ev_vertex->front().X());
    _other_vertex.SetY(ev_vertex->front().Y());
    _other_vertex.SetZ(ev_vertex->front().Z());

  }


  auto geomHelper = larutil::GeometryHelper::GetME();
  auto LArProp = larutil::LArProperties::GetME();
  auto detprop = larutil::DetectorProperties::GetME();

  double _mip_q = LArProp->ModBoxInverse(1.73);


  const double recombFactor = LArProp->ModBoxCorrection(_mip_q) / _mip_q;
  // std::cout << "Recomb factor: " << recombFactor << std::endl;

  double _tau = LArProp->ElectronLifetime();
  double timetick = detprop->SamplingRate() * 1.e-3;

  // std::cout << "Run " << run << " event " << event << "." << std::endl;

  for (size_t i_shower = 0; i_shower < ev_shower -> size(); i_shower ++) {

    auto & shower = ev_shower -> at(i_shower);

    auto & pfpart = ev_pfpart -> at(pfpart_ass[i_shower].front());

    // Get the proto shower associate with this pfparticle

    TVector3 startDir = shower.Direction();
    TVector3 startpoint = shower.ShowerStart();



    std::vector<protoshower::ProtoShower> ps_vec;
    ps_helper.GenerateProtoShowers(storage, ev_pfpart->name(), ps_vec);

    auto & proto_shower = ps_vec.at(pfpart_ass[i_shower].front());

    ShowerCalo this_calo;

    this_calo._other_start_point = _other_vertex;

    this_calo._3D_start_point = startpoint;
    this_calo._3D_direction = startDir;

    // Now, for each plane, get the start point of the shower
    // and use it to pick the hits
    // Get the run and event number:
    this_calo.run = ev_pfpart->run();
    this_calo.event = ev_pfpart->event_id();


    for (auto & _params : proto_shower.params()) {

      int plane = _params.plane_id.Plane;

      auto startingPoint = geomHelper->Point_3Dto2D(_other_vertex, _params.plane_id.Plane);
      Hit2D startingHit;
      startingHit.w = startingPoint.w;
      startingHit.t = startingPoint.t;

      float slope = startingHit.t - _params.mean_y;
      slope /= startingHit.w - _params.mean_x;


      Hit2D averagePoint;

      std::vector<unsigned int> _close_hit_indexes =
        geomHelper -> SelectLocalPointList( _params.hit_vector,
                                            startingHit,
                                            4.0,
                                            0.5,
                                            slope,
                                            averagePoint);

      startingPoint = findClosestHit(_params.hit_vector,
                                     _close_hit_indexes,
                                     startingPoint);

      // Need to get the pitch in this plane:
      double pitch = getPitch(startDir, plane);


      double projection_error = 0.0;
      projection_error = fabs((geomHelper->Slope_3Dto2D(startDir, plane) - slope) / slope);

      // Fill out the plane by plane info:
      if (_params.plane_id.Plane == 0) {
        //induction

        for (auto & index : _close_hit_indexes) {
          auto & hit = _params.hit_vector.at(index);
          this_calo._induction_hits.push_back(_params.hit_vector.at(index));
          // To get the dEdx, apply the lifetime correction and
          // the calbration constants to get dQ/dx, then scale to dE/dx
          double dq = hit.charge;
          // Lifetime correction needs to use the time tick:
          double tick = (hit.t + 0.6) / (geomHelper->TimeToCm())
                        + detprop -> TriggerOffset();
          double lifetime_corr = exp(tick * timetick / _tau);
          double _wire_temp = (hit.w) / geomHelper->WireToCm();
          int wire = _wire_temp + 0.1;
          double dqdx = (dq * lifetime_corr * _calorimetry_corrections[0][wire]) / pitch;
          this_calo._induction_dqdx.push_back(dqdx);
          this_calo._induction_dedx.push_back( recombFactor * dqdx);
          double wire_diff = (hit.w - startingPoint.w);
          this_calo._induction_dist_3D.push_back(wire_diff * pitch);

        }


        this_calo._induction_start = startingPoint;
        this_calo._induction_pitch = pitch;
        this_calo._induction_slope = slope;
        this_calo._induction_projection_error = projection_error;
      }
      else {

        for (auto & index : _close_hit_indexes) {
          auto & hit = _params.hit_vector.at(index);
          this_calo._collection_hits.push_back(_params.hit_vector.at(index));
          // To get the dEdx, apply the lifetime correction and
          // the calbration constants to get dQ/dx, then scale to dE/dx
          double dq = hit.charge;
          // Lifetime correction needs to use the time tick:
          double tick = (hit.t + 0.2) / (geomHelper->TimeToCm())
                        + detprop -> TriggerOffset();
          double lifetime_corr = exp(tick * timetick / _tau);
          double _wire_temp = (hit.w) / geomHelper->WireToCm();
          int wire = _wire_temp + 0.1;
          double dqdx = (dq * lifetime_corr * _calorimetry_corrections[1][wire]) / pitch;
          this_calo._collection_dqdx.push_back(dqdx);
          this_calo._collection_dedx.push_back( recombFactor * dqdx);
          double wire_diff = (hit.w - startingHit.w);
          this_calo._collection_dist_3D.push_back(wire_diff * pitch);

        }

        this_calo._collection_start = startingPoint;
        this_calo._collection_pitch = pitch;
        this_calo._collection_slope = slope;
        this_calo._collection_projection_error = projection_error;
      }

    }

    // Here, we can make the quality cuts:

    if (this_calo.dEdx_meta_err(0) / this_calo.dEdx_meta(0) > 0.3) {
      return false;
    }
    if (this_calo.dEdx_meta_err(1) / this_calo.dEdx_meta(1) > 0.3) {
      return false;
    }

    // if ( fabs(this_calo.dEdx_meta(0) - this_calo.dEdx_meta(1)) > 1.5 ){
      // return false;
    // }
    if (_is_mc) {
      // Monte Carlo
    }
    else {
      // Data

    }

    // Make the ShowerCalo object for this shower
    _shower_calo_vector.push_back(this_calo);

  }

  return true;
}

double dEdxShowerCaloMaker::getPitch(const TVector3 & dir3D, int pl ) {

  double pitch = 0.4;

  auto geom = larutil::Geometry::GetME();

  double angleToVert = geom -> WireAngleToVertical((::larlite::geo::View_t) pl) - 0.5 * TMath::Pi();
  double cosgamma = TMath::Abs(TMath::Sin(angleToVert) * dir3D.Y() +
                               TMath::Cos(angleToVert) * dir3D.Z());

  if (cosgamma > 0) pitch = pitch / cosgamma;

  return pitch;
}



Point2D dEdxShowerCaloMaker::findClosestHit(std::vector<Hit2D> hit_vector,
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

bool dEdxShowerCaloMaker::finalize() {


  return true;
}

}
#endif
