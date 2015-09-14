/**
 * \file ShowerFilter.h
 *
 * \ingroup filter
 *
 * \brief Class def header for a class ShowerFilter
 *
 * @author cadams
 */

/** \addtogroup filter

    @{*/

#ifndef ARGOFILTER_SHOWERFILTER_H
#define ARGOFILTER_SHOWERFILTER_H

#include "Analysis/ana_base.h"
#include "ClusterRecoUtil/Base/CRUHelper.h"
#include "ClusterRecoUtil/Alg/DefaultParamsAlg.h"
#include "utils/TrackShower.h"

namespace argofilter {
/**
   \class ShowerFilter
   User custom analysis class made by SHELL_USER_NAME
 */
class ShowerFilter : public ::larlite::ana_base {

public:

  /// Default constructor
  ShowerFilter() { _name = "ShowerFilter"; _fout = 0;}

  /// Default destructor
  virtual ~ShowerFilter() {}

  /** IMPLEMENT in ShowerFilter.cc!
      Initialization method to be called before the analysis event loop.
  */
  virtual bool initialize();

  /** IMPLEMENT in ShowerFilter.cc!
      Analyze a data event-by-event
  */
  virtual bool analyze(::larlite::storage_manager* storage);

  /** IMPLEMENT in ShowerFilter.cc!
      Finalize method to be called after all events processed.
  */
  virtual bool finalize();

protected:

  // Parameters:
  size_t _min_hits;

  std::string _input_producer;
  ::cluster::DefaultParamsAlg _params_alg;
  ::cluster::CRUHelper _cru_helper;

  ::argoutils::TrackShower ts;


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
