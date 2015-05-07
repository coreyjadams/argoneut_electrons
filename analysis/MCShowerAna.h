/**
 * \file MCShowerAna.h
 *
 * \ingroup analysis
 * 
 * \brief Class def header for a class MCShowerAna
 *
 * @author cadams
 */

/** \addtogroup analysis

    @{*/

#ifndef LARLITE_MCSHOWERANA_H
#define LARLITE_MCSHOWERANA_H

#include "Analysis/ana_base.h"

namespace larlite {
  /**
     \class MCShowerAna
     User custom analysis class made by SHELL_USER_NAME
   */
  class MCShowerAna : public ana_base{
  
  public:

    /// Default constructor
    MCShowerAna(){ _name="MCShowerAna"; _fout=0;}

    /// Default destructor
    virtual ~MCShowerAna(){}

    /** IMPLEMENT in MCShowerAna.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in MCShowerAna.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in MCShowerAna.cc! 
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
