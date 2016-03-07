/**
 * \file dEdxSelectStartPoint.h
 *
 * \ingroup ClusterRecoUtil
 * 
 * \brief Class def header for a class dEdxSelectStartPoint
 *
 * @author corey adams
 */

/** \addtogroup ClusterRecoUtil

    @{*/
#ifndef DEDXSELECTSTARTPOINT_H
#define DEDXSELECTSTARTPOINT_H

#include <iostream>
#include "ClusterRecoUtil/Alg/ParamsAlgBase.h"

namespace argoutils {

  /**
     \class dEdxSelectStartPoint
     User defined class dEdxSelectStartPoint ... these comments are used to generate
     doxygen documentation!
   */
  class dEdxSelectStartPoint : public ::cluster::ParamsAlgBase {

  public:

    /// Default constructor
    dEdxSelectStartPoint(){_name = "dEdxSelectStartPoint"; }

    /// Default destructor
    ~dEdxSelectStartPoint(){}

    void do_params_fill(::cluster::cluster_params &);

  private:

    
  };

} // cluster


#endif
/** @} */ // end of doxygen group 

