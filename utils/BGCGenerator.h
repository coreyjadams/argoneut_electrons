/**
 * \file BGCGenerator.h
 *
 * \ingroup utils
 *
 * \brief Class def header for a class BGCGenerator
 *
 * @author cadams
 */

/** \addtogroup utils

    @{*/

#ifndef LARLITE_BGCGENERATOR_H
#define LARLITE_BGCGENERATOR_H

#include "Analysis/ana_base.h"

#include "BGCFileReader.h"

#include "DataFormat/hit.h"
#include "DataFormat/cluster.h"
#include "DataFormat/pfpart.h"
#include "DataFormat/vertex.h"

namespace argoutils {
/**
   \class BGCGenerator
   User custom analysis class made by SHELL_USER_NAME
 */
class BGCGenerator : public larlite::ana_base {

public:

  /// Default constructor
  BGCGenerator() { _name = "BGCGenerator"; _fout = 0;}

  /// Default destructor
  virtual ~BGCGenerator() {}

  /** IMPLEMENT in BGCGenerator.cc!
      Initialization method to be called before the analysis event loop.
  */
  virtual bool initialize();

  /** IMPLEMENT in BGCGenerator.cc!
      Analyze a data event-by-event
  */
  virtual bool analyze(larlite::storage_manager* storage);

  /** IMPLEMENT in BGCGenerator.cc!
      Finalize method to be called after all events processed.
  */
  virtual bool finalize();

  void setBGCFile(std::string s);

protected:

  std::vector<unsigned int> buildCluster(const BCGData&  bgclust,
                                         larlite::event_hit * ev_hit);

  BGCFileReader _bfr;

  std::map< int, std::map< int,  std::vector<BCGData> > > _clusterData;


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
