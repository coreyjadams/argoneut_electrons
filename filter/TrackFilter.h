/**
 * \file TrackFilter.h
 *
 * \ingroup filter
 * 
 * \brief Class def header for a class TrackFilter
 *
 * @author cadams
 */

/** \addtogroup filter

    @{*/

#ifndef LARLITE_TRACKFILTER_H
#define LARLITE_TRACKFILTER_H

#include "Analysis/ana_base.h"

namespace larlite {
  /**
     \class TrackFilter
     User custom analysis class made by SHELL_USER_NAME
   */
  class TrackFilter : public ana_base{
  
  public:

    /// Default constructor
    TrackFilter(){ _name="TrackFilter"; _fout=0;}

    /// Default destructor
    virtual ~TrackFilter(){}

    /** IMPLEMENT in TrackFilter.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in TrackFilter.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in TrackFilter.cc! 
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
