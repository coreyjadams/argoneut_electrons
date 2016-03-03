/**
 * \file ProductSps.h
 *
 * \ingroup spacepoints
 *
 * \brief Class def header for a class ProductSps
 *
 * @author cadams
 */

/** \addtogroup spacepoints

    @{*/

#ifndef LARLITE_PRODUCTSPS_H
#define LARLITE_PRODUCTSPS_H

#include "Analysis/ana_base.h"
#include "TVector3.h"

namespace larlite {
/**
   \class ProductSps
   User custom analysis class made by SHELL_USER_NAME
 */
class ProductSps : public ana_base {

public:

  /// Default constructor
  ProductSps() { _name = "ProductSps"; _fout = 0;}

  /// Default destructor
  virtual ~ProductSps() {}

  /** IMPLEMENT in ProductSps.cc!
      Initialization method to be called before the analysis event loop.
  */
  virtual bool initialize();

  /** IMPLEMENT in ProductSps.cc!
      Analyze a data event-by-event
  */
  virtual bool analyze(storage_manager* storage);

  /** IMPLEMENT in ProductSps.cc!
      Finalize method to be called after all events processed.
  */
  virtual bool finalize();

protected:

  void fillVoxels(double * start_point, double * end_point, int plane);

  int calculateVoxel(const TVector3 &);

  int _n_voxels_x;
  int _n_voxels_y;
  int _n_voxels_z;

  float _x_min, _x_max;
  float _y_min, _y_max;
  float _z_min, _z_max;

  std::vector<std::vector<std::vector<float> > > _voxels;

};
}
#endif

//**************************************************************************
//
// For Analysis framework documentation, read Manual.pdf here:
//
// http://microboone-docdb.fnal.gov:8080/cgi-bin/ShowDocument?docid=3183
//
//**************************************************************************

/** @} */ // end of doxygen group
