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
  TVector3 _truth_start_dir;

  double _true_energy = 0.0;
  double _true_deposited_energy = 0.0;

  if (_is_mc) {
    // Get the mcshower start point:
    larlite::event_mcshower * ev_mcshower = storage->get_data<larlite::event_mcshower>("mcinfo");
    _other_vertex = ev_mcshower -> front().DetProfile().Position().Vect();
    _truth_start_dir = ev_mcshower -> front().DetProfile().Momentum().Vect();
    _true_energy = ev_mcshower -> front().Start().E();
    _true_deposited_energy = ev_mcshower -> front().DetProfile().E();
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

    if (std::isnan(startDir.X()) ||
        std::isnan(startDir.Y()) ||
        std::isnan(startDir.Z())) {
      return false;
    }


    std::vector<protoshower::ProtoShower> ps_vec;
    ps_helper.GenerateProtoShowers(storage, ev_pfpart->name(), ps_vec);

    auto & proto_shower = ps_vec.at(pfpart_ass[i_shower].front());

    ShowerCalo this_calo;

    if (_is_mc) {
      this_calo._is_mc = true;
      this_calo._true_start_point = _other_vertex;
      this_calo._true_direction = _truth_start_dir;
      this_calo._true_energy = _true_energy;
      this_calo._true_deposited_energy = _true_deposited_energy;
    }

    this_calo._other_start_point = _other_vertex;

    this_calo._3D_start_point = startpoint;
    this_calo._3D_direction = startDir;

    // Now, for each plane, get the start point of the shower
    // and use it to pick the hits
    // Get the run and event number:
    this_calo._run = ev_pfpart->run();
    this_calo._event = ev_pfpart->event_id();

    int best_plane = 1;
    this_calo._best_plane = best_plane;

    // See if this event passes the cut:
    if (! _is_mc && _select_events) {

      bool pass =  keepEvent(this_calo.run(), this_calo.event(), best_plane);


      if (! pass) {
        return false;
      }

      if (best_plane != 1) {
        this_calo._best_plane = best_plane;
      }


    }


    for (auto & _params : proto_shower.params()) {

      int plane = _params.plane_id.Plane;

      // This block is the new way to get the hits for dE/dx computation.
      // Uses the hits between the starting point and the showering point

      auto startingPoint = _params.start_point;
      auto showeringPoint = _params.showering_point;

      double slope;
      if (_params.start_dir[0] != 0.0) {
        slope = _params.start_dir[1] / _params.start_dir[0];
      }
      else {
        slope = 999;
      }

      std::vector<unsigned int> _close_hit_indexes;

      double dist = (startingPoint.w - showeringPoint.w) *
                    (startingPoint.w - showeringPoint.w);
      dist +=  (startingPoint.t - showeringPoint.t) *
               (startingPoint.t - showeringPoint.t);

      dist = sqrt(dist);


      Hit2D averagePoint;
      Hit2D startingHit;
      startingHit.w = startingPoint.w;
      startingHit.t = startingPoint.t;

      if (! _is_mc && _select_events) {
        _close_hit_indexes = hand_select_hits(this_calo.run(), this_calo.event(), plane);
        if (_close_hit_indexes.size() == 0) {
          _close_hit_indexes =
            geomHelper -> SelectLocalPointList( _params.hit_vector,
                                                startingHit,
                                                dist,
                                                0.5,
                                                slope,
                                                averagePoint);
        }
      }
      else {
        _close_hit_indexes =
          geomHelper -> SelectLocalPointList( _params.hit_vector,
                                              startingHit,
                                              dist,
                                              0.5,
                                              slope,
                                              averagePoint);
      }

      // This block of code is the "old way" to get the hits for computing dE/dx
      /*

      // auto startingPoint = geomHelper->Point_3Dto2D(_other_vertex, _params.plane_id.Plane);
      // Hit2D startingHit;
      // startingHit.w = startingPoint.w;
      // startingHit.t = startingPoint.t;

      // float slope = startingHit.t - _params.mean_y;
      // slope /= startingHit.w - _params.mean_x;


      // Hit2D averagePoint;

      // std::vector<unsigned int> _close_hit_indexes =
      //   geomHelper -> SelectLocalPointList( _params.hit_vector,
      //                                       startingHit,
      //                                       4.0,
      //                                       0.5,
      //                                       slope,
      //                                       averagePoint);


      // startingPoint = findClosestHit(_params.hit_vector,
      //                                _close_hit_indexes,
      //                                startingPoint);

      */

      // Need to get the pitch in this plane:
      double pitch = getPitch(startDir, plane);


      double projection_error = 0.0;
      projection_error = fabs((geomHelper->Slope_3Dto2D(startDir, plane) - slope) / slope);

      // Fill out the plane by plane info:
      if (_params.plane_id.Plane == 0) {
        //induction

        for (auto & index : _close_hit_indexes) {
          auto & hit = _params.hit_vector.at(index);
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
          double dedx = recombFactor * dqdx;
          if (dedx > 8.0) {
            continue;
          }

          this_calo._induction_hits.push_back(_params.hit_vector.at(index));
          this_calo._induction_dqdx.push_back(dqdx);
          this_calo._induction_dedx.push_back( recombFactor * dqdx);
          this_calo._induction_dedx_box.push_back( LArProp->ModBoxCorrection(dqdx));
          double wire_diff = (hit.w - startingPoint.w) / geomHelper->WireToCm();
          this_calo._induction_dist_3D.push_back(wire_diff * pitch);

        }


        this_calo._induction_start = startingPoint;
        this_calo._induction_pitch = pitch;
        this_calo._induction_slope = slope;
        this_calo._induction_projection_error = projection_error;
        this_calo._induction_dist = dist;
      }
      else {

        for (auto & index : _close_hit_indexes) {
          auto & hit = _params.hit_vector.at(index);
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
          double dedx = recombFactor * dqdx;
          if (dedx > 8.0) {
            continue;
          }
          this_calo._collection_hits.push_back(_params.hit_vector.at(index));
          this_calo._collection_dqdx.push_back(dqdx);
          this_calo._collection_dedx.push_back( recombFactor * dqdx);
          this_calo._collection_dedx_box.push_back( LArProp->ModBoxCorrection(dqdx));
          double wire_diff = (hit.w - startingHit.w) / geomHelper->WireToCm();
          this_calo._collection_dist_3D.push_back(wire_diff * pitch);

        }

        this_calo._collection_start = startingPoint;
        this_calo._collection_pitch = pitch;
        this_calo._collection_slope = slope;
        this_calo._collection_projection_error = projection_error;
        this_calo._collection_dist = dist;
      }

    }

    // if (this_calo._collection_dist < 0.5) {
    //   return false;
    // }
    // if (this_calo._induction_dist < 0.5) {
    //   return false;
    // }
    // if (this_calo.dEdx_meta_err(0) / this_calo.dEdx_meta(0) > 0.3) {
    //   return false;
    // }
    // if (this_calo.dEdx_meta_err(1) / this_calo.dEdx_meta(1) > 0.3) {
    //   return false;
    // }

    // if ( fabs(this_calo.dEdx_meta(0) - this_calo.dEdx_meta(1)) > 1.5 ) {
    //   return false;
    // }

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

// This is god-mode:
// bool dEdxShowerCaloMaker::keepEvent(int run, int event, int & best_plane) {

//   if (run == 775 && event == 8598) {
//     return true;
//   }
//   if (run == 650 && event == 11366) {
//     return true;
//   }
//   if (run == 787 && event == 35240) {
//     return true;
//   }
//   if (run == 661 && event == 30777) {
//     return true;
//   }
//   if (run == 799 && event == 22141) {
//     return true;
//   }
//   if (run == 827 && event == 29437) {
//     return true;
//   }
//   if (run == 738 && event == 3028) {
//     return true;
//   }
//   if (run == 738 && event == 31324) {
//     return true;
//   }
//   if (run == 622 && event == 2738) {
//     return true;
//   }
//   if (run == 761 && event == 24948) {
//     return true;
//   }
//   if (run == 765 && event == 19905) {
//     return true;
//   }

//   return false;
// }

bool dEdxShowerCaloMaker::keepEvent(int run, int event, int & best_plane) {


  if (run == 770 && event == 15857) {
    return true;
  }
  if (run == 772 && event == 26010) {
    return true;
  }
  if (run == 775 && event == 25633) {
    return true;
  }
  if (run == 775 && event == 8598) {
    return true;
  }
  if (run == 650 && event == 11366) {
    return true;
  }
  if (run == 653 && event == 7810) {
    return true;
  }
  if (run == 783 && event == 33971) {
    best_plane = 0;
    return true;
  }
  if (run == 787 && event == 19854) {
    return true;
  }
  if (run == 787 && event == 35240) {
    return true;
  }
  if (run == 661 && event == 30777) {
    return true;
  }
  if (run == 668 && event == 12290) {
    return true;
  }
  if (run == 799 && event == 22141) {
    return true;
  }
  if (run == 673 && event == 26021) {
    return true;
  }
  if (run == 674 && event == 15897) {
    best_plane = 0;
    return true;
  }
  // if (run == 677 && event == 20169) {
  //   return true;
  // }
  if (run == 839 && event == 19489) {
    return true;
  }
  if (run == 815 && event == 8471) {
    return true;
  }
  if (run == 755 && event == 27529) {
    return true;
  }
  if (run == 823 && event == 17611) {
    return true;
  }
  if (run == 697 && event == 9815) {
    return true;
  }
  if (run == 827 && event == 29437) {
    return true;
  }
  if (run == 832 && event == 26531) {
    return true;
  }
  if (run == 711 && event == 2465) {
    return true;
  }
  if (run == 847 && event == 11704) {
    return true;
  }
  if (run == 720 && event == 34843) {
    return true;
  }
  if (run == 720 && event == 4063) {
    best_plane = 0;
    return true;
  }
  if (run == 721 && event == 39873) {
    best_plane = 0;
    return true;
  }
  if (run ==   724 && event == 13980) {
    best_plane = 0;
    return true;
  }
  if (run ==   724 && event == 21713) {
    best_plane = 0;
    return true;
  }
  if (run == 724 && event == 22877) {
    best_plane = 0;
    return true;
  }
  if (run == 738 && event == 3028) {
    return true;
  }
  if (run == 738 && event == 31324) {
    return true;
  }
  if (run == 828 && event == 3901) {
    return true;
  }
  if (run == 620 && event == 3756) {
    return true;
  }
  if (run == 622 && event == 2738) {
    return true;
  }
  if (run ==   627 && event == 895) {
    best_plane = 0;
    return true;
  }
  if (run == 629 && event == 32812) {
    return true;
  }
  if (run == 629 && event == 25898) {
    return true;
  }
  if (run == 761 && event == 24948) {
    return true;
  }
  if (run == 634 && event == 25212) {
    return true;
  }
  if (run ==   635 && event == 25757) {
    best_plane = 0;
    return true;
  }
  if (run == 765 && event == 19905) {
    return true;
  }

  return false;

}

std::vector<unsigned int> dEdxShowerCaloMaker::hand_select_hits(int run, int event, int plane) {
  std::vector<unsigned int> _hit_indexes;
  

  if (run == 770 && event == 15857 && plane == 1) {
    _hit_indexes = {1,2,3,4,5};
  }
  if (run == 772 && event == 26010 && plane == 1) {
    _hit_indexes = {1,2,3};
  }
  if (run == 775 && event == 25633 && plane == 1) {
  }
  if (run == 775 && event == 8598 && plane == 1) {
  }
  if (run == 650 && event == 11366 && plane == 1) {
    _hit_indexes = {1,2,3,4,5};
  }
  if (run == 653 && event == 7810 && plane == 1) {
    _hit_indexes = {1,2,3,4,5,6,7};
  }
  if (run == 783 && event == 33971 && plane == 0) {
    _hit_indexes = {0,1,2,3};
  }
  if (run == 787 && event == 19854) {
  }
  if (run == 787 && event == 35240) {
  }
  if (run == 661 && event == 30777) {
  }
  if (run == 668 && event == 12290) {
  }
  if (run == 799 && event == 22141) {
  }
  if (run == 673 && event == 26021) {
  }
  if (run == 674 && event == 15897 && plane == 0) {
    _hit_indexes = {0,1,2}
  }
  // if (run == 677 && event == 20169) {
  // }
  if (run == 839 && event == 19489) {
  }
  if (run == 815 && event == 8471) {
  }
  if (run == 755 && event == 27529) {
  }
  if (run == 823 && event == 17611) {
  }
  if (run == 697 && event == 9815) {
  }
  // Left off right here
  if (run == 827 && event == 29437) {
  }
  if (run == 832 && event == 26531) {
  }
  if (run == 711 && event == 2465) {
  }
  if (run == 847 && event == 11704) {
  }
  if (run == 720 && event == 34843) {
  }
  if (run == 720 && event == 4063) {
    // best_plane = 0;
  }
  if (run == 721 && event == 39873) {
    // best_plane = 0;
  }
  if (run ==   724 && event == 13980) {
    // best_plane = 0;
  }
  if (run ==   724 && event == 21713) {
    // best_plane = 0;
  }
  if (run == 724 && event == 22877) {
    // best_plane = 0;
  }
  if (run == 738 && event == 3028) {
  }
  if (run == 738 && event == 31324) {
  }
  if (run == 828 && event == 3901) {
  }
  if (run == 620 && event == 3756) {
  }
  if (run == 622 && event == 2738) {
  }
  if (run ==   627 && event == 895) {
    // best_plane = 0;
  }
  if (run == 629 && event == 32812) {
  }
  if (run == 629 && event == 25898) {
  }
  if (run == 761 && event == 24948) {
  }
  if (run == 634 && event == 25212) {
  }
  if (run ==   635 && event == 25757) {
    // best_plane = 0;
  }
  if (run == 765 && event == 19905) {
  }  

  return _hit_indexes;
}

}
#endif
