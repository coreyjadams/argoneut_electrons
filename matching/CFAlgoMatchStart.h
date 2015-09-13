/**
 * \file CFAlgoMatchStart.h
 *
 * \ingroup CMTool
 *
 * \brief Class def header for a class CFAlgoMatchStart
 *
 * @author ariana hackenburg
 */

/** \addtogroup CMTool

    @{*/
#ifndef RECOTOOL_CFALGOMATCHSTART_H
#define RECOTOOL_CFALGOMATCHSTART_H

#include "CMTool/CMToolBase/CFloatAlgoBase.h"
#include "utils/TrackShower.h"

namespace cmtool {
/**
   \class CFAlgoMatchStart
   User implementation for CFloatAlgoBase class
   doxygen documentation!
*/
class CFAlgoMatchStart : public CFloatAlgoBase {

public:

  /// Default constructor
  CFAlgoMatchStart();

  /// Default destructor
  virtual ~CFAlgoMatchStart() {};

  /**This algorithm calculates the difference between start and end times for merged clusters,
  and compares across planes to form matches.
  */
  virtual float Float(const std::vector<const cluster::cluster_params*> &clusters);

  void SetStartTimeCut(float start_time) { _start_time_cut = start_time ; }

  void SetRatioCut(float ratio) { _time_ratio_cut = ratio ; }


  //Order the theta, phi, hits per plane to make cuts convenient
  /*
  virtual void SetMaxMiddleMin(const double first, const double second, const double third,
       double &most, double &middle, double &least) ;
  */
  void SetDebug(bool debug) { _debug = debug ; }

  void SetVerbose(bool verbose) { _verbose = verbose ; }

  void RequireThreePlanes(bool doit) { _require_3planes = doit; }

  std::vector<Hit2D> GetClosestHits(const cluster::cluster_params & params,
                                    larutil::PxPoint start,
                                    int n_hits, bool forward_only);

  virtual void Report();

  virtual void Reset();

protected:
  float _time_ratio_cut ;
  float _start_time_cut ;
  bool _debug ;
  bool _verbose ;
  bool _require_3planes;

  argoutils::TrackShower ts;

};
}
#endif
/** @} */ // end of doxygen group

