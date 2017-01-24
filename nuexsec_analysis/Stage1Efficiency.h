/**
 * \file Stage1Efficiency.h
 *
 * \ingroup nuexsec_analysis
 *
 * \brief Class def header for a class Stage1Efficiency
 *
 * @author cadams
 */

/** \addtogroup nuexsec_analysis

    @{*/

#ifndef LARLITE_STAGE1EFFICIENCY_H
#define LARLITE_STAGE1EFFICIENCY_H

#include "Analysis/ana_base.h"

#include "TVector3.h"
#include "TLorentzVector.h"

namespace argoana {

/**
   \class Stage1Efficiency
   User custom analysis class made by SHELL_USER_NAME
 */
class Stage1Efficiency : public larlite::ana_base {

public:

  /// Default constructor
  Stage1Efficiency() { _name = "Stage1Efficiency"; _fout = 0; _verbose = false;}

  /// Default destructor
  virtual ~Stage1Efficiency() {}

  /** IMPLEMENT in Stage1Efficiency.cc!
      Initialization method to be called before the analysis event loop.
  */
  virtual bool initialize();

  /** IMPLEMENT in Stage1Efficiency.cc!
      Analyze a data event-by-event
  */
  virtual bool analyze(larlite::storage_manager* storage);

  /** IMPLEMENT in Stage1Efficiency.cc!
      Finalize method to be called after all events processed.
  */
  virtual bool finalize();

  bool isFiducial(const TLorentzVector & vertex, double cut);
  bool filterEvent();

  void clear();


  void setTrackProducer(std::string s) {_track_producer = s;}
  void setClusterProducer(std::string s) {_cluster_producer = s;}
  void setMinTrackLength(float f) {_min_track_length = f;}

  void setVerbose(bool b){_verbose = false;}

protected:

  std::string _track_producer;
  std::string _cluster_producer;

  TTree * _tree;

  // Vertex variables (truth):
  float _vertex_x_true;
  float _vertex_y_true;
  float _vertex_z_true;
  bool _is_fiducial;


  // Neutrino Variables:
  float _neutrino_energy;
  int _neutrino_pdg;



  // Number of reconstructed vertexes:
  int _n_vertexes;

  // Information about number of clusters:
  int _n_clusters;

  // Information about tracks:
  int _n_tracks;
  int _n_long_tracks;

  float _min_track_length;
  float _longest_track;

  // How many clustered hits in the collection plane
  // are associated to the longest track?
  float _percent_hits_of_longest_track;


  // Internal variables:
  bool _verbose;

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
