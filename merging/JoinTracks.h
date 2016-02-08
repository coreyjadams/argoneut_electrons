/**
 * \file JoinTracks.h
 *
 * \ingroup merging
 *
 * \brief Class def header for a class JoinTracks
 *
 * @author cadams
 */

/** \addtogroup merging

    @{*/

#ifndef LARLITE_JOINTRACKS_H
#define LARLITE_JOINTRACKS_H

#include "Analysis/ana_base.h"

namespace larlite {
/**
   \class JoinTracks
   User custom analysis class made by SHELL_USER_NAME
 */
class JoinTracks : public ana_base {

public:

  /// Default constructor
  JoinTracks() { _name = "JoinTracks"; _fout = 0;}

  /// Default destructor
  virtual ~JoinTracks() {}

  /** IMPLEMENT in JoinTracks.cc!
      Initialization method to be called before the analysis event loop.
  */
  virtual bool initialize();

  /** IMPLEMENT in JoinTracks.cc!
      Analyze a data event-by-event
  */
  virtual bool analyze(storage_manager* storage);

  /** IMPLEMENT in JoinTracks.cc!
      Finalize method to be called after all events processed.
  */
  virtual bool finalize();

  void SetInputProducer(std::string s) {_input_producer = s;}
  void SetOutputProducer(std::string s) {_output_producer = s;}

  size_t getClosestApproach(larlite::event_hit * ev_hit,
                            const std::vector<unsigned int> & hit_index_i,
                            const std::vector<unsigned int> & hit_index_j,
                            size_t & index_i,
                            size_t & index_j
                           );

protected:
  std::string _input_producer;
  std::string _output_producer;
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
