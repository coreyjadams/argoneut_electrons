/**
 * \file CBAlgoMergeWithinBoundary.h
 *
 * \ingroup CMTool
 *
 * \brief Class def header for a class CBAlgoMergeWithinBoundary
 *
 * @author pandacruzer
 */

/** \addtogroup CMTool

    @{*/
#ifndef RECOTOOL_CBALGOMERGEWITHINBOUNDARY_H
#define RECOTOOL_CBALGOMERGEWITHINBOUNDARY_H

#include <iostream>
#include "CMTool/CMToolBase/CBoolAlgoBase.h"

/**
   \class CBAlgoMergeWithinBoundary
   User defined class CBAlgoMergeWithinBoundary ... these comments are used to generate
   doxygen documentation!
 */

namespace argomerge {

class CBAlgoMergeWithinBoundary : public ::cmtool::CBoolAlgoBase {

public:

  /// Default constructor
  CBAlgoMergeWithinBoundary();

  /// Default destructor
  virtual ~CBAlgoMergeWithinBoundary() {}

  virtual bool Bool(const ::cluster::cluster_params &cluster1,
                    const ::cluster::cluster_params &cluster2);

  bool isBig(const ::cluster::cluster_params &cluster);
  bool isSmall(const ::cluster::cluster_params &cluster);
  //float* inBoundary(const ::cluster::cluster_params &cluster);
  float closestApproach(const ::cluster::cluster_params &cluster1,
                        const ::cluster::cluster_params &cluster2);

private:

  float _max_length;
  float _min_length;
  float _max_hits;
  float _min_hits;
  float _max_dist;
  float _max_ep;

};
}
#endif
/** @} */ // end of doxygen group

