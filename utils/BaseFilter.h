/**
 * \file BaseFilter.h
 *
 * \ingroup utils
 * 
 * \brief Class def header for a class BaseFilter
 *
 * @author cadams
 */

/** \addtogroup utils

    @{*/

#ifndef LARLITE_BASEFILTER_H
#define LARLITE_BASEFILTER_H

#include "Analysis/ana_base.h"

namespace larlite {
  /**
     \class BaseFilter
     User custom analysis class made by SHELL_USER_NAME
   */
  class BaseFilter : public ana_base{
  
  public:

    /// Default constructor
    BaseFilter(){ _name="BaseFilter"; _fout=0;}

    /// Default destructor
    virtual ~BaseFilter(){}

    /** IMPLEMENT in BaseFilter.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in BaseFilter.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in BaseFilter.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

    void setProducer(std::string s){_producer = s;}

  protected:
    std::string _producer;
  };
}
#endif

//**************************************************************************
// 
// For Analysis framework documentation, read Manual.pdf here:
//
// http://microboone-docdb.fnal.gov:8080/cgi-bin/ShowDocument?docid=3183
//
//**************************************************************************

/** @} */ // end of doxygen group 
