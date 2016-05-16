/**
 * \file CBAlgoMergeBrokenTrack.h
 *
 * \ingroup CMTool
 *
 * \brief Class def header for a class CBAlgoMergeBrokenTrack
 *
 * @author pandacruzer
 */

/** \addtogroup CMTool

    @{*/
#ifndef RECOTOOL_CBALGOMERGEBROKENTRACK_H
#define RECOTOOL_CBALGOMERGEBROKENTRACK_H

#include <iostream>
#include "CMTool/CMToolBase/CBoolAlgoBase.h"
#include "LArUtil/Geometry.h"
 
/**
   \class CBAlgoMergeBrokenTrack
   User defined class CBAlgoMergeBrokenTrack ... these comments are used to generate
   doxygen documentation!
 */

namespace argomerge {

class CBAlgoMergeBrokenTrack : public ::cmtool::CBoolAlgoBase {

public:

  /// Default constructor
  CBAlgoMergeBrokenTrack();

  /// Default destructor
  virtual ~CBAlgoMergeBrokenTrack() {}

  virtual bool Bool(const ::cluster::cluster_params &cluster1,
                    const ::cluster::cluster_params &cluster2);


  float closestApproach(const ::cluster::cluster_params &cluster1,
                        const ::cluster::cluster_params &cluster2);

  float getWireGap(cluster::cluster_params clust1,
                   cluster::cluster_params clust2,
                   size_t & index_i,
                   size_t & index_j);

  void setMinCosThetaCut(float f) {_min_cos_angle_diff = f;}

  void setDebug(bool b) {_debug = b;}
  void setMinHits(int b) {_min_hits = b;}

  void setMaxWireGap(float g) {_max_wire_gap = g;}
  void setMaxTimeGap(float g) {_max_time_gap = g;}

private:

  float _max_wire_gap;
  float _max_time_gap;
  float _max_dist_gap;
  float _min_cos_angle_diff;

  int _deadWireStart;
  int _deadWireEnd;

  int _min_hits;

  bool _debug;

};
}
#endif
/** @} */ // end of doxygen group

