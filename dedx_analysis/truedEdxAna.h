/**
 * \file truedEdxAna.h
 *
 * \ingroup analysis
 * 
 * \brief Class def header for a class truedEdxAna
 *
 * @author cadams
 */

/** \addtogroup analysis

    @{*/

#ifndef LARLITE_TRUEDEDXANA_H
#define LARLITE_TRUEDEDXANA_H

#include "Analysis/ana_base.h"

namespace larlite {
  /**
     \class truedEdxAna
     User custom analysis class made by SHELL_USER_NAME
   */
  class truedEdxAna : public ana_base{
  
  public:

    /// Default constructor
    truedEdxAna(){ _name="truedEdxAna"; _fout=0;}

    /// Default destructor
    virtual ~truedEdxAna(){}

    /** IMPLEMENT in truedEdxAna.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in truedEdxAna.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in truedEdxAna.cc! 
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
