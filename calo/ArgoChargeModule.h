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
  ArgoChargeModule() {
    _name = "ArgoChargeModule";
    _area_q_correction.push_back(0.8152236862020928);
    _area_q_correction.push_back(0.8078780681936417);
    _area_e_correction.push_back(185.00584273133333);
    _area_e_correction.push_back(44.90267652367623);
  }

  /// Default destructor
  ~ArgoChargeModule() {}

  void do_reconstruction(const showerreco::ProtoShower &, showerreco::Shower_t &);

  void initialize();

private:


  ::calo::CalorimetryAlg _calo_alg;
  std::vector<double> fdQdx;
  int    _pl_best;// Now set as Y plane
  std::vector<double> _area_q_correction;
  std::vector<double> _area_e_correction;
  std::vector<double> _amp_q_correction;
  std::vector<double> _amp_e_correction;

};

/** @} */ // end of doxygen group

} // argocalo

#endif
