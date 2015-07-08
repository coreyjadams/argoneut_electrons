/**
 * \file CFAlgoMatchStart.h
 *
 * \ingroup CMTool
 * 
 * \brief Class def header for a class CFAlgoMatchStart
 *
 * @author ariana hackenburg
 */

/** \addtogroup CMTool

    @{*/
#ifndef RECOTOOL_CFALGOMATCHSTARTCOPY_H
#define RECOTOOL_CFALGOMATCHSTARTCOPY_H

#include "CMTool/CMToolBase/CFloatAlgoBase.h"
#include "utils/TrackShower.h"

namespace cmtool {
  /**
     \class CFAlgoMatchStartCOPY
     User implementation for CFloatAlgoBase class
     doxygen documentation!
  */
  class CFAlgoMatchStartCOPY : public CFloatAlgoBase {
    
  public:
    
    /// Default constructor
    CFAlgoMatchStartCOPY();
    
    /// Default destructor
    virtual ~CFAlgoMatchStartCOPY(){};

    /**This algorithm calculates the difference between start and end times for merged clusters,
		and compares across planes to form matches. 
    */
    virtual float Float(const std::vector<const cluster::ClusterParamsAlg*> &clusters);
    
    void SetStartTimeCut(float start_time) { _start_time_cut = start_time ; } 
    
    void SetRatioCut(float ratio) { _time_ratio_cut = ratio ; } 

 
    //Order the theta, phi, hits per plane to make cuts convenient
    /*
    virtual void SetMaxMiddleMin(const double first, const double second, const double third, 
				 double &most, double &middle, double &least) ;
    */
    void SetDebug(bool debug) { _debug = debug ; }
    
    void SetVerbose(bool verbose) { _verbose = verbose ; }

    void RequireThreePlanes(bool doit) { _require_3planes = doit; }

    std::vector<larutil::PxHit> GetClosestHits(const cluster::ClusterParamsAlg & params,
                                                larutil::PxPoint start, 
                                                int n_hits, bool forward_only);

    virtual void Report();
    
    virtual void Reset();

  protected:
    float _time_ratio_cut ;
    float _start_time_cut ;
    bool _debug ;
    bool _verbose ;
    bool _require_3planes;

    argo::TrackShower ts;

  };
}
#endif
/** @} */ // end of doxygen group 

