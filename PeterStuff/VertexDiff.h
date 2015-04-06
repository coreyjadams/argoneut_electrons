/**
 * \file VertexDiff.h
 *
 * \ingroup PeterStuff
 * 
 * \brief Class def header for a class VertexDiff
 *
 * @author pandacruzer
 */

/** \addtogroup PeterStuff

    @{*/

#ifndef LARLITE_VERTEXDIFF_H
#define LARLITE_VERTEXDIFF_H

#include "Analysis/ana_base.h"
#include "DataFormat/vertex.h"
#include "DataFormat/track.h"
#include "DataFormat/mctruth.h"
#include "TH2D.h"
#include "TH1D.h"
#include <iostream>
#include <fstream>
using namespace std;

namespace larlite {
  /**
     \class VertexDiff
     User custom analysis class made by SHELL_USER_NAME
   */
  class VertexDiff : public ana_base{
  
  public:

    /// Default constructor
    VertexDiff(){ _name="VertexDiff"; _fout=0;}

    /// Default destructor
    virtual ~VertexDiff(){}

    /** IMPLEMENT in VertexDiff.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in VertexDiff.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in VertexDiff.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

  protected:

    TH1D *fHvx = nullptr;
    TH1D *fHvy = nullptr;
    TH1D *fHvz = nullptr;
    TH1D *fHtx = nullptr;
    TH1D *fHty = nullptr;
    TH1D *fHtz = nullptr;
    TH1D *fHmcx = nullptr;
    TH1D *fHmcy = nullptr;
    TH1D *fHmcz = nullptr;
    TH1D *fHdiffX = nullptr;
    TH1D *fHdiffY = nullptr;
    TH1D *fHdiffZ = nullptr;
    TH1D *fHTrdiffX = nullptr;
    TH1D *fHTrdiffY = nullptr;
    TH1D *fHTrdiffZ = nullptr;
    TH1D *fHminX = nullptr;
    TH1D *fHminY = nullptr;
    TH1D *fHminZ = nullptr;
    TH1D *fHTrminX = nullptr;
    TH1D *fHTrminY = nullptr;
    TH1D *fHTrminZ = nullptr;
    TH1D *fHtot = nullptr;
    TH1D *fHTrtot = nullptr;
    TH1D *fHmintot = nullptr;
    TH1D *fHTrmintot = nullptr;

    double vertX;
    double vertY;
    double vertZ;
    double trackX;
    double trackY;
    double trackZ;    
    double mcX;
    double mcY;
    double mcZ;
    double minX;
    double minY;
    double minZ;
    double TrminX;
    double TrminY;
    double TrminZ;

    int event;
    ofstream output;
    ofstream out2;
    
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
