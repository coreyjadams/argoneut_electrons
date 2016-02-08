/**
 * \file VertexPlaneMatch.h
 *
 * \ingroup vertex
 * 
 * \brief Class def header for a class VertexPlaneMatch
 *
 * @author cadams
 */

/** \addtogroup vertex

    @{*/

#ifndef LARLITE_VERTEXPLANEMATCH_H
#define LARLITE_VERTEXPLANEMATCH_H

#include "Analysis/ana_base.h"

namespace larlite {
  /**
     \class VertexPlaneMatch
     User custom analysis class made by SHELL_USER_NAME
   */
  class VertexPlaneMatch : public ana_base{
  
  public:

    /// Default constructor
    VertexPlaneMatch(){ _name="VertexPlaneMatch"; _fout=0;}

    /// Default destructor
    virtual ~VertexPlaneMatch(){}

    /** IMPLEMENT in VertexPlaneMatch.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in VertexPlaneMatch.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in VertexPlaneMatch.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

  protected:
    
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
