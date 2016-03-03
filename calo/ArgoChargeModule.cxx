#ifndef ARGOCHARGEMODULE_CXX
#define ARGOCHARGEMODULE_CXX

#include "ArgoChargeModule.h"
#include "LArUtil/LArProperties.h"
#include "LArUtil/DetectorProperties.h"


namespace argocalo {

void ArgoChargeModule::initialize()
{
  // to go from mV to fC the ASIC gain and Shaping time
  // must be considered
  // fC -> mV *= ( shaping time * ASIC gain )
  _shp_time  = 2.;  // in usec
  _asic_gain = 7.8; // in mV/fC
  _fC_to_e = 6250.; // a fC in units of the electron charge
  _ADC_to_mV = 0.5; // ADC -> mV conversion from gain measurements
  // _charge_conversion = _ADC_to_mV * _fC_to_e / ( _shp_time * _asic_gain ) ;
  _charge_conversion = 1 ;
  // Correction factr
  _tau = larutil::LArProperties::GetME()->ElectronLifetime();             //electron lifetime in usec
  _timetick = larutil::DetectorProperties::GetME()->SamplingRate() * 1.e-3; //time sample in usec, 0.5us

  return;
}

void ArgoChargeModule::do_reconstruction(const ProtoShower & input_shower,
    Shower_t & result_shower) {

  // First, get the hits from each cluster 
  // that correspond to the start of the shower

  // Then, 

  return;

}

}

#endif
