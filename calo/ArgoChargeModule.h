/**
 * \file ArgoChargeModule.h
 *
 * \ingroup calo
 * 
 * \brief Class def header for a class ArgoChargeModule
 *
 * @author cadams
 */

/** \addtogroup calo

    @{*/
#ifndef ARGOCHARGEMODULE_H
#define ARGOCHARGEMODULE_H

#include <iostream>
#include "ShowerReco3D/ModularAlgo/ShowerRecoModuleBase.h"

#include "AnalysisAlg/CalorimetryAlg.h"

/**
   \class ArgoChargeModule
   User defined class ArgoChargeModule ... these comments are used to generate
   doxygen documentation!
 */
namespace argocalo {


class ArgoChargeModule : public showerreco::ShowerRecoModuleBase {

public:

  /// Default constructor
  ArgoChargeModule(){}

  /// Default destructor
  ~ArgoChargeModule(){}

  void do_reconstruction(const ProtoShower &, Shower_t &);

  void initialize();

private:


  ::calo::CalorimetryAlg _calo_alg;

};

/** @} */ // end of doxygen group 

} // argocalo

#endif
