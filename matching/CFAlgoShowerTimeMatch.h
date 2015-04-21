/**
 * \file CFAlgoShowerTimeMatch.h
 *
 * \ingroup CMTool
 * 
 * \brief Class def header for a class CFAlgoShowerTimeMatch
 *
 * @author ariana hackenburg
 */

/** \addtogroup CMTool

    @{*/
#ifndef RECOTOOL_CFALGOSHOWERTIMEMATCH_H
#define RECOTOOL_CFALGOSHOWERTIMEMATCH_H

#include "CMTool/CMToolBase/CFloatAlgoBase.h"

#include "utils/TrackShower.h"

namespace cmtool {
  /**
     \class CFAlgoShowerTimeMatch
     User implementation for CFloatAlgoBase class
     doxygen documentation!
  */
  class CFAlgoShowerTimeMatch : public CFloatAlgoBase {
    
  public:
    
    /// Default constructor
    CFAlgoShowerTimeMatch();
    
    /// Default destructor
    virtual ~CFAlgoShowerTimeMatch(){};

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

    virtual void Report();
    
    virtual void Reset();

    std::vector<float> getTimeRange(const cluster::ClusterParamsAlg*);
    float getScore( std::vector<float> showerRange, 
                    std::vector<float> otherRange);
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

