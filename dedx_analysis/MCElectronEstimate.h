/**
 * \file MCElectronEstimate.h
 *
 * \ingroup analysis
 * 
 * \brief Class def header for a class MCElectronEstimate
 *
 * @author cadams
 */

/** \addtogroup analysis

    @{*/

#ifndef LARLITE_MCELECTRONESTIMATE_H
#define LARLITE_MCELECTRONESTIMATE_H

#include "Analysis/ana_base.h"

#include "TLorentzVector.h"
#include "TH1F.h"
#include "TH2F.h"

namespace larlite {
  /**
     \class MCElectronEstimate
     User custom analysis class made by SHELL_USER_NAME
   */
  class MCElectronEstimate : public ana_base{
  
  public:

    /// Default constructor
    MCElectronEstimate(){ _name="MCElectronEstimate"; _fout=0;}

    /// Default destructor
    virtual ~MCElectronEstimate(){}

    /** IMPLEMENT in MCElectronEstimate.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in MCElectronEstimate.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    virtual bool begin_subrun(storage_manager* storage);


    /** IMPLEMENT in MCElectronEstimate.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

    std::vector<double> _deposited_energy;
    std::vector<double> _true_energy;

    std::vector<double> _theta;
    std::vector<double> _phi;
    std::vector<double> _pdg;



  protected:

    float pot;

    // some useful numbers to calculate:
    int _n_electrons;
    int _n_positrons;

    int subrun;

    // private functions:
    bool isActive(const TLorentzVector & vertex,double cut=4);
    
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
