#ifndef LARLITE_LINEINTERSECTIONVERTEXER_CXX
#define LARLITE_LINEINTERSECTIONVERTEXER_CXX

#include "LineIntersectionVertexer.h"
#include "DataFormat/cluster.h"
#include "DataFormat/endpoint2d.h"

#include "LArUtil/Geometry.h"
#include "LArUtil/GeometryHelper.h"
#include "LArUtil/DetectorProperties.h"

#include "Base/GeoConstants.h"


namespace argovertex {


LineIntersectionVertexer::LineIntersectionVertexer() {
  _name = "LineIntersectionVertexer";
  _fout = 0;
  _cluster_producer = "cccluster";
  _min_hits = 20;
}

bool LineIntersectionVertexer::initialize() {

  //
  // This function is called in the beggining of event loop
  // Do all variable initialization you wish to do here.
  // If you have a histogram to fill in the event loop, for example,
  // here is a good place to create one on the heap (i.e. "new TH1D").
  //

  _params_alg.SetVerbose(false);
  _params_alg.SetDebug(false);
  _params_alg.SetMinHits(_min_hits);


  auto geom = larutil::Geometry::GetME();
  auto geomHelper = larutil::GeometryHelper::GetME();
  auto detProp = larutil::DetectorProperties::GetME();



  // Here, set up the min and max values for each plane in wire and time
  boundaries.resize(geom -> Nplanes());
  for (size_t p = 0; p < geom -> Nplanes(); p++) {

    double * origin;
    origin = new double[geom->Nviews()];
    geom->PlaneOriginVtx(p, origin);
    float planeOffset = origin[0];

    boundaries.at(p).x_min = 0;
    boundaries.at(p).x_max = geom -> Nwires(p) * geomHelper -> WireToCm();
    boundaries.at(p).y_min
      = (-detProp->TriggerOffset()) * geomHelper->TimeToCm() + planeOffset;
    boundaries.at(p).y_max
      = (detProp->ReadOutWindowSize() - detProp->TriggerOffset())
        * geomHelper->TimeToCm() + planeOffset;

    delete origin;

  }

  return true;
}

bool LineIntersectionVertexer::analyze(larlite::storage_manager* storage) {

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

  // Attempt to get the output at the front:
  auto endpoint2d_v =  storage -> get_data<larlite::event_endpoint2d>("tempName");
  auto out_ass = storage->get_data<larlite::event_ass>(endpoint2d_v->name());
  // This ensures that the tree exists for each event, even if there are no clusters.

  // Get the clusters for this event:
  auto cluster_v = storage -> get_data<larlite::event_cluster>(_cluster_producer);

  storage ->set_id(cluster_v -> run(), cluster_v->subrun(), cluster_v ->event_id());

  if (!cluster_v) {
    std::cerr << "ERROR: no event cluster gotten from event." << std::endl;
    return false;
  }

  if (cluster_v->size() == 0) {
    std::cerr << "Skipping this event because no clusters found." << std::endl;
    return false;
  }



  std::vector<interceptCand> listOfIntercepts;

  // Fill the cluster params alg
  std::vector<cluster::cluster_params> params_v;
  _cru_helper.GenerateParams( storage, _cluster_producer, params_v);


  // Since we need the axis of the clusters, compute their params just one:
  for (auto & clust : params_v) {
    _params_alg.FillParams(clust);
  }


  // Now, loop over every cluster of size greater than _min_hits
  for (size_t i_cluster_1 = 0; i_cluster_1 < params_v.size(); ++i_cluster_1)
  {
    // Skip clusters that are too small:
    if (params_v.at(i_cluster_1).N_Hits < _min_hits)
      continue;

    // cut clusters that aren't particularly straight, too:
    if (params_v.at(i_cluster_1).eigenvalue_principal < 0.9)
      continue;

    // Start the interior loop off by one
    for (size_t i_cluster_2 = i_cluster_1 + 1; i_cluster_2 < params_v.size(); ++i_cluster_2)
    {
      // Again skip clusters that are too small:
      if (params_v.at(i_cluster_2).N_Hits < _min_hits)
        continue;

      if (params_v.at(i_cluster_2).eigenvalue_principal < 0.9)
        continue;

      // Also skip clusters from different planes:
      if (params_v.at(i_cluster_1).plane_id.Plane != params_v.at(i_cluster_2).plane_id.Plane )
        continue;

      // Now, take this pair of clusters and see if their axes intercept anywhere
      auto intercept = IntersectionPoint(params_v.at(i_cluster_1), params_v.at(i_cluster_2));

      if (isInPlane(intercept, params_v.at(i_cluster_1).plane_id.Plane)) {
        float strength = 1;

        listOfIntercepts.push_back(
          interceptCand( intercept,
                         strength,
                         i_cluster_1,
                         i_cluster_2));
        // std::cout << "Plane " << params_v.at(i_cluster_1).plane_id.Plane << ": ("
        //           << params_v.at(i_cluster_1).start_point.w << ", "
        //           << params_v.at(i_cluster_1).start_point.t << ") "
        //           << "intersects at ("
        //           << params_v.at(i_cluster_2).start_point.w << ", "
        //           << params_v.at(i_cluster_2).start_point.t << ") "
        //           << "at (" << intercept.w << ", " << intercept.t << ")."
        //           << std::endl;
      }

      // If they don't intercept within the TPC, continue
      //
      //
      // If they DO intercept within the WIRE/TIME coordinates, store the intercept:




    }
  }


  // For each intercept, create an endpoint2d and associate it to the clusters
  // that created it

  auto geoHelper = larutil::GeometryHelper::GetME();

  size_t i = 0;

  larlite::AssSet_t ep_ass;

  for (auto & intc : listOfIntercepts ) {
    endpoint2d_v -> emplace_back(
      larlite::endpoint2d( intc.intercept.t / geoHelper -> TimeToCm(),
                           intc.intercept.w / geoHelper -> WireToCm(),
                           1.0,
                           i,
                           (larlite::geo::View_t) intc.intercept.plane,
                           1.0));


    larlite::AssUnit_t new_association;
    new_association.push_back(intc.cluster_index_1);
    new_association.push_back(intc.cluster_index_2);
    ep_ass.push_back(new_association);
    i ++;
  }

  out_ass->set_association(endpoint2d_v->id(), cluster_v->id(), ep_ass);

  std::cout << "Event " << storage -> event_id()
            << ", endpoint2d_v.size() is "
            << endpoint2d_v->size() << std::endl;

  // storage -> set_id(1, 0, endpoint2d_v->event_id());


  return true;
}

bool LineIntersectionVertexer::finalize() {

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


larutil::Point2D LineIntersectionVertexer::IntersectionPoint(
  cluster::cluster_params & clust1,
  cluster::cluster_params & clust2) {

  // This is a slightly complicated formula, read up on how to
  //  calculate the intersection of points here:
  // https://www.topcoder.com/community/data-science/data-science
  // -tutorials/geometry-concepts-line-intersection-and-its-applications/
  //

  larutil::Point2D returnPoint(clust1.plane_id.Plane,
                      std::numeric_limits<double>::max(),
                      std::numeric_limits<double>::max());

  // Use this section to change what gets used to define the line.
  // p is the point, v is the direction
  double clust1_p1x, clust1_p2x, clust1_p1y, clust1_p2y;
  double clust2_p1x, clust2_p2x, clust2_p1y, clust2_p2y;

  clust1_p1x = clust1.mean_x;
  clust1_p1y = clust1.mean_y;
  clust2_p1x = clust2.mean_x;
  clust2_p1y = clust2.mean_y;

  clust1_p2x = clust1.mean_x + clust1.principal_dir[0];
  clust1_p2y = clust1.mean_y + clust1.principal_dir[1];
  clust2_p2x = clust2.mean_x + clust2.principal_dir[0];
  clust2_p2y = clust2.mean_y + clust2.principal_dir[1];


  double clust1_A = clust1_p2y - clust1_p1y;
  double clust2_A = clust2_p2y - clust2_p1y;
  double clust1_B = clust1_p1x - clust1_p2x;
  double clust2_B = clust2_p1x - clust2_p2x;
  double clust1_C = clust1_A * clust1_p1x + clust1_B * clust1_p1y;
  double clust2_C = clust2_A * clust2_p1x + clust2_B * clust2_p1y;



  //
  //
  double determinant = clust1_A * clust2_B - clust2_A * clust1_B;

  // Check that the determinant isn't zero:
  if ( determinant == 0) {
    // return because can't divide by zero.
    return returnPoint;
  }
  else {
    returnPoint.w = (clust2_B * clust1_C - clust1_B * clust2_C) / determinant;
    returnPoint.t = (clust1_A * clust2_C - clust2_A * clust1_C) / determinant;

    return returnPoint;
  }

}

bool LineIntersectionVertexer::isInPlane(larutil::Point2D & point, unsigned int plane) {

  // Check the wire coordinate:
  if (point.w < boundaries.at(plane).x_min ||
      point.w > boundaries.at(plane).x_max)
    return false;
  if (point.t < boundaries.at(plane).y_min ||
      point.t > boundaries.at(plane).y_max)
    return false;

  // Default, return true
  return true;
}


}
#endif
