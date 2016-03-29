#ifndef LARLITE_SHOWERSPS_CXX
#define LARLITE_SHOWERSPS_CXX

#include "ShowerSPS.h"

#include "LArUtil/Geometry.h"
#include "LArUtil/GeometryHelper.h"

namespace argosps {

bool ShowerSPS::initialize() {

  //
  // This function is called in the beggining of event loop
  // Do all variable initialization you wish to do here.
  // If you have a histogram to fill in the event loop, for example,
  // here is a good place to create one on the heap (i.e. "new TH1D").
  //
  return true;
}

bool ShowerSPS::analyze(larlite::storage_manager* storage) {


  // Look for the hits that come from the clusters from the input
  // pfparticles.


  // Make space for the output spacepoints:
  auto ev_sps = storage -> get_data<larlite::event_spacepoint>("spsTest");
  // auto out_ass = storage->get_data<larlite::event_ass>(ev_sps->name());


  // because vertexing is run last, get the vertexes first and work through the
  // associations from there.
  auto ev_vertex = storage -> get_data<larlite::event_vertex>("bootlegVertex");



  // Get the pfparticles
  larlite::event_pfpart * ev_pfpart = nullptr;
  auto pfpart_ass = storage -> find_one_ass(ev_vertex->id(), ev_pfpart, ev_vertex->name());

  // Going to be appending to this association:
  auto out_ass = storage -> get_data<larlite::event_ass>(ev_pfpart->name());

  larlite::event_vertex * ev_vertex2 = nullptr;
  auto vertex_ass = storage -> find_one_ass(ev_pfpart->id(), ev_vertex2, ev_pfpart->name());


  // Get the clusters
  larlite::event_cluster * ev_clust = nullptr;
  auto clus_ass = storage -> find_one_ass(ev_pfpart->id(), ev_clust, ev_pfpart->name());


  // Get the hits associated with the clusters too
  larlite::event_hit * ev_hit = nullptr;
  auto hit_ass = storage -> find_one_ass(ev_clust->id(), ev_hit, ev_clust->name());

  // Get the endpoint2ds:
  larlite::event_endpoint2d * ev_endpoint = nullptr;
  auto endpoint_ass = storage -> find_one_ass(ev_clust->id(), ev_endpoint, ev_clust->name());


  // set event ID through storage manager
  storage->set_id(ev_pfpart->run(),
                  ev_pfpart->subrun(),
                  ev_pfpart->event_id());



  if (! ev_pfpart) {
    return false;
  }

  if (!ev_hit) {
    return false;
  }

  // std::cout << "Number of pfpart in this event: " << ev_pfpart->size() << std::endl;
  // std::cout << "Number of clusters in this event: " << ev_clust->size() << std::endl;
  // std::cout << "Number of cluster assns in this event: " << ev_clust->size() << std::endl;
  // std::cout << "Number of hits in this event: " << ev_hit->size() << std::endl;
  // std::cout << "Number of vertexes in this event: " << ev_vertex->size() << std::endl;


  auto geom = larutil::Geometry::GetME();
  auto geoHelper = larutil::GeometryHelper::GetME();

  // For this, use cluster params since it will get the offset of the hits correct.
  // Plus we get the mean and direction, etc.
  //

  std::vector<cluster::cluster_params> _params_v;

  _cru_helper.GenerateParams(storage,
                             ev_clust->name(),
                             _params_v);


  double  xyzStart[3];
  double  xyzEnd[3];

  // Collection of proto points, for each pfparticle
  std::vector<std::vector<ProtoSps> > _protoSPS;
  _protoSPS.resize(ev_clust->size());

  for (size_t i_pfpart = 0; i_pfpart < ev_pfpart->size(); i_pfpart ++) {


    for (size_t i_clust : clus_ass.at(i_pfpart)) {

      // std::cout << "On cluster " << i_clust
      //           << " with " << hit_ass.at(i_clust).size() << " hits."
      //           << std::endl;

      // Fill the parameters:
      _params_alg.FillParams(_params_v.at(i_clust));

      // Figure out which hits are important for this shower


      // getClosestHits(_params_v.at(i_clust),close_hit_indexes,ev_vertex->at(i_pfpart));
      auto vertex = ev_vertex->at(i_pfpart);
      double xyz[3];
      vertex.XYZ(xyz);
      auto target_point = geoHelper->Point_3Dto2D(xyz, _params_v.at(i_clust).plane_id.Plane);
      Hit2D startingHit;
      startingHit.w = target_point.w;
      startingHit.t = target_point.t;

      float slope = target_point.t - _params_v.at(i_clust).mean_y;
      slope /= target_point.w - _params_v.at(i_clust).mean_x;

      Hit2D averagePoint;


      std::vector<unsigned int> close_hit_indexes =
        geoHelper -> SelectLocalPointList( _params_v.at(i_clust).hit_vector,
                                           startingHit,
                                           4.0,
                                           1.0,
                                           slope,
                                           averagePoint);


      // // Want to know what hits were selected:
      // for (auto & index : close_hit_indexes) {
      //   std::cout << "Selected hit at (" << _params_v.at(i_clust).hit_vector.at(index).w
      //             << ", " << _params_v.at(i_clust).hit_vector.at(index).t << ").\n";
      // }
      // std::cout << std::endl;

      // Now, make proto-spacepoints for each hit.

      // Loop over all the hits in this cluster:
      for (size_t i_hit : close_hit_indexes) {



        // std::cout << "On hit " << i_hit << std::endl;

        // For this hit, figure out the min and max allowed points,
        // and then connect them with a line.
        auto & hit = _params_v.at(i_clust).hit_vector.at(i_hit);

        // std::cout << "Plane is " << hit.WireID().Plane << std::endl;

        // Need to covert wire in cm to wire in, well wires:
        double wire = hit.w / geoHelper->WireToCm() + 0.1;

        geom -> WireEndPoints(hit.plane,
                              wire,
                              xyzStart,
                              xyzEnd);

        // Ignore the x of the wires, use the x of the hits instead.
        // But this information defines a line in space that the hit has to have come from
        xyzEnd[0] = hit.t;
        xyzStart[0] = hit.t;

        // // Need to correct for the offset of the hit itself:
        // xyzStart[0] = hit.PeakTime() * geoHelper->TimeToCm() - xyzStart[0];
        // xyzEnd[0] = hit.PeakTime() * geoHelper->TimeToCm() - xyzEnd[0];



        _protoSPS.at(i_clust).push_back(ProtoSps());
        _protoSPS.at(i_clust).back()._matched_hit_index = hit_ass.at(i_clust).at(i_hit);
        _protoSPS.at(i_clust).back()._params_hit_index = i_hit;


        TVector3 start(xyzStart), end(xyzEnd);
        TVector3 origin = 0.5 * (start + end);
        TVector3 freeDirection = (end - start);
        freeDirection *= 1.0 / freeDirection.Mag();
        _protoSPS.at(i_clust).back().freeDirection = freeDirection;
        _protoSPS.at(i_clust).back().origin = origin;
        // Get the intial max and min alphas.  Their absolute value can only decrease from here.
        _protoSPS.at(i_clust).back()._max_alpha = (start - origin).Mag();
        _protoSPS.at(i_clust).back()._min_alpha = -(end - origin).Mag();
        _protoSPS.at(i_clust).back()._origin_plane = hit.plane;



      }

    }
  }

  /*
  Each cluster now has a set of proto spacepoints associated with it.
  First, adjust the points around a mean value until their projected
  slope matches a target slope in the other plane
  */

  // To match the slope, have to provide the target slope, target plane, and the list of spacepoints
  for (size_t i_pfpart = 0; i_pfpart < ev_pfpart->size(); i_pfpart ++) {

    // The clus_ass provides a pair of clusters that have space points that should match.
    // Just do this pairwise:

    auto cluster_to_adjust = clus_ass.at(i_pfpart).front();
    auto range_cluster = clus_ass.at(i_pfpart).back();

    // Get the target plane:
    auto range_plane = _params_v.at(range_cluster).plane_id.Plane;


    // Need the min and max wire from the range cluster:
    double min = 999 , max = -999;
    for (auto & _p_sps : _protoSPS.at(range_cluster)) {
      auto projection = geoHelper->Point_3Dto2D(_p_sps.xyz(), _p_sps._origin_plane);
      if (projection.w > max) {
        max = projection.w;
      }
      if (projection.w < min) {
        min = projection.w;
      }
    }

    // std::cout << "Plane " << range_plane << ": Min = " << min << ", max = " << max << std::endl;

    // Now go through and restrict the other spacepoints:
    for (auto & _p_sps : _protoSPS.at(cluster_to_adjust)) {
      restrictRange(_p_sps, min, max, range_plane);
    }


    // // Use the hits that are selected for space points from the target cluster
    // // to make the target slope:
    // double slope = getSlope(_params_v.at(match_target_cluster), _protoSPS.at(match_target_cluster));

    // fitPointsToSlope(_protoSPS.at(match_target_cluster), slope, target_plane);



    cluster_to_adjust = clus_ass.at(i_pfpart).back();
    range_cluster = clus_ass.at(i_pfpart).front();

    // Get the target plane:
    range_plane = _params_v.at(range_cluster).plane_id.Plane;

    // Need the min and max wire from the range cluster:
    min = 999;
    max = -999;
    for (auto & _p_sps : _protoSPS.at(range_cluster)) {
      auto projection = geoHelper->Point_3Dto2D(_p_sps.xyz(), _p_sps._origin_plane);
      if (projection.w > max) {
        max = projection.w;
      }
      if (projection.w < min) {
        min = projection.w;
      }
    }

    // std::cout << "Plane " << range_plane << ": Min = " << min << ", max = " << max << std::endl;

    // Now go through and restrict the other spacepoints:
    for (auto & _p_sps : _protoSPS.at(cluster_to_adjust)) {
      restrictRange(_p_sps, min, max, range_plane);
    }


    // // Use the hits that are selected for space points from the target cluster
    // // to make the target slope:
    // slope = getSlope(_params_v.at(match_target_cluster), _protoSPS.at(match_target_cluster));

    // fitPointsToSlope(_protoSPS.at(match_target_cluster), slope, target_plane);

  }

  // Then, adjust a global offset of the points from a matched set of proto sps
  // to minimize the 3D volume of the points.



  // // In this part, adjust the spacepoints along their free direction to match the vertex
  // for (size_t i_pfpart = 0; i_pfpart < ev_pfpart->size(); i_pfpart ++) {

  //   // Basically, the idea is to take a space point that came from one plane and
  //   // project it into the other plane, and see if it fits the points from the other cluster.
  //   // Gotta force the points to maintain their order in the projection, probably.
  //   auto vertex = ev_vertex->at(i_pfpart);

  //   for (size_t i_clust : clus_ass.at(i_pfpart)) {
  //     // for right now, just take the space points and adjust their
  //     // xyz until the y matches the vertex.
  //     //
  //     // It's wrong, but I want to see *how* wrong
  //     for (auto & psps : _protoSPS.at(i_clust)) {
  //       if (psps._origin_plane == 0) {
  //         // std::cout << "Adjusting ( " << psps.xyz.X()
  //         //           << ", " << psps.xyz.Y()
  //         //           << ", " << psps.xyz.Z()
  //         //           << ") ";
  //         TVector3 correction = psps.freeDirection;
  //         correction *= (vertex.Y() - psps.xyz.Y());
  //         correction *= 1.0 / psps.freeDirection.Y();
  //         psps.xyz +=  correction;
  //         // std::cout << "to ( " << psps.xyz.X()
  //         //           << ", " << psps.xyz.Y()
  //         //           << ", " << psps.xyz.Z()
  //         //           << ") " << std::endl;
  //       }
  //       else {
  //         TVector3 correction = psps.freeDirection;
  //         correction *= (vertex.Y() - psps.xyz.Y());
  //         correction *= 1.0 / psps.freeDirection.Y();

  //         psps.xyz += correction;

  //       }
  //     }
  //   }

  // }

  // Now save out the space points in the event.
  // Need to make associations:
  // sps <-> hit
  // vec<sps> < --- > pfpart

  larlite::AssSet_t spsToHitAss;
  larlite::AssSet_t pfpToSpsAss;
  pfpToSpsAss.resize(ev_pfpart->size());
  size_t id_index = 0;
  for (size_t i_pfpart = 0; i_pfpart < ev_pfpart->size(); i_pfpart ++) {

    // Make a space point set for this pfparticle.
    for (size_t i_clust : clus_ass.at(i_pfpart)) {
      for (size_t i_sps = 0; i_sps < _protoSPS.at(i_clust).size(); i_sps ++) {
        // Just as a test, skip hits starting from collection plane
        if (_protoSPS.at(i_clust).at(i_sps)._origin_plane == 1) {
          continue;
        }
        auto & proto_sps = _protoSPS.at(i_clust).at(i_sps);
        ev_sps->push_back(larlite::spacepoint(id_index,
                                              proto_sps.xyz().X(),
                                              proto_sps.xyz().Y(),
                                              proto_sps.xyz().Z(),
                                              0, 0, 0, 0));
        spsToHitAss.push_back(std::vector<unsigned int>(1, proto_sps._matched_hit_index));
        pfpToSpsAss.at(i_pfpart).push_back(id_index);
        id_index ++;

        // This is the limiting points:

        ev_sps->push_back(larlite::spacepoint(id_index,
                                              (proto_sps.origin + proto_sps._max_alpha * proto_sps.freeDirection).X(),
                                              (proto_sps.origin + proto_sps._max_alpha * proto_sps.freeDirection).Y(),
                                              (proto_sps.origin + proto_sps._max_alpha * proto_sps.freeDirection).Z(),
                                              0, 0, 0, 0));
        spsToHitAss.push_back(std::vector<unsigned int>(1, proto_sps._matched_hit_index));
        pfpToSpsAss.at(i_pfpart).push_back(id_index);
        id_index ++;
        ev_sps->push_back(larlite::spacepoint(id_index,
                                              (proto_sps.origin + proto_sps._min_alpha * proto_sps.freeDirection).X(),
                                              (proto_sps.origin + proto_sps._min_alpha * proto_sps.freeDirection).Y(),
                                              (proto_sps.origin + proto_sps._min_alpha * proto_sps.freeDirection).Z(),
                                              0, 0, 0, 0));
        spsToHitAss.push_back(std::vector<unsigned int>(1, proto_sps._matched_hit_index));
        pfpToSpsAss.at(i_pfpart).push_back(id_index);
        id_index ++;

        //////
      }
    }

  }

  // Set the associations:
  out_ass->set_association(ev_pfpart->id(), ev_sps->id(), pfpToSpsAss);
  out_ass->set_association(ev_sps->id(), ev_hit->id(), spsToHitAss);

  //         ev_sps -> push_back(spacepoint(id_index,
  //                                        x_point,
  //                                        y_point,
  //                                        z_point,
  //                                        0,
  //                                        0,
  //                                        0,
  //                                        0));


  // if
  // std::cout << "Made " << ev_sps -> size() << " spacepoints." << std::endl;


  return true;
}


double ShowerSPS::getSlope(const ::cluster::cluster_params & _params,
                           const std::vector<ProtoSps> & _proto_sps_v) {

  // This function takes the input params, fits a line to the hits specified by the indexes in the
  // proto sps, and returns the slope

  // Use TPrincipal to calculate the direction in the plane of this list of hits
  TPrincipal fPrincipal(2, "D");
  for (auto & psps : _proto_sps_v) {
    double data[2];
    data[0] = _params.hit_vector.at(psps._params_hit_index).w;
    data[1] = _params.hit_vector.at(psps._params_hit_index).t;
    fPrincipal.AddRow(data);
  }


  fPrincipal.MakePrincipals();
  double slope = ((* fPrincipal.GetEigenVectors())[1][0]) / ((* fPrincipal.GetEigenVectors())[0][0]);
  return slope;


}

void ShowerSPS::restrictRange(ProtoSps & _proto_sps, double wireMin, double wireMax, unsigned int plane) {

  // Shrink the max and min values for the scaling of this proto sps until it's
  // projection falls in the wire range

  auto geoHelper = larutil::GeometryHelper::GetME();

  // Deal with Max alpha first.
  // Find out if it's to the right or left of the wire range:

  auto projected_point
    = geoHelper->Point_3Dto2D(_proto_sps.origin + _proto_sps._max_alpha * _proto_sps.freeDirection,
                              plane);

  // std::cout << "Projecting a point from plane " << _proto_sps._origin_plane <<  " into plane " << plane << ", start w is " << projected_point.w << std::endl;

  double _abs_min = _proto_sps._min_alpha;
  double _abs_max = _proto_sps._max_alpha;

  // Get very close to just beyond the edge:
  int it = 0;
  double curr_err = fabs(projected_point.w - wireMax - 0.4);
  double step_size = 1.0;
  double curr_alpha = _proto_sps._max_alpha;
  while ( curr_err > 0.1 ) {
    if (it > 100) {
      std::cout << "  iteration maxout" << std::endl;
      break;
    }

    // Make three points, one at the value, one at +/- value
    auto point_minus
      = geoHelper->Point_3Dto2D(_proto_sps.origin + (curr_alpha - step_size) * _proto_sps.freeDirection,
                                plane);
    auto point_plus
      = geoHelper->Point_3Dto2D(_proto_sps.origin + (curr_alpha + step_size) * _proto_sps.freeDirection,
                                plane);
    auto point_nom
      = geoHelper->Point_3Dto2D(_proto_sps.origin + curr_alpha * _proto_sps.freeDirection,
                                plane);

    double error_minus = fabs(point_minus.w - wireMax - 0.4);
    double error_plus = fabs(point_plus.w - wireMax - 0.4);
    double error_nom = fabs(point_nom.w - wireMax - 0.4);

    if (error_plus < error_minus && error_plus < error_nom) {
      curr_alpha += step_size;
      curr_err = error_plus;
      continue;
    }
    else if (error_minus < error_plus && error_minus < error_nom) {
      curr_alpha -= step_size;
      curr_err = error_minus;
      continue;
    }
    else {
      step_size *= 0.75;
      // curr_err is unchanged here
      continue;
    }

    it ++;
  }

  // std::cout << "  curr_err is " << curr_err << " for alpha at " << curr_alpha << std::endl;

  // After this loop, set the max to the appropriate value:
  if ( curr_alpha < _proto_sps._max_alpha && curr_alpha > _proto_sps._min_alpha) {
    _proto_sps._max_alpha = curr_alpha;
  }
  // projected_point
  //   = geoHelper->Point_3Dto2D(_proto_sps.origin + _proto_sps._max_alpha * _proto_sps.freeDirection,
  //                             plane);
  // std::cout << "  Shifted the max from right in plane " << plane << ", end w is " << projected_point.w << std::endl;

// Get very close to just beyond the edge:
  it = 0;
  curr_err = fabs(projected_point.w - wireMin + 0.4);
  step_size = 1.0;
  curr_alpha = _proto_sps._max_alpha;
  while ( curr_err > 0.1 ) {
    if (it > 100) {
      std::cout << "  iteration maxout" << std::endl;
      break;
    }

    // Make three points, one at the value, one at +/- value
    auto point_minus
      = geoHelper->Point_3Dto2D(_proto_sps.origin + (curr_alpha - step_size) * _proto_sps.freeDirection,
                                plane);
    auto point_plus
      = geoHelper->Point_3Dto2D(_proto_sps.origin + (curr_alpha + step_size) * _proto_sps.freeDirection,
                                plane);
    auto point_nom
      = geoHelper->Point_3Dto2D(_proto_sps.origin + curr_alpha * _proto_sps.freeDirection,
                                plane);

    double error_minus = fabs(point_minus.w - wireMin + 0.4);
    double error_plus = fabs(point_plus.w - wireMin + 0.4);
    double error_nom = fabs(point_nom.w - wireMin + 0.4);

    if (error_plus < error_minus && error_plus < error_nom) {
      curr_alpha += step_size;
      curr_err = error_plus;
      continue;
    }
    else if (error_minus < error_plus && error_minus < error_nom) {
      curr_alpha -= step_size;
      curr_err = error_minus;
      continue;
    }
    else {
      step_size *= 0.75;
      // curr_err is unchanged here
      continue;
    }

    it ++;
  }

  if ( curr_alpha < _proto_sps._max_alpha && curr_alpha > _proto_sps._min_alpha) {
    _proto_sps._min_alpha = curr_alpha;
  }


  // Lastly, set the "real" alpha to the average of the max and min...
  _proto_sps.alpha = 0.5*(_proto_sps._min_alpha + _proto_sps._max_alpha);

}

// void ShowerSPS::fitPointsToSlope(std::vector<ProtoSps> & _proto_sps_v,
//                                  double slope, unsigned int target_plane) {

//   // Basically, start with the first point in the vector of proto_sps.
//   // Then, adjust the next point until the slope of the two points matches the target slope.
//   // Then, adjust the next point ...
//   // Continue until all points have been fitted to the slope.
//   // It's expected that a perfect fit isn't possible, so each point will
//   // just walk along it's allowed values until it has reached a minimum in it's error

//   // Need a collection of sps that are already in the fit:
//   std::vector<unsigned int> _used_sps;
//   // definitely use the first one!
//   _used_sps.push_back(0);

//   // note the target point:
//   unsigned int target_index = _used_sps.back() + 1;
//   auto geoHelper = larutil::GeometryHelper::GetME();


//   while (_used_sps.size() < _proto_sps_v.size()) {
//     double alpha = 5.0;
//     while (alpha > 0.5) {

//       // Make a TPrincipal for each point already used, plus the new one.
//       TPrincipal fPrincipal_nominal(2, "D");
//       TPrincipal fPrincipal_plus(2, "D");
//       TPrincipal fPrincipal_minus(2, "D");
//       for (auto & index : _used_sps ) {
//         double data[2];
//         auto point
//           = geoHelper -> Point_3Dto2D(_proto_sps_v.at(index).xyz.X(),
//                                       _proto_sps_v.at(index).xyz.Y(),
//                                       _proto_sps_v.at(index).xyz.Z(),
//                                       target_plane);
//         data[0] = point.w;
//         data[1] = point.t;
//         fPrincipal_nominal.AddRow(data);
//         fPrincipal_plus.AddRow(data);
//         fPrincipal_minus.AddRow(data);
//       }

//       // And add the target index point:
//       double data[2];
//       auto point_nominal
//         = geoHelper -> Point_3Dto2D(_proto_sps_v.at(target_index).xyz.X(),
//                                     _proto_sps_v.at(target_index).xyz.Y(),
//                                     _proto_sps_v.at(target_index).xyz.Z(),
//                                     target_plane);
//       auto plus_center = _proto_sps_v.at(target_index).xyz + alpha * _proto_sps_v.at(target_index).freeDirection;
//       auto minus_center = _proto_sps_v.at(target_index).xyz - alpha * _proto_sps_v.at(target_index).freeDirection;
//       auto point_plus
//         = geoHelper -> Point_3Dto2D(plus_center.X(),
//                                     plus_center.Y(),
//                                     plus_center.Z(),
//                                     target_plane);
//       auto point_minus
//         = geoHelper -> Point_3Dto2D(minus_center.X(),
//                                     minus_center.Y(),
//                                     minus_center.Z(),
//                                     target_plane);
//       data[0] = point_nominal.w;
//       data[1] = point_nominal.t;
//       fPrincipal_nominal.AddRow(data);
//       data[0] = point_plus.w;
//       data[1] = point_plus.t;
//       fPrincipal_plus.AddRow(data);
//       data[0] = point_minus.w;
//       data[1] = point_minus.t;
//       fPrincipal_minus.AddRow(data);

//       // Now make the principals and get the slopes:
//       fPrincipal_nominal.MakePrincipals();
//       fPrincipal_plus.MakePrincipals();
//       fPrincipal_minus.MakePrincipals();
//       double slope_nominal
//         = ((* fPrincipal_nominal.GetEigenVectors())[1][0]) /
//           ((* fPrincipal_nominal.GetEigenVectors())[0][0]);
//       double slope_plus
//         = ((* fPrincipal_plus.GetEigenVectors())[1][0]) /
//           ((* fPrincipal_plus.GetEigenVectors())[0][0]);
//       double slope_minus
//         = ((* fPrincipal_minus.GetEigenVectors())[1][0]) /
//           ((* fPrincipal_minus.GetEigenVectors())[0][0]);


//       // Next, compare the slopes to the target slope.
//       // Choose the one with the smallest error.  If it's an edge one, move the proto sps to that point.
//       // if it's the center one, keep alpha as it is.

//       double error_nominal = (slope_nominal - slope) / slope;
//       double error_plus = (slope_plus - slope) / slope;
//       double error_minus = (slope_minus - slope) / slope;

//       // std::cout << "alpha is " << alpha << std::endl;

//       // Pick the minimum:
//       if (error_plus < error_nominal && error_plus < error_minus) {
//         _proto_sps_v.at(target_index).xyz += alpha * _proto_sps_v.at(target_index).freeDirection;
//         continue;
//       }
//       else if (error_minus < error_nominal && error_minus < error_plus) {
//         _proto_sps_v.at(target_index).xyz -= alpha * _proto_sps_v.at(target_index).freeDirection;
//         continue;
//       }
//       else {
//         alpha *= 0.75;
//       }
//     }
//     // If this point is reached, the above fit converged.  So add the index.
//     _used_sps.push_back(target_index);
//     target_index += 1;
//   }


//   // Now, at this point, go through the entire list of hits again
//   // and refit all of them just varying one hit at a time.
//   for (size_t _sps_index = 0; _sps_index < _proto_sps_v.size(); _sps_index ++){

//     double alpha = 5.0;
//     while (alpha > 0.5) {
//       // Make a TPrincipal for each point already used, plus the new one.
//       TPrincipal fPrincipal_nominal(2, "D");
//       TPrincipal fPrincipal_plus(2, "D");
//       TPrincipal fPrincipal_minus(2, "D");
//       for (size_t index = 0; index <_proto_sps_v.size();index ++ ) {
//         if (index == _sps_index){
//           continue;
//         }
//         double data[2];
//         auto point
//           = geoHelper -> Point_3Dto2D(_proto_sps_v.at(index).xyz.X(),
//                                       _proto_sps_v.at(index).xyz.Y(),
//                                       _proto_sps_v.at(index).xyz.Z(),
//                                       target_plane);
//         data[0] = point.w;
//         data[1] = point.t;
//         fPrincipal_nominal.AddRow(data);
//         fPrincipal_plus.AddRow(data);
//         fPrincipal_minus.AddRow(data);
//       }

//       // And add the target index point:
//       double data[2];
//       auto point_nominal
//         = geoHelper -> Point_3Dto2D(_proto_sps_v.at(_sps_index).xyz.X(),
//                                     _proto_sps_v.at(_sps_index).xyz.Y(),
//                                     _proto_sps_v.at(_sps_index).xyz.Z(),
//                                     target_plane);
//       auto plus_center = _proto_sps_v.at(_sps_index).xyz + alpha * _proto_sps_v.at(_sps_index).freeDirection;
//       auto minus_center = _proto_sps_v.at(_sps_index).xyz - alpha * _proto_sps_v.at(_sps_index).freeDirection;
//       auto point_plus
//         = geoHelper -> Point_3Dto2D(plus_center.X(),
//                                     plus_center.Y(),
//                                     plus_center.Z(),
//                                     target_plane);
//       auto point_minus
//         = geoHelper -> Point_3Dto2D(minus_center.X(),
//                                     minus_center.Y(),
//                                     minus_center.Z(),
//                                     target_plane);
//       data[0] = point_nominal.w;
//       data[1] = point_nominal.t;
//       fPrincipal_nominal.AddRow(data);
//       data[0] = point_plus.w;
//       data[1] = point_plus.t;
//       fPrincipal_plus.AddRow(data);
//       data[0] = point_minus.w;
//       data[1] = point_minus.t;
//       fPrincipal_minus.AddRow(data);

//       // Now make the principals and get the slopes:
//       fPrincipal_nominal.MakePrincipals();
//       fPrincipal_plus.MakePrincipals();
//       fPrincipal_minus.MakePrincipals();
//       double slope_nominal
//         = ((* fPrincipal_nominal.GetEigenVectors())[1][0]) /
//           ((* fPrincipal_nominal.GetEigenVectors())[0][0]);
//       double slope_plus
//         = ((* fPrincipal_plus.GetEigenVectors())[1][0]) /
//           ((* fPrincipal_plus.GetEigenVectors())[0][0]);
//       double slope_minus
//         = ((* fPrincipal_minus.GetEigenVectors())[1][0]) /
//           ((* fPrincipal_minus.GetEigenVectors())[0][0]);


//       // Next, compare the slopes to the target slope.
//       // Choose the one with the smallest error.  If it's an edge one, move the proto sps to that point.
//       // if it's the center one, keep alpha as it is.

//       double error_nominal = (slope_nominal - slope) / slope;
//       double error_plus = (slope_plus - slope) / slope;
//       double error_minus = (slope_minus - slope) / slope;

//       // std::cout << "alpha is " << alpha << std::endl;

//       // Pick the minimum:
//       if (error_plus < error_nominal && error_plus < error_minus) {
//         _proto_sps_v.at(_sps_index).xyz += alpha * _proto_sps_v.at(_sps_index).freeDirection;
//         continue;
//       }
//       else if (error_minus < error_nominal && error_minus < error_plus) {
//         _proto_sps_v.at(_sps_index).xyz -= alpha * _proto_sps_v.at(_sps_index).freeDirection;
//         continue;
//       }
//       else {
//         alpha *= 0.75;
//       }
//     }
//   }


//   return;
// }

bool ShowerSPS::finalize() {

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
