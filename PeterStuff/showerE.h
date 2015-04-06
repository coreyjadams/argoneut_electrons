/**
 * \file showerE.h
 *
 * \ingroup PeterStuff
 * 
 * \brief Class def header for a class showerE
 *
 * @author pandacruzer
 */

/** \addtogroup PeterStuff

    @{*/

#ifndef LARLITE_SHOWERE_H
#define LARLITE_SHOWERE_H

#include "Analysis/ana_base.h"
#include "DataFormat/vertex.h"
#include "DataFormat/mcshower.h"
#include "Analysis/ana_base.h"
#include "DataFormat/track.h"
#include "DataFormat/mctruth.h"
#include "DataFormat/minos.h"
#include "TH2D.h"
#include "TH1D.h"
#include <iostream>
#include <fstream>

namespace larlite {
  /**
     \class showerE
     User custom analysis class made by SHELL_USER_NAME
   */
  class showerE : public ana_base{
  
  public:

    /// Default constructor
    showerE(){ _name="showerE"; _fout=0;}

    /// Default destructor
    virtual ~showerE(){}

    /** IMPLEMENT in showerE.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in showerE.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in showerE.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

  protected:

    TH1D *fHeCount = nullptr;
    TH1D *fHmcE = nullptr;
    TH1D *fH3dE = nullptr;

    int eCount;
    
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
