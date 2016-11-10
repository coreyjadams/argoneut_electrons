/**
 * \file dEdxShowerCaloMaker.h
 *
 * \ingroup analysis
 *
 * \brief Class def header for a class dEdxShowerCaloMaker
 *
 * @author cadams
 */

/** \addtogroup analysis

    @{*/

#ifndef LARLITE_DEDXSHOWERCALOMAKER_H
#define LARLITE_DEDXSHOWERCALOMAKER_H

#include "Analysis/ana_base.h"
#include "ShowerReco3D/ProtoShower/ProtoShowerHelper.h"
#include "ShowerReco3D/ProtoShower/ProtoShowerAlgArgoNeuT.h"
#include "ShowerCalo.h"

#include "LArUtil/GeometryHelper.h"
#include "LArUtil/Geometry.h"
#include "LArUtil/DetectorProperties.h"
#include "LArUtil/LArProperties.h"


#include "DataFormat/shower.h"
#include "DataFormat/pfpart.h"
#include "DataFormat/mcshower.h"

namespace argoana {
/**
   \class dEdxShowerCaloMaker
   User custom analysis class made by SHELL_USER_NAME
 */
class dEdxShowerCaloMaker : public larlite::ana_base {

public:

  /// Default constructor
  dEdxShowerCaloMaker() { _name = "dEdxShowerCaloMaker"; _fout = 0; _select_events = false;}

  /// Default destructor
  virtual ~dEdxShowerCaloMaker() {}

  /** IMPLEMENT in dEdxShowerCaloMaker.cc!
      Initialization method to be called before the analysis event loop.
  */
  virtual bool initialize();

  /** IMPLEMENT in dEdxShowerCaloMaker.cc!
      Analyze a data event-by-event
  */
  virtual bool analyze(larlite::storage_manager* storage);

  /** IMPLEMENT in dEdxShowerCaloMaker.cc!
      Finalize method to be called after all events processed.
  */
  virtual bool finalize();

  void setWireCorrections(std::vector<std::vector<double> > corr) {_calorimetry_corrections = corr;}
  std::vector<std::vector<double> > getWireCorrections() {return _calorimetry_corrections;}

  void setProducer(std::string s) {_producer_name = s;}

  void isMC(bool b) {_is_mc = b;}

  double getPitch(const TVector3 & dir3D, int pl );

  const shower_collection & getShowerCalos() {return _shower_calo_vector;}

  larutil::Point2D findClosestHit(std::vector<larutil::Hit2D> hit_vector,
                                  std::vector<unsigned int> hit_indexes,
                                  larutil::Point2D start_point);

  bool keepEvent(int run, int event_no, int & best_plane);

  std::vector<unsigned int> hand_select_hits(int run, int event, int plane);


  void set_select_events(bool b) {_select_events = b;}

  void setLifetimes(std::map<int, int> _lts) {_lifetimes = _lts;}
  std::map<int, int> getLifetimes() {return _lifetimes;}

  bool isFiducial(const TVector3 & _point);
  float BDTW(const TVector3 & start_point, const TVector3 & start_dir);

  bool getTruedEdxVector(larlite::storage_manager * storage, std::vector<std::pair<double,double> > &);


protected:

  std::map<int, int> _lifetimes;

  bool _select_events;

  bool _is_mc;

  std::vector<std::vector<double> > _calorimetry_corrections;

  std::string _producer_name;

  shower_collection _shower_calo_vector;

  protoshower::ProtoShowerHelper ps_helper;
  protoshower::ProtoShowerAlgArgoNeuT *  ps_alg;

  float threshold = 5.0;
  float _total_depE;

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
