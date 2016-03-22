/**
 * \file dEdxShowerAna.h
 *
 * \ingroup analysis
 *
 * \brief Class def header for a class dEdxShowerAna
 *
 * @author cadams
 */

/** \addtogroup analysis

    @{*/

#ifndef LARLITE_DEDXSHOWERANA_H
#define LARLITE_DEDXSHOWERANA_H

#include "Analysis/ana_base.h"
#include "DataFormat/shower.h"
#include "DataFormat/cluster.h"
#include "DataFormat/hit.h"
#include "LArUtil/Geometry.h"
#include "TVector3.h"

#include "TTree.h"

#include "utils/BGCFileReader.h"

namespace larlite {
/**
   \class dEdxShowerAna
   User custom analysis class made by SHELL_USER_NAME
 */
class dEdxShowerAna : public ana_base {

public:

  /// Default constructor
  dEdxShowerAna() { _name = "dEdxShowerAna"; _fout = 0;}

  /// Default destructor
  virtual ~dEdxShowerAna() {}

  /** IMPLEMENT in dEdxShowerAna.cc!
      Initialization method to be called before the analysis event loop.
  */
  virtual bool initialize();

  /** IMPLEMENT in dEdxShowerAna.cc!
      Analyze a data event-by-event
  */
  virtual bool analyze(storage_manager* storage);

  /** IMPLEMENT in dEdxShowerAna.cc!
      Finalize method to be called after all events processed.
  */
  virtual bool finalize();

  void getClosestHits(larlite::event_hit * ev_hit, std::vector<unsigned int> hits_to_consider,
                      std::vector<size_t> & close_hit_indexes,
                      std::vector<double> starthit);

  double getPitch(const TVector3 & dir3D, int plane );

  std::vector<std::pair<int, int> > makeMatches(const std::vector<argoutils::BGCData> &);

  std::vector<std::pair<int, int> > matchBCG_to_LL(const std::vector<std::pair<int, int> > & bgcmatches,
      const std::vector<argoutils::BGCData> &,
      larlite::event_hit *,
      const std::vector<std::vector<unsigned int> > & hit_ass );

protected:

  void unwindVectors(larlite::event_hit * ev_hit, std::vector<size_t> & close_hit_indexes);

  std::map< int, std::map< int,  std::vector<argoutils::BGCData> > > bgcdata;

  const larutil::Geometry * geom;



  TTree * tree;

  //
  // Variables to save into the ttree:
  //
  int run;
  int event;
  std::vector<double> fDCosStart; // the 3D axis

  std::vector<double> collection_hittimes;
  std::vector<double> collection_hitwires;
  std::vector<double> collection_hitcharges;
  std::vector<double> collection_hitpeaks;
  std::vector<double> collection_starthit;
  std::vector<double> collection_lifetime_corr;
  std::vector<double> collection_dist_from_start;
  double collection_slope;
  double collection_pitch;
  double collection_pitch_err;

  std::vector<double> induction_hittimes;
  std::vector<double> induction_hitwires;
  std::vector<double> induction_hitcharges;
  std::vector<double> induction_hitpeaks;
  std::vector<double> induction_starthit;
  std::vector<double> induction_lifetime_corr;
  std::vector<double> induction_dist_from_start;
  double induction_slope;
  double induction_pitch;
  double induction_pitch_err;


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
