/**
 * \file dEdxPfPartAna.h
 *
 * \ingroup analysis
 *
 * \brief Class def header for a class dEdxPfPartAna
 *
 * @author cadams
 */

/** \addtogroup analysis

    @{*/

#ifndef LARLITE_DEDXPFPARTANA_H
#define LARLITE_DEDXPFPARTANA_H

#include "Analysis/ana_base.h"
#include "DataFormat/shower.h"
#include "DataFormat/cluster.h"
#include "DataFormat/hit.h"
#include "DataFormat/pfpart.h"
#include "DataFormat/vertex.h"
#include "DataFormat/endpoint2d.h"
#include "LArUtil/Geometry.h"
#include "TVector3.h"

#include "ClusterRecoUtil/Alg/DefaultParamsAlg.h"
#include "ClusterRecoUtil/Base/CRUHelper.h"
#include "Cluster3DRecoUtil/Alg/Default3DParamsAlg.h"
#include "Cluster3DRecoUtil/Base/CRU3DHelper.h"

#include "TTree.h"

namespace argoana {
/**
   \class dEdxPfPartAna
   User custom analysis class made by SHELL_USER_NAME
 */
class dEdxPfPartAna : public larlite::ana_base {

public:

  /// Default constructor
  dEdxPfPartAna();

  /// Default destructor
  ~dEdxPfPartAna() {}

  /** IMPLEMENT in dEdxPfPartAna.cc!
      Initialization method to be called before the analysis event loop.
  */
  virtual bool initialize();

  /** IMPLEMENT in dEdxPfPartAna.cc!
      Analyze a data event-by-event
  */
  virtual bool analyze(larlite::storage_manager* storage);

  /** IMPLEMENT in dEdxPfPartAna.cc!
      Finalize method to be called after all events processed.
  */
  virtual bool finalize();

  void getClosestHits(larlite::event_hit * ev_hit, std::vector<unsigned int> hits_to_consider,
                      std::vector<size_t> & close_hit_indexes,
                      const larlite::endpoint2d & );

  double getPitch(const TVector3 & dir3D, int plane );


protected:

  void unwindVectors(larlite::event_hit * ev_hit, std::vector<size_t> & close_hit_indexes);

  Point2D findClosestHit(std::vector<Hit2D> hit_vector,
                         std::vector<unsigned int> hit_indexes,
                         Point2D start_point);
  
  const larutil::Geometry * geom;

  ::cluster::CRUHelper _cru_helper;
  ::cluster::DefaultParamsAlg _params_alg;
  ::cluster3D::CRU3DHelper _cru3D_helper;
  ::cluster3D::Default3DParamsAlg _params3D_alg;

  TTree * tree;

  //
  // Variables to save into the ttree:
  //
  int run;
  int event;
  std::vector<double> fDCosStart; // the 3D axis

  std::vector<double> collection_hittimes;
  std::vector<double> collection_hitwires;
  std::vector<double> collection_hitcharges;
  std::vector<double> collection_hitpeaks;
  std::vector<double> collection_starthit;
  std::vector<double> collection_lifetime_corr;
  std::vector<double> collection_dist_from_start;
  double collection_slope;
  double collection_pitch;
  double collection_pitch_err;

  std::vector<double> induction_hittimes;
  std::vector<double> induction_hitwires;
  std::vector<double> induction_hitcharges;
  std::vector<double> induction_hitpeaks;
  std::vector<double> induction_starthit;
  std::vector<double> induction_lifetime_corr;
  std::vector<double> induction_dist_from_start;
  double induction_slope;
  double induction_pitch;
  double induction_pitch_err;


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
