/**
 * \file ArgoCaloXingMuons.h
 *
 * \ingroup calo
 *
 * \brief Class def header for a class ArgoCaloXingMuons
 *
 * @author cadams
 */

/** \addtogroup calo

    @{*/

#ifndef LARLITE_ARGOCALOXINGMUONS_H
#define LARLITE_ARGOCALOXINGMUONS_H

#include "Analysis/ana_base.h"

#include "DataFormat/track.h"
#include "DataFormat/calorimetry.h"
#include "DataFormat/spacepoint.h"
#include "DataFormat/hit.h"

#include "AnalysisAlg/CalorimetryAlg.h"

#include "TH1F.h"
#include "TF1.h"

namespace larlite {
/**
   \class ArgoCaloXingMuons
   User custom analysis class made by SHELL_USER_NAME
 */

// static const double area_q_corrections[2] = {1.829692e+02, 4.900967e+01};
// static const double amp_q_corrections[2] = {1.531968e+02, 4.008038e+01};

class wireCaloInfo {

public:

  unsigned int plane;
  unsigned int wire;

  unsigned int n_hits;

  std::vector<double> Q_area;
  std::vector<double> Q_amp;

  std::vector<double> Q_area_e;
  std::vector<double> Q_amp_e;

  std::vector<double> dQdx_area_e;
  std::vector<double> dQdx_amp_e;

  std::vector<double> dEdx_area;
  std::vector<double> dEdx_amp;

  TH1F * _dQdx_area;
  TH1F * _dQdx_amp;
  TH1F * _dEdx_area;
  TH1F * _dEdx_amp;

  double _correction_factor;

};


class ArgoCaloXingMuons : public ana_base {

public:

  /// Default constructor
  ArgoCaloXingMuons() {
    _name = "ArgoCaloXingMuons";
    _fout = 0;
    area_q_corrections.resize(2, 1.0);
    amp_q_corrections.resize(2, 1.0);
    area_e_corrections.resize(2, 1.0);
    amp_e_corrections.resize(2, 1.0);
  }

  /// Default destructor
  virtual ~ArgoCaloXingMuons() {}

  /** IMPLEMENT in ArgoCaloXingMuons.cc!
      Initialization method to be called before the analysis event loop.
  */
  virtual bool initialize();

  /** IMPLEMENT in ArgoCaloXingMuons.cc!
      Analyze a data event-by-event
  */
  virtual bool analyze(storage_manager* storage);

  /** IMPLEMENT in ArgoCaloXingMuons.cc!
      Finalize method to be called after all events processed.
  */
  virtual bool finalize();

  void setAreaQConstant(float calibration_const, int plane) {
    area_q_corrections[plane] = calibration_const;
  }
  void setAmpQConstant(float calibration_const, int plane) {
    amp_q_corrections[plane] = calibration_const;
  }

  void setAreaEConstant(float calibration_const, int plane) {
    area_e_corrections[plane] = calibration_const;
  }
  void setAmpEConstant(float calibration_const, int plane) {
    amp_e_corrections[plane] = calibration_const;
  }

  double induction_q_area_mean() {return _induction_q_area_mean;}
  double induction_q_amp_mean() {return _induction_q_amp_mean;}
  double collection_q_area_mean() {return _collection_q_area_mean;}
  double collection_q_amp_mean() {return _collection_q_amp_mean;}
  double induction_q_target_mean() {return _induction_q_target_mean;}
  double collection_q_target_mean() {return _collection_q_target_mean;}

  double induction_e_area_mean() {return _induction_e_area_mean;}
  double induction_e_amp_mean() {return _induction_e_amp_mean;}
  double collection_e_area_mean() {return _collection_e_area_mean;}
  double collection_e_amp_mean() {return _collection_e_amp_mean;}
  double induction_e_target_mean() {return _induction_e_target_mean;}
  double collection_e_target_mean() {return _collection_e_target_mean;}

  void setWireCorrections(std::vector<std::vector<double> > corr){_wire_corrections = corr;}
  std::vector<std::vector<double> > getWireCorrections(){return _wire_corrections;}

  wireCaloInfo getWireCaloInfo(int plane, int wire){return _wire_calo_info[plane][wire];}

  void setLifetimes(std::map<int,int> _lts){_lifetimes = _lts;}
  std::map<int,int> getLifetimes(){return _lifetimes;}

protected:

  std::map<int,int> _lifetimes;

  std::vector<double> _collection_dqdx_by_wire;
  std::vector<double> _collection_dedx_by_wire;
  std::vector<double> _induction_dqdx_by_wire;
  std::vector<double> _induction_dedx_by_wire;
  std::vector<double> _collection_dqdx_by_wire_count;
  std::vector<double> _induction_dqdx_by_wire_count;

  std::vector<std::vector< wireCaloInfo > > _wire_calo_info;
  std::vector < std::vector< double > > _wire_corrections;

  // Should argoneut worry about a wire by wire calibration?
  TH1F * _collection_wire_dqdx;
  TH1F * _induction_wire_dqdx;
  TH1F * _collection_wire_dedx;
  TH1F * _induction_wire_dedx;

  TH1F * _collection_wire_dqdx_corrected;
  TH1F * _induction_wire_dqdx_corrected;

  // Need some histograms:
  TH1F * _collection_calo_dqdx;
  TH1F * _induction_calo_dqdx;
  TH1F * _collection_calo_dEdx;
  TH1F * _induction_calo_dEdx;

  TH1F * _collection_caloAlg_dqdx_area;
  TH1F * _induction_caloAlg_dqdx_area;
  TH1F * _collection_caloAlg_dEdx_area;
  TH1F * _induction_caloAlg_dEdx_area;
  TH1F * _collection_caloAlg_dqdx_amp;
  TH1F * _induction_caloAlg_dqdx_amp;
  TH1F * _collection_caloAlg_dEdx_amp;
  TH1F * _induction_caloAlg_dEdx_amp;

  ::calo::CalorimetryAlg _calo_alg;

  std::vector<double> area_q_corrections;
  std::vector<double> amp_q_corrections;
  std::vector<double> area_e_corrections;
  std::vector<double> amp_e_corrections;

  double _induction_q_area_mean;
  double _induction_q_amp_mean;

  double _induction_e_area_mean;
  double _induction_e_amp_mean;

  double _collection_q_area_mean;
  double _collection_q_amp_mean;

  double _collection_e_area_mean;
  double _collection_e_amp_mean;

  double _induction_q_target_mean;
  double _collection_q_target_mean;

  double _induction_e_target_mean;
  double _collection_e_target_mean;
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
