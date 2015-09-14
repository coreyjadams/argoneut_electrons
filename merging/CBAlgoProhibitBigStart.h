/**
 * \file CBAlgoProhibitBigStart.h
 *
 * \ingroup CMTool
 *
 * \brief Class def header for a class CBAlgoProhibitBigStart
 *
 * @author Corey Adams
 */

/** \addtogroup CMTool

    @{*/
#ifndef RECOTOOL_CBALGOPROHIBITBIGSTART_H
#define RECOTOOL_CBALGOPROHIBITBIGSTART_H

#include <iostream>
#include "CMTool/CMToolBase/CBoolAlgoBase.h"
#include "LArUtil/GeometryUtilities.h"


namespace argomerge {
/**
   \class CMalgoPolyContain
   Merge Polygons if the two overlap even partially
*/
class CBAlgoProhibitBigStart : public ::cmtool::CBoolAlgoBase {

public:

  /// Default constructor
  CBAlgoProhibitBigStart();

  /// Default destructor
  virtual ~CBAlgoProhibitBigStart() {};

  /**
     Core function: given the cluster_params input, return whether a cluster should be
     merged or not.
  */
  virtual bool Bool(const ::cluster::cluster_params &cluster1,
                    const ::cluster::cluster_params &cluster2);

  void SetMinSeparation(float n) {_min_separation = n;}
  void SetMinHits(int n) {_min_hits = n;}

  float closestApproach(larutil::PxPoint start_point,
                        const ::cluster::cluster_params &cluster2);

  larutil::PxPoint alt_start(const std::vector<larutil::PxHit> & hitlist);

  void SetDebug(bool b) {debug = b;}

private:

  bool debug;
  float _min_separation;
  int _min_hits;
};
}

#endif
/** @} */ // end of doxygen group

