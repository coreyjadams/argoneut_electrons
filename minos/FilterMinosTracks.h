/**
 * \file FilterMinosTracks.h
 *
 * \ingroup minos
 * 
 * \brief Class def header for a class FilterMinosTracks
 *
 * @author ariana Hackenburg 
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

    virtual bool initialize();

    virtual bool analyze(storage_manager* storage);

    virtual bool finalize();

	void SetBoundary(double boundary){ fdBoundary = boundary ; }

	void PrepareTTree() ;

	void Reset() ;

  protected:

	double fdBoundary ;
	TTree * _ana_tree ;

	int _outGoingLepton;
	int _pdgRejected;
 	int _pdgKept ;
	double _rejX ;
	double _rejY ;
	double _rejZ ;
	double _keptX ;
	double _keptY ;
	double _keptZ ;
    
    
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
