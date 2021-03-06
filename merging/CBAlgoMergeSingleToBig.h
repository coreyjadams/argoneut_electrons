/**
 * \file CBAlgoMergeSingleToBig.h
 *
 * \ingroup CMTool
 *
 * \brief Class def header for a class CBAlgoMergeSingleToBig
 * This algorithm will merge small hits into nearby large clusters
 * You can set the minimum distance, which will be the closest approach
 * as well as the average distance between all hits.
 *
 * @author Corey Adams
 */

/** \addtogroup CMTool

    @{*/
#ifndef RECOTOOL_CBALGOMERGESINGLETOBIG_H
#define RECOTOOL_CBALGOMERGESINGLETOBIG_H

#include <iostream>
#include "CMTool/CMToolBase/CBoolAlgoBase.h"


namespace argomerge {
/**
   \class CMalgoPolyContain
   Merge Polygons if the two overlap even partially
*/
class CBAlgoMergeSingleToBig : public ::cmtool::CBoolAlgoBase {

public:

  /// Default constructor
  CBAlgoMergeSingleToBig();

  /// Default destructor
  virtual ~CBAlgoMergeSingleToBig() {};

  /**
     Core function: given the cluster_params input, return whether a cluster should be
     merged or not.
  */
  virtual bool Bool(const ::cluster::cluster_params &cluster1,
                    const ::cluster::cluster_params &cluster2);


  bool isContainedBy(const ::cluster::cluster_params &cluster1,
                     const ::cluster::cluster_params &cluster2);

  void SetMaxDistance(float d) {_max_distance = d;}
  void SetMaxAverageDistance(float d) {_max_average_distance = d;}
  void SetMaxSmallClustHits(int d) {_max_small_clust_hits = d;}

private:


  bool compareClusters(const ::cluster::cluster_params & bigCluster,
                       const ::cluster::cluster_params & smallCluster);

  float _max_distance;
  float _max_average_distance;
  unsigned int _min_big_clust_hits;
  unsigned int _max_small_clust_hits;
};
}

#endif
/** @} */ // end of doxygen group

