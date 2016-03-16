#ifndef ARGOCHARGEMODULE_CXX
#define ARGOCHARGEMODULE_CXX

#include "ArgoChargeModule.h"
#include "LArUtil/LArProperties.h"
#include "LArUtil/DetectorProperties.h"
#include "LArUtil/GeometryHelper.h"

namespace argocalo {

void ArgoChargeModule::initialize()
{
  // to go from mV to fC the ASIC gain and Shaping time
  // must be considered
  // fC -> mV *= ( shaping time * ASIC gain )
  // _shp_time  = 2.;  // in usec
  // _asic_gain = 7.8; // in mV/fC
  // _fC_to_e = 6250.; // a fC in units of the electron charge
  // _ADC_to_mV = 0.5; // ADC -> mV conversion from gain measurements
  // // _charge_conversion = _ADC_to_mV * _fC_to_e / ( _shp_time * _asic_gain ) ;
  // _charge_conversion = 1 ;
  // // Correction factr
  // _tau = larutil::LArProperties::GetME()->ElectronLifetime();             //electron lifetime in usec
  // _timetick = larutil::DetectorProperties::GetME()->SamplingRate() * 1.e-3; //time sample in usec, 0.5us

  _calo_alg.setUseModBox(true);

  return;
}

void ArgoChargeModule::do_reconstruction(
  const showerreco::ProtoShower & input_shower,
  showerreco::Shower_t & result_shower) {

  auto & clusters = input_shower.params();

  /***
  dQdx = hit charge / pitch .
  For details, see Doc-db 4909 .
  dQdx of a shower is the median of dQdxs of hits along trunk .
  Trunk connects cluster and shower start points .
  Calculation starts line 84 .
  dQdx is calculated for 3 planes. All variables are initialized
  correspondingly as a 3-dimensional vector.
  Note within this Algorithm, the best plane is defined as the
  plane w/ longest trunk length. Therefore here best plane does
  NOT refer to best dQdx.
  User should define their own best plane OUTSIDE this algorithm.
  --------------------------------------------------------------
  In OCT 2015, developers tried sigma or rms to better calculate
  dQdx but results showed no improvements. Now this part is removed.
  ***/

  auto geom = larutil::Geometry::GetME();
  auto geomHelper = larutil::GeometryHelper::GetME();
  auto detProp = larutil::DetectorProperties::GetME();
  const size_t nplanes = geom->Nplanes();
  //auto t2cm = geomHelper->TimeToCm();  //time to cm conversion for e lifetime correction
  auto & dir3D = result_shower.fDCosStart;
  dir3D *= 1.0 / dir3D.Mag();

  std::vector<double> trunk_length(nplanes, 0);
  std::vector<int>    n_hits(nplanes, 0);
  std::vector<std::vector<double> > dQdxs(nplanes, std::vector<double>());
  std::vector<std::vector<double> > dEdxs(nplanes, std::vector<double>());
  std::vector<double> median(nplanes, 0);
  std::vector<double> median_dedx(nplanes, 0);

  //// loop over all input cluster -> calculate a dQdx per plane
  for (size_t n = 0; n < clusters.size(); n++) {

    // get the hits associated with this cluster
    auto const& hits = clusters.at(n).hit_vector;
    // get the plane associated with this cluster
    auto const& pl = clusters.at(n).plane_id.Plane;
    // get the start point for this cluster
    auto const& clu_start = clusters.at(n).start_point;

    // Get the mean point for this cluster:
    auto mean_x = clusters.at(n).mean_x;
    auto mean_y = clusters.at(n).mean_y;

    if (pl >= nplanes) throw showerreco::ShowerRecoException("Invalid plane found!");

    /***
    Find list of hits that are along the trunk of
    the shower. Grab all hits within 2.4cm w.r.t
    to cluster start points and hits within trunk
    length compared to cluster start point.
    ---------------------------------------------
    2.4cm is just another magic number in uBooNE.
    If you found a better cut, should just modify it.
    ---------------------------------------------
    For all these hits, save dQdx into dQdx if tan
    value at cluster start point is no larger than
    tan value of shower open angle.
    ***/
    double pitch = 0.4;


    double angleToVert = geom -> WireAngleToVertical((::larlite::geo::View_t) pl) - 0.5* TMath::Pi();
    double cosgamma = TMath::Abs(TMath::Sin(angleToVert) * dir3D.Y() +
                                 TMath::Cos(angleToVert) * dir3D.Z());

    if (cosgamma > 0) pitch = pitch / cosgamma;

    // Got to calculate the pitch:

    double dist_hit_clu;
    double dist_hit_shr;

    double * origin;
    origin = new double[geom->Nviews()];
    geom->PlaneOriginVtx(pl, origin);
    double planeOffset = origin[0];

    for (int i = 0; i < hits.size(); i++) {

      dist_hit_clu = sqrt((hits[i].w - clu_start.w) * (hits[i].w - clu_start.w) +
                          (hits[i].t - clu_start.t) * (hits[i].t - clu_start.t));
      /***
          Correction for electron life time,
          Now removed since T0 is unknown.
          ---------------------------------
          double hit_tick =hits[i].t/t2cm;
          double lifetimeCorr = exp( hit_tick * _timetick / _tau );
          double Q = hits[i].charge * _charge_conversion*lifetimeCorr;
      ***/
      // double Q = hits[i].charge * _charge_conversion;
      double Q = _area_q_correction[pl] * hits[i].charge;
      double tick = (hits[i].t - planeOffset) / geomHelper -> TimeToCm() + detProp -> TriggerOffset();
        // std::cout << "LifetimeCorrection is " << _calo_alg.LifetimeCorrection(tick) << std::endl;
      // h.t = (hit.PeakTime() - detProp->TriggerOffset()) * geoH->TimeToCm() + planeOffset;
      Q *= _calo_alg.LifetimeCorrection(tick);
      double Q_e = Q * _area_e_correction[pl];
      double dEdx  = larutil::LArProperties::GetME() -> ModBoxCorrection(Q_e / pitch);
      // double dEdx  = larutil::LArProperties::GetME() -> BirksCorrection(Q_e / pitch);
      if (dist_hit_clu <= 2.4 ) {
        // std::cout << "Using hit at (" << hits[i].w << ", " << hits[i].t
        //           << ") on plane " << pl
        //           << ", dQ/dx is " << Q_e / pitch
        //           << ", dE/dx is " << dEdx
        //           << ", charge is " << Q_e
        //           << std::endl;

        dQdxs[pl].push_back(Q_e / pitch);
        dEdxs[pl].push_back(dEdx);
        // // This catches the first hit
        // if (dist_hit_clu * dist_hit_shr == 0) {
        //   // std::cout << "Using hit at (" << hits[i].w << ", " << hits[i].t
        //   //           << ") on plane " << pl
        //   //           << ", dQ/dx is " << Q / pitch
        //   //           << ", dE/dx is " << dEdx
        //   //           << std::endl;
        //   dQdxs[pl].push_back(Q / pitch);
        //   dEdxs[pl].push_back(dEdx);
        // }

        // else if (dist_hit_clu * dist_hit_shr > 0) {

        //   // This hit is within 2.4 cm of the start hit
        //   // But should we use it?
        //   // Why not?
        //   if () {
        //     // std::cout << "Using hit at (" << hits[i].w << ", " << hits[i].t
        //     //           << ") on plane " << pl
        //     //           << ", dQ/dx is " << Q / pitch
        //     //           << ", dE/dx is " << dEdx
        //     //           << std::endl;
        //     dQdxs[pl].push_back(Q / pitch);
        //     dEdxs[pl].push_back(dEdx);
        //   }
        // }
      }
    }
    delete origin;
    // Get median of dQdx
    std::nth_element(dQdxs[pl].begin(), dQdxs[pl].begin() + dQdxs[pl].size() / 2, dQdxs[pl].end());
    if (dQdxs[pl].size() > 0)median[pl] = dQdxs[pl].at((dQdxs[pl].size() / 2));

    // Get median of dEdx
    std::nth_element(dEdxs[pl].begin(), dEdxs[pl].begin() + dEdxs[pl].size() / 2, dEdxs[pl].end());
    if (dEdxs[pl].size() > 0)median_dedx[pl] = dEdxs[pl].at((dEdxs[pl].size() / 2));

    /***
    Best plane here refers to the plane w/ longest
    trunk length. Note that Some trunk length are
    larger than 10cm due to mis-assigned cluster
    or shower start point. Throw them away.
    --------------------------------------------------
    From previous study and for users' convenience,
    set Y plane the best plane.
    ***/
    /*if(pl==2){
    std::vector<std::pair<int,double>> pl_best;
    pl_best.resize(3);
    pl_best.at(0).first=0;
    pl_best.at(0).second=trunk_length[0];
    pl_best.at(1).first=1;
    pl_best.at(1).second=trunk_length[1];
    pl_best.at(2).first=2;
    pl_best.at(2).second=trunk_length[2];

    std::sort(pl_best.begin(),pl_best.end(),larger);

    if(pl_best.at(2).second<10)_pl_best=pl_best.at(2).first;
    else if(pl_best.at(1).second<10)_pl_best=pl_best.at(1).first;
    else if(pl_best.at(0).second<10)_pl_best=pl_best.at(0).first;
    else break;
    resultShower.fBestdQdxPlane = _pl_best;
    }*/
    _pl_best = nplanes - 1;
    result_shower.fBestdQdxPlane = _pl_best;
    result_shower.fdQdx_v[pl] = median[pl];
    result_shower.fdEdx_v[pl] = median_dedx[pl];



  }// for all clusters (planes)

  // set the total dEdx to the collection plane value:
  result_shower.fdEdx = median_dedx.back();

  return;

}

}

#endif
