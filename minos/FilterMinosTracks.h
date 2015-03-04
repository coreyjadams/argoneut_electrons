/**
 * \file FilterMinosTracks.h
 *
 * \ingroup minos
 * 
 * \brief Class def header for a class FilterMinosTracks
 *
 * @author ahack379
 */

/** \addtogroup minos

    @{*/

#ifndef LARLITE_FILTERMINOSTRACKS_H
#define LARLITE_FILTERMINOSTRACKS_H

#include "Analysis/ana_base.h"

namespace larlite {
  /**
     \class FilterMinosTracks
     User custom analysis class made by SHELL_USER_NAME
   */
  class FilterMinosTracks : public ana_base{
  
  public:

    /// Default constructor
    FilterMinosTracks(){ _name="FilterMinosTracks"; _fout=0;}

    /// Default destructor
    virtual ~FilterMinosTracks(){}

    /** IMPLEMENT in FilterMinosTracks.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in FilterMinosTracks.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in FilterMinosTracks.cc! 
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
