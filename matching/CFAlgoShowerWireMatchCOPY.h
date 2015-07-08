/**
 * \file CFAlgoShowerWireMatch.h
 *
 * \ingroup CMTool
 * 
 * \brief Class def header for a class CFAlgoShowerWireMatch
 *
 * @author ariana hackenburg
 */

/** \addtogroup CMTool

    @{*/
#ifndef RECOTOOL_CFALGOSHOWERWIREMATCHCOPY_H
#define RECOTOOL_CFALGOSHOWERWIREMATCHCOPY_H

#include "CMTool/CMToolBase/CFloatAlgoBase.h"

#include "utils/TrackShower.h"

namespace cmtool {
  /**
     \class CFAlgoShowerWireMatch
     User implementation for CFloatAlgoBase class
     doxygen documentation!
  */
  class CFAlgoShowerWireMatchCOPY : public CFloatAlgoBase {
    
  public:
    
    /// Default 
    CFAlgoShowerWireMatchCOPY();
    
    /// Default destructor
    virtual ~CFAlgoShowerWireMatchCOPY(){};

    /**This algorithm calculates the difference between start and end times for merged clusters,
		and compares across planes to form matches. 
    */
    virtual float Float(const std::vector<const cluster::ClusterParamsAlg*> &clusters);
    
    void SetDebug(bool debug) { _debug = debug ; }
    
    void SetVerbose(bool verbose) { _verbose = verbose ; }

    void RequireThreePlanes(bool doit) { _require_3planes = doit; }

    virtual void Report();
    
    virtual void Reset();

    //Calculate the world coords in yz given the wire number in each plane, angle (angle indicates plane; 30 deg from z = U, 150 = V )
    void WireIDtoWorld(int wireID, double time, double angle, std::vector<double> & yz) ;

    float getScore( std::vector<float> showerRange, std::vector<float> otherRange);

  protected:

    bool _debug ;
    bool _verbose ;
    bool _require_3planes;
    double _w2cm, _t2cm ;

    argo::TrackShower ts;

  };
}
#endif
/** @} */ // end of doxygen group 

