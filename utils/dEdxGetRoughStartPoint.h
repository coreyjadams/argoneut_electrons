/**
 * \file dEdxGetRoughStartPoint.h
 *
 * \ingroup ClusterRecoUtil
 * 
 * \brief Class def header for a class dEdxGetRoughStartPoint
 *
 * @author david caratelli
 * @author corey adams
 * 
 */

/** \addtogroup ClusterRecoUtil

    @{*/
#ifndef DEDXGETROUGHSTARTPOINT_H
#define DEDXGETROUGHSTARTPOINT_H

#include <iostream>
#include "ClusterRecoUtil/Alg/ParamsAlgBase.h"

namespace argoutils {

  /**
     \class dEdxGetRoughStartPoint
     User defined class dEdxGetRoughStartPoint ... these comments are used to generate
     doxygen documentation!
   */
  class dEdxGetRoughStartPoint : public ::cluster::ParamsAlgBase {

  public:

    /// Default constructor
    dEdxGetRoughStartPoint();

    /// Default destructor
    ~dEdxGetRoughStartPoint(){}

    void do_params_fill(::cluster::cluster_params &);

    /**
     * @brief Set Number of Hits to consider for angle calculation
     * @details The algorithm computes the angle between the segment
     * connecting each pair of hits in this list and the slope_2d
     * parameter. This mean N! angle computations.
     * The N hits are the hits with the mosst charge
     */
    void SetNHits(size_t n) { _N = n; }

  private:
    
    /**
       @brief Get angle between two hits and the 2d slope.
       @details Angle is calculated as the angle (radians)
       * between the line connecting h1 and h2 (pointing to h2)
       * and the slope. This calculation is in (cm,cm) space
       * @param h1 is a Hit2D
       * @param h2 is a Hit2D
       * @param slope is the slope_2d of the cluster previously calculated
       * @return angle between (h1->h2) and slope
     */
    double GetAngle(const larutil::Point2D& h1, const larutil::Point2D& h2, const double& slope);

    /// Number of hits to be used for angle calculation
    size_t _N;

    /// Matrix that will hold the angle computed for each pair
    /// of hits. Vector size is _N X _N
    std::vector<std::vector<float> > _angleMatrix;

  };

} // cluster


#endif
/** @} */ // end of doxygen group 

