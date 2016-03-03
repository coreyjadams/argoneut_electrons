#ifndef LARLITE_PRODUCTSPS_CXX
#define LARLITE_PRODUCTSPS_CXX

#include "ProductSps.h"
#include "DataFormat/pfpart.h"
#include "DataFormat/spacepoint.h"
#include "DataFormat/cluster.h"
#include "DataFormat/hit.h"
#include "LArUtil/Geometry.h"
#include "LArUtil/GeometryHelper.h"

namespace larlite {

bool ProductSps::initialize() {

  //
  // This function is called in the beggining of event loop
  // Do all variable initialization you wish to do here.
  // If you have a histogram to fill in the event loop, for example,
  // here is a good place to create one on the heap (i.e. "new TH1D").
  //

  auto geom = larutil::Geometry::GetME();


  _x_min = 0;
  _x_max = 2 * geom -> DetHalfWidth();
  _y_min = - geom -> DetHalfHeight();
  _y_max = geom -> DetHalfHeight();
  _z_min = 0;
  _z_max = geom -> DetLength();



  _n_voxels_x = 40;
  _n_voxels_y = 45;
  _n_voxels_z = 90;

  return true;
}

bool ProductSps::analyze(storage_manager* storage) {

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


  // Look for the hits that come from the clusters from the input
  // pfparticles.
  //
  auto ev_pfpart = storage -> get_data<larlite::event_pfpart>("ccMatched");

  // Make space for the output spacepoints:
  auto ev_sps = storage -> get_data<larlite::event_spacepoint>("spsTest");
  auto out_ass = storage->get_data<larlite::event_ass>(ev_sps->name());

  // set event ID through storage manager
  storage->set_id(storage->get_data<event_pfpart>("ccMatched")->run(),
                  storage->get_data<event_pfpart>("ccMatched")->subrun(),
                  storage->get_data<event_pfpart>("ccMatched")->event_id());



  if (! ev_pfpart) {
    return true;
  }


  ::larlite::event_cluster * ev_clust = nullptr;
  auto const& ass_cluster_v
    = storage->find_one_ass(ev_pfpart->id(), ev_clust, ev_pfpart->name());

  if (!ev_clust) {
    return true;
  }

  // Want to get the hits from the cluster too:
  ::larlite::event_hit * ev_hit = nullptr;
  auto const& ass_hit_v
    = storage->find_one_ass(ev_clust->id(), ev_hit, ev_clust->name());

  std::cout << "Number of pfpart in this event: " << ev_pfpart->size() << std::endl;
  std::cout << "Number of clusters in this event: " << ev_clust->size() << std::endl;
  std::cout << "Number of hits in this event: " << ev_hit->size() << std::endl;


  auto geom = larutil::Geometry::GetME();
  auto geoHelper = larutil::GeometryHelper::GetME();

  // Define a 1cm by 1cm voxel matrix here

  _voxels.clear();
  _voxels.resize(_n_voxels_x);
  for (auto & vox : _voxels ) {
    vox.resize(_n_voxels_y);
    for (auto & subvox : vox ) {
      subvox.resize(_n_voxels_z);
    }
  }


  double  xyzStart[3];
  double  xyzEnd[3];

  // Now, loop over the hits in each cluster, from each plane, and
  // fill in voxels if a hit could go there.
  //
  for (size_t i_pfpart = 0; i_pfpart < ev_pfpart->size(); i_pfpart ++) {

    for (size_t i_clust : ass_cluster_v.at(i_pfpart)) {

      std::cout << "On cluster " << i_clust
                << " with " << ass_hit_v.at(i_clust).size() << " hits."
                << std::endl;

      // Loop over all the hits in this cluster:
      for (size_t i_hit : ass_hit_v.at(i_clust)) {

        // std::cout << "On hit " << i_hit << std::endl;

        // For this hit, figure out the min and max allowed points,
        // and then connect them with a line.
        auto & hit = ev_hit->at(i_hit);

        // std::cout << "Plane is " << hit.WireID().Plane << std::endl;

        geom -> WireEndPoints(hit.WireID().Plane,
                              hit.WireID().Wire,
                              xyzStart,
                              xyzEnd);

        // Need to correct for the offset of the hit itself:
        xyzStart[0] = hit.PeakTime() * geoHelper->TimeToCm() - xyzStart[0];
        xyzEnd[0] = hit.PeakTime() * geoHelper->TimeToCm() - xyzEnd[0];

        // std::cout << "ystart is " << xyzStart[1] << ", yend is " << xyzEnd[1] << std::endl;
        // Fill in the voxels for this hit:
        fillVoxels(xyzStart, xyzEnd, hit.WireID().Plane);

      }
    }
  }

  // Now that all the voxels are filled up, make space points
  // if there are multiple hits:
  int id_index = 0;
  for (int _x_index = 0; _x_index < _voxels.size(); _x_index ++) {
    for (int _y_index = 0; _y_index < _voxels.at(_x_index).size(); _y_index ++) {
      for (int _z_index = 0; _z_index < _voxels.at(_x_index).at(_y_index).size(); _z_index ++) {

        if (_voxels[_x_index][_y_index][_z_index] < 2)
          continue;
        else {
          std::cout << "Have value of " << _voxels[_x_index][_y_index][_z_index] << std::endl;
          // figure out the center of this voxel and make a space point
          // there
          // Figure out the remainder from _n_voxels_z * _n_voxels_y

          float x_point = ( (float) _x_index / _n_voxels_x) * (_x_max - _x_min);
          float y_point = ( (float) _y_index / _n_voxels_y) * (_y_max - _y_min) - 0.5 * (_y_max - _y_min);
          float z_point = ( (float) _z_index / _n_voxels_z) * (_z_max - _z_min);
          // int x_remainder = index % (_n_voxels_z * _n_voxels_y);

          // float y_point = x_remainder / _n_voxels_z;
          // float y_remainder = x_remainder % _n_voxels_z;

          ev_sps -> push_back(spacepoint(id_index,
                                         x_point,
                                         y_point,
                                         z_point,
                                         0,
                                         0,
                                         0,
                                         0));

        }
      }
    }
  }

  // if
  std::cout << "Made " << ev_sps -> size() << " spacepoints." << std::endl;


  return true;
}

void ProductSps::fillVoxels(double * start_point, double * end_point, int plane) {

  // Just doing this the stupid way:
  // start at the start point, move towards the endpoint
  // in steps, and when a new voxel is entered note it.
  //
  // Use a map to keep track of the voxels entered, and copy it into
  // the voxel array at the end
  //
  // Use a separate function to figure out what voxel we're in

  TVector3 start(start_point);
  TVector3 current_point(start_point);
  TVector3 direction;
  direction.SetX(end_point[0] - start_point[0]);
  direction.SetY(end_point[1] - start_point[1]);
  direction.SetZ(end_point[2] - start_point[2]);

  float length = direction.Mag();

  // Normalize the direction:
  direction *= 1.0 / length;

  // Now set up a map:
  std::map < size_t, std::map<size_t, std::map<size_t, int> > > _found_voxels;


  float nSteps = 100;
  float stepSize = length / nSteps;

  for (float i = 0; i <= nSteps; i ++) {
    current_point = start + stepSize * i * direction;
    // std::cout << "Start is " << start.Y() << std::endl;
    // std::cout << "Current point is " << current_point.Y() << std::endl;
    // std::cout << "End is " << end_point[1] << std::endl;

    // Verify this point is within the boundaries:
    if (current_point.X() > _x_max || current_point.X() < _x_min) {
      // std::cout << "current_point.X() is " << current_point.X() << std::endl;
      continue;
    }
    if (current_point.Y() > _y_max || current_point.Y() < _y_min) {
      // std::cout << "current_point.Y() is " << current_point.Y() << std::endl;
      continue;
    }
    if (current_point.Z() > _z_max || current_point.Z() < _z_min) {
      // std::cout << "current_point.Z() is " << current_point.Z() << std::endl;
      continue;
    }

    int _x_index = _n_voxels_x * (current_point.X() - _x_min) / (_x_max - _x_min);
    int _y_index = _n_voxels_y * (current_point.Y() - _y_min) / (_y_max - _y_min);
    int _z_index = _n_voxels_z * (current_point.Z() - _z_min) / (_z_max - _z_min);

    _found_voxels[_x_index][_y_index][_z_index] ++;

    // if (plane == 0){
    //   _voxels[_x_index][_y_index][_z_index] ++;
    // }
    // else{
    //   _voxels[_x_index][_y_index][_z_index] *= 2.0;
    // }
  }

  // std::cout << "found voxels: " << _found_voxels.size() << std::endl;

  // Now fill in the voxels for real:
  for (auto & voxel_1 : _found_voxels) {
    for (auto & voxel_2 : voxel_1.second) {
      for (auto & voxel_z : voxel_2.second) {
        if (plane == 0) {
          if (_voxels[voxel_1.first][voxel_2.first][voxel_z.first] == 0)
            _voxels[voxel_1.first][voxel_2.first][voxel_z.first] ++;
        }
        else {
          if (_voxels[voxel_1.first][voxel_2.first][voxel_z.first] == 1)
            _voxels[voxel_1.first][voxel_2.first][voxel_z.first] ++;
        }
        // std::cout << "Added to the voxel at " << voxel.first
        //           << " (val = " << voxel.second << ")"
        //           << std::endl;
      }
    }
  }

  return;

}

int ProductSps::calculateVoxel(const TVector3 & point) {

  // Verify this point is within the boundaries:
  if (point.X() > _x_max || point.X() < _x_min) {
    // std::cout << "point.X() is " << point.X() << std::endl;
    return -1;
  }
  if (point.Y() > _y_max || point.Y() < _y_min) {
    // std::cout << "point.Y() is " << point.Y() << std::endl;
    return -1;
  }
  if (point.Z() > _z_max || point.Z() < _z_min) {
    // std::cout << "point.Z() is " << point.Z() << std::endl;
    return -1;
  }

  // Use the min and max values and the step sizes to figure out
  // the voxel for this point
  int _x_index = _n_voxels_x * (point.X() - _x_min) / (_x_max - _x_min);
  int _y_index = _n_voxels_y * (point.Y() - _y_min) / (_y_max - _y_min);
  int _z_index = _n_voxels_z * (point.Z() - _z_min) / (_z_max - _z_min);

  // Indexing is [x][y][z], so convert that to a total number with
  // int index = _z_index + _y_index * (_n_voxels_z) + _x_index * (_n_voxels_z * _n_voxels_y);
  // std::cout << "Returning index " << index << std::endl;
  return  _z_index +
          _y_index * (_n_voxels_z) +
          _x_index * (_n_voxels_z * _n_voxels_y);

}


bool ProductSps::finalize() {

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
