/**
 * \file SinglesEndpointCheater.h
 *
 * \ingroup utils
 * 
 * \brief Class def header for a class SinglesEndpointCheater
 *
 * @author cadams
 */

/** \addtogroup utils

    @{*/

#ifndef ARGOUTIL_SINGLESENDPOINTCHEATER_H
#define ARGOUTIL_SINGLESENDPOINTCHEATER_H

#include "Analysis/ana_base.h"

namespace argoutils {
  /**
     \class SinglesEndpointCheater
     User custom analysis class made by SHELL_USER_NAME
   */
  class SinglesEndpointCheater : public larlite::ana_base{
  
  public:

    /// Default constructor
    SinglesEndpointCheater(){ _name="SinglesEndpointCheater"; _fout=0;}

    /// Default destructor
    virtual ~SinglesEndpointCheater(){}

    /** IMPLEMENT in SinglesEndpointCheater.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in SinglesEndpointCheater.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(larlite::storage_manager* storage);

    /** IMPLEMENT in SinglesEndpointCheater.cc! 
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
