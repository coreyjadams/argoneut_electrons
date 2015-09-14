/**
 * \file CFAlgoREPLACENAME.h
 *
 * \ingroup CMTool
 * 
 * \brief Class def header for a class CFAlgoREPLACENAME
 *
 * @author ariana hackenburg
 */

/** \addtogroup CMTool

    @{*/
#ifndef RECOTOOL_CFALGOREPLACENAME_H
#define RECOTOOL_CFALGOREPLACENAME_H

#include "CMTool/CMToolBase/CFloatAlgoBase.h"

namespace argomatch {
  /**
     \class CFAlgoREPLACENAME
     User implementation for CFloatAlgoBase class
     doxygen documentation!
  */
  class CFAlgoREPLACENAME : public ::cmtool::CFloatAlgoBase {
    
  public:
    
    /// Default constructor
    CFAlgoREPLACENAME();
    
    /// Default destructor
    virtual ~CFAlgoREPLACENAME(){};

    /**This algorithm calculates the difference between start and end times for merged clusters,
		and compares across planes to form matches. 
    */
    virtual float Float(const std::vector<const cluster::cluster_params*> &clusters);
    
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

  protected:
    float _time_ratio_cut ;
    float _start_time_cut ;
    bool _debug ;
    bool _verbose ;
    bool _require_3planes;
  };
}
#endif
/** @} */ // end of doxygen group 

