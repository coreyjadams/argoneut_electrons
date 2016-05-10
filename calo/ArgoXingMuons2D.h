/**
 * \file ArgoXingMuons2D.h
 *
 * \ingroup calo
 *
 * \brief Class def header for a class ArgoXingMuons2D
 *
 * @author cadams
 */

/** \addtogroup calo

    @{*/

#ifndef LARLITE_ARGOXINGMUONS2D_H
#define LARLITE_ARGOXINGMUONS2D_H

#include "Analysis/ana_base.h"

#include "DataFormat/track.h"
#include "DataFormat/calorimetry.h"
#include "DataFormat/spacepoint.h"
#include "DataFormat/hit.h"

#include "AnalysisAlg/CalorimetryAlg.h"

#include "ArgoCaloXingMuons.h"

#include "TH1F.h"
#include "TF1.h"

namespace larlite {
/**
   \class ArgoXingMuons2D
   User custom analysis class made by SHELL_USER_NAME
 */


class ArgoXingMuons2D : public ana_base {

public:

  /// Default constructor
  ArgoXingMuons2D() {
    _name = "ArgoXingMuons2D";
    _fout = 0;
  }

  /// Default destructor
  virtual ~ArgoXingMuons2D() {}

  /** IMPLEMENT in ArgoXingMuons2D.cc!
      Initialization method to be called before the analysis event loop.
  */
  virtual bool initialize();

  /** IMPLEMENT in ArgoXingMuons2D.cc!
      Analyze a data event-by-event
  */
  virtual bool analyze(storage_manager* storage);

  /** IMPLEMENT in ArgoXingMuons2D.cc!
      Finalize method to be called after all events processed.
  */
  virtual bool finalize();


  void setWireCorrections(std::vector<std::vector<double> > corr){_wire_corrections = corr;}
  std::vector<std::vector<double> > getWireCorrections(){return _wire_corrections;}

  wireCaloInfo getWireCaloInfo(int plane, int wire){return _wire_calo_info[plane][wire];}



protected:

  double getMode(const std::vector<double> &, float lowbin, float highbin, int n_bins);


  std::vector < std::vector< double > > _wire_corrections;
  ::calo::CalorimetryAlg _calo_alg;
  std::vector<std::vector< wireCaloInfo > > _wire_calo_info;


public:

  std::vector<double> _collection_muons_average_dedx;
  std::vector<double> _collection_muons_average_dqdx;
  std::vector<double> _induction_muons_average_dedx;
  std::vector<double> _induction_muons_average_dqdx;

  std::vector<double> _collection_muons_median_dedx;
  std::vector<double> _collection_muons_median_dqdx;
  std::vector<double> _induction_muons_median_dedx;
  std::vector<double> _induction_muons_median_dqdx;

  std::vector<double> _collection_muons_mode_dedx;
  std::vector<double> _collection_muons_mode_dqdx;
  std::vector<double> _induction_muons_mode_dedx;
  std::vector<double> _induction_muons_mode_dqdx;

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
