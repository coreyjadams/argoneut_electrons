/**
 * \file VertexCondenser.h
 *
 * \ingroup vertex
 *
 * \brief Class def header for a class VertexCondenser
 *
 * @author cadams
 */

/** \addtogroup vertex

    @{*/

#ifndef LARLITE_VERTEXCONDENSER_H
#define LARLITE_VERTEXCONDENSER_H

#include "Analysis/ana_base.h"

namespace argovertex {
/**
   \class VertexCondenser
   User custom analysis class made by SHELL_USER_NAME
 */
class VertexCondenser : public larlite::ana_base {

public:

  /// Default constructor
  VertexCondenser() { _name = "VertexCondenser"; _fout = 0;}

  /// Default destructor
  virtual ~VertexCondenser() {}

  /** IMPLEMENT in VertexCondenser.cc!
      Initialization method to be called before the analysis event loop.
  */
  virtual bool initialize();

  /** IMPLEMENT in VertexCondenser.cc!
      Analyze a data event-by-event
  */
  virtual bool analyze(larlite::storage_manager* storage);

  /** IMPLEMENT in VertexCondenser.cc!
      Finalize method to be called after all events processed.
  */
  virtual bool finalize();

protected:

  float _minDist2d;


  std::vector<size_t>  condeseIndex(size_t index,
                                    std::vector<std::vector<size_t>> & input );


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
