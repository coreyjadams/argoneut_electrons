/**
 * \file BasicFilter.h
 *
 * \ingroup filter
 *
 * \brief Class def header for a class BasicFilter
 *
 * @author cadams
 */

/** \addtogroup filter

    @{*/

#ifndef ARGOFILTER_BASICFILTER_H
#define ARGOFILTER_BASICFILTER_H

#include "Analysis/ana_base.h"
#include "ClusterRecoUtil/Base/CRUHelper.h"
#include "ClusterRecoUtil/Alg/DefaultParamsAlg.h"

namespace argofilter {
/**
   \class BasicFilter
   User custom analysis class made by SHELL_USER_NAME
 */
class BasicFilter : public ::larlite::ana_base {

public:

  /// Default constructor
  BasicFilter() { _name = "BasicFilter"; _fout = 0;}

  /// Default destructor
  virtual ~BasicFilter() {}

  /** IMPLEMENT in BasicFilter.cc!
      Initialization method to be called before the analysis event loop.
  */
  virtual bool initialize();

  /** IMPLEMENT in BasicFilter.cc!
      Analyze a data event-by-event
  */
  virtual bool analyze(::larlite::storage_manager* storage);

  /** IMPLEMENT in BasicFilter.cc!
      Finalize method to be called after all events processed.
  */
  virtual bool finalize();

  void SetInputProducer(std::string s){_input_producer = s;}

protected:


  std::string _input_producer;



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
