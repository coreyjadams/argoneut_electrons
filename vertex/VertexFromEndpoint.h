/**
 * \file VertexFromEndpoint.h
 *
 * \ingroup vertex
 * 
 * \brief Class def header for a class VertexFromEndpoint
 *
 * @author cadams
 */

/** \addtogroup vertex

    @{*/

#ifndef LARLITE_VERTEXFROMENDPOINT_H
#define LARLITE_VERTEXFROMENDPOINT_H

#include "DataFormat/vertex.h"
#include "DataFormat/endpoint2d.h"
#include "DataFormat/cluster.h"
#include "DataFormat/hit.h"
#include "DataFormat/pfpart.h"

#include "Analysis/ana_base.h"

namespace argovertex {
  /**
     \class VertexFromEndpoint
     User custom analysis class made by SHELL_USER_NAME
   */
  class VertexFromEndpoint : public larlite::ana_base{
  
  public:

    /// Default constructor
    VertexFromEndpoint(){ _name="VertexFromEndpoint"; _fout=0;}

    /// Default destructor
    virtual ~VertexFromEndpoint(){}

    /** IMPLEMENT in VertexFromEndpoint.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in VertexFromEndpoint.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(larlite::storage_manager* storage);

    /** IMPLEMENT in VertexFromEndpoint.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

  protected:

    double ind_plane_offset, coll_plane_offset;
    
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
