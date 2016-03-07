/**
 * \file ArgoneutParamsAlg.h
 *
 * \ingroup ClusterRecoUtil
 * 
 * \brief Class def header for a class ArgoneutParamsAlg
 *
 * @author cadams
 */

/** \addtogroup ClusterRecoUtil

    @{*/
#ifndef ARGONEUTPARAMSALG_H
#define ARGONEUTPARAMSALG_H

#include <iostream>
#include "ClusterRecoUtil/Base/ClusterParamsAlg.h"
#include "ClusterRecoUtil/Alg/GetAverages.h"
#include "ClusterRecoUtil/Alg/FillPolygon.h"
#include "ClusterRecoUtil/Alg/GetRoughStartPoint.h"
#include "ClusterRecoUtil/Alg/RefineStartPoints.h"
#include "ClusterRecoUtil/Alg/SelectStartPoint.h"
#include "ClusterRecoUtil/Alg/FindShoweringPoint.h"
#include "ClusterRecoUtil/Alg/FillGeomParams.h"

#include "dEdxGetRoughStartPoint.h"
#include "dEdxSelectStartPoint.h"

namespace argoutils {


  /**
     \class ArgoneutParamsAlg
     User defined class ArgoneutParamsAlg ... these comments are used to generate
     doxygen documentation!
   */
  class ArgoneutParamsAlg : public ::cluster::ClusterParamsAlg {

  public:

    /// Default constructor
    ArgoneutParamsAlg();

    /// Default destructor
    ~ArgoneutParamsAlg(){}

  };


} // cluster


#endif
/** @} */ // end of doxygen group 

