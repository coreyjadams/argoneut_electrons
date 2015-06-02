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

#include "TH1F.h"
#include "TVector3.h"

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

    bool isFiducial(const TVector3 & vertex);


  protected:
    
    // Histograms to compare vertex information, dE/dx, direction:
    TH1F * vertex_X;
    TH1F * vertex_Y;
    TH1F * vertex_Z;
    TH1F * vertex_abs;

    TH1F * direction_X;
    TH1F * direction_Y;
    TH1F * direction_Z;
    TH1F * direction_abs;

    TH1F * dEdx;
    TH1F * dEdx_fid;

    std::vector<int> _good_event_list;
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
