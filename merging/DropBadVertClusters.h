/**
 * \file DropBadVertClusters.h
 *
 * \ingroup merging
 * 
 * \brief Class def header for a class DropBadVertClusters
 *
 * @author cadams
 */

/** \addtogroup merging

    @{*/

#ifndef LARLITE_DROPBADVERTCLUSTERS_H
#define LARLITE_DROPBADVERTCLUSTERS_H

#include "Analysis/ana_base.h"
#include "ClusterRecoUtil/CRUHelper.h"

namespace larlite {
  /**
     \class DropBadVertClusters
     User custom analysis class made by SHELL_USER_NAME
   */
  class DropBadVertClusters : public ana_base{
  
  public:

    /// Default constructor
    DropBadVertClusters(){ _name="DropBadVertClusters"; _fout=0;}

    /// Default destructor
    virtual ~DropBadVertClusters(){}

    /** IMPLEMENT in DropBadVertClusters.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in DropBadVertClusters.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in DropBadVertClusters.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

    void SetInputProducer(std::string s){_input_producer = s;}
    void SetOutputProducer(std::string s){_output_producer = s;}

    bool isBadVertCluster(::cluster::ClusterParamsAlg &cluster);

  protected:
    std::string _input_producer;
    std::string _output_producer;

    ::cluster::CRUHelper helper;

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
