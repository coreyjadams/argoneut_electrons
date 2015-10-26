/**
 * \file RemoveTrackHits.h
 *
 * \ingroup merging
 * 
 * \brief Class def header for a class RemoveTrackHits
 *
 * @author cadams
 */

/** \addtogroup merging

    @{*/

#ifndef LARLITE_REMOVETRACKHITS_H
#define LARLITE_REMOVETRACKHITS_H

#include "Analysis/ana_base.h"
#include "ClusterRecoUtil/Base/CRUHelper.h"
#include "ClusterRecoUtil/Alg/DefaultParamsAlg.h"
#include "TVector3.h"
#include "LArUtil/Geometry.h"
 
#include "DataFormat/track.h"

namespace argomerge {
  /**
     \class RemoveTrackHits
     User custom analysis class made by SHELL_USER_NAME
   */
  class RemoveTrackHits : public ::larlite::ana_base{
  
  public:

    /// Default constructor
    RemoveTrackHits(){ _name="RemoveTrackHits"; _fout=0;}

    /// Default destructor
    virtual ~RemoveTrackHits(){}

    /** IMPLEMENT in RemoveTrackHits.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in RemoveTrackHits.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(::larlite::storage_manager* storage);

    /** IMPLEMENT in RemoveTrackHits.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

    void SetClusterProducer(std::string s){_cluster_producer = s;}
    void SetOutputProducer(std::string s){_output_producer = s;}
    void SetTrackProducer(std::string s){_track_producer = s;}



  protected:

    bool isComplete(const ::larlite::track &, const ::larlite::cluster &);

    bool isFiducial(const TVector3 & point);

    std::string _cluster_producer;
    std::string _track_producer;
    std::string _output_producer;

    ::cluster::CRUHelper helper;
    ::cluster::DefaultParamsAlg _params_alg;

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
