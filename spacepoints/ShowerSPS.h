/**
 * \file ShowerSPS  .h
 *
 * \ingroup spacepoints
 *
 * \brief Class def header for a class ShowerSPS
 *
 * @author cadams
 */

/** \addtogroup spacepoints

    @{*/

#ifndef LARLITE_SHOWERSPS_H
#define LARLITE_SHOWERSPS_H

#include "Analysis/ana_base.h"
#include "TVector3.h"
#include "ClusterRecoUtil/Alg/DefaultParamsAlg.h"
#include "ClusterRecoUtil/Base/CRUHelper.h"

#include "DataFormat/pfpart.h"
#include "DataFormat/spacepoint.h"
#include "DataFormat/cluster.h"
#include "DataFormat/hit.h"
#include "DataFormat/vertex.h"

#include "TPrincipal.h"

namespace argosps {
/**
   \class ShowerSPS
   User custom analysis class made by SHELL_USER_NAME
 */

class ProtoSps {

public:
  // The current best estimate of the xyz position:
  TVector3 xyz() {return origin + alpha * freeDirection;}

  double alpha;
  TVector3 origin;
  TVector3 freeDirection;

  double _max_alpha, _min_alpha;
  unsigned int _matched_hit_index;
  unsigned int _params_hit_index;
  int _origin_plane;

};

class ShowerSPS  : public larlite::ana_base {

public:

  /// Default constructor
  ShowerSPS () { _name = "ShowerSPS  "; _fout = 0;}

  /// Default destructor
  virtual ~ShowerSPS  () {}

  /** IMPLEMENT in ShowerSPS  .cc!
      Initialization method to be called before the analysis event loop.
  */
  virtual bool initialize();

  /** IMPLEMENT in ShowerSPS  .cc!
      Analyze a data event-by-event
  */
  virtual bool analyze(larlite::storage_manager* storage);

  /** IMPLEMENT in ShowerSPS  .cc!
      Finalize method to be called after all events processed.
  */
  virtual bool finalize();

protected:

  ::cluster::CRUHelper _cru_helper;
  ::cluster::DefaultParamsAlg _params_alg;


  double getSlope(const ::cluster::cluster_params & _params,
                  const std::vector<ProtoSps> & _proto_sps_v);

  void fitPointsToSlope(std::vector<ProtoSps> & _proto_sps_v,
                        double slope, unsigned int target_plane);

  void restrictRange(ProtoSps & _proto_sps, double wireMin, double wireMax, unsigned int plane);

  void stretchSpacePoints(std::vector<ProtoSps> & _proto_sps, unsigned int plane);

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
