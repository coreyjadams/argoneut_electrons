/**
 * \file CBAlgoProhibitAllTracks.h
 *
 * \ingroup CMTool
 *
 * \brief Class def header for a class CBAlgoProhibitAllTracks
 *
 * @author davidkaleko
 */

/** \addtogroup CMTool

    @{*/
#ifndef RECOTOOL_CBALGOPROHIBITTRACK_H
#define RECOTOOL_CBALGOPROHIBITTRACK_H

#include <iostream>
#include "CMTool/CMToolBase/CBoolAlgoBase.h"

namespace argomerge {
/**
   \class CBAlgoProhibitAllTracks
   User implementation for CBoolAlgoBase class
   doxygen documentation!
*/
class CBAlgoProhibitTrack : public ::cmtool::CBoolAlgoBase {

public:

  /// Default constructor
  CBAlgoProhibitTrack();

  /// Default destructor
  virtual ~CBAlgoProhibitTrack() {};

  /**
     Optional function: called at the beginning of 1st iteration. This is called per event.
   */
  //virtual void EventBegin(const std::vector<cluster::cluster_params> &clusters);

  /**
     Optional function: called at the end of event ... after the last merging iteration is over.
   */
  //virtual void EventEnd();

  /**
     Optional function: called at the beggining of each iteration over all pairs of clusters.
     This provides all clusters' information in case the algorithm need them. Note this
     is called per iteration which may be more than once per event.
   */
  //virtual void IterationBegin(const std::vector<cluster::cluster_params> &clusters);

  /**
     Optional function: called at the end of each iteration over all pairs of clusters.
   */
  //virtual void IterationEnd();

  /**
     Core function: given the CPAN input, return whether a cluster should be
     merged or not.
  */
  virtual bool Bool(const ::cluster::cluster_params &cluster1,
                    const ::cluster::cluster_params &cluster2);

  /**
     Optional function: called after each Merge() function call by CMergeManager IFF
     CMergeManager is run with verbosity level kPerMerging. Maybe useful for debugging.
  */
  virtual void Report();

  /// Function to reset the algorithm instance ... maybe implemented via child class
  virtual void Reset();


  void SetMinEP(double value) { _min_EP = value; }

protected:

  double _min_EP;

};


}
#endif
/** @} */ // end of doxygen group

