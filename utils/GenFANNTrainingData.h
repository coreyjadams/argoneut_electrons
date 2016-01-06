/**
 * \file GenFANNTrainingData.h
 *
 * \ingroup utils
 * 
 * \brief Class def header for a class GenFANNTrainingData
 *
 * @author cadams
 */

/** \addtogroup utils

    @{*/

#ifndef LARLITE_GENFANNTRAININGDATA_H
#define LARLITE_GENFANNTRAININGDATA_H

#include "Analysis/ana_base.h"
#include "ClusterRecoUtil/Base/CRUHelper.h"

#include <iostream>
#include "stdio.h"
#include "TH1F.h"
#include "TCanvas.h"

namespace argoutils {
  /**
     \class GenFANNTrainingData
     User custom analysis class made by SHELL_USER_NAME
   */
  class GenFANNTrainingData : public ::larlite::ana_base{
  
  public:

    enum clusterClassification{kShower=1,kTrack=0};

    /// Default constructor
    GenFANNTrainingData(){ _name="GenFANNTrainingData"; _fout=0;}

    /// Default destructor
    virtual ~GenFANNTrainingData(){}

    /** IMPLEMENT in GenFANNTrainingData.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in GenFANNTrainingData.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(::larlite::storage_manager* storage);

    /** IMPLEMENT in GenFANNTrainingData.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

    void setOutputFile(std::string s){_output_file_name = s;}

    void setClusterProducer(std::string s){_cluster_producer = s;}

  protected:
    
    std::string _output_file_name;
    const std::string _tempFileName = "temporaryFileBufferDoNotDelete.txt";

    std::string _cluster_producer;

    int _n_training_items;
    int _n_inputs;
    int _n_outputs;

    ::cluster::CRUHelper helper;


    // Histograms to look at the effectiveness of the input parameters:
    std::vector<TH1F *> _track_params;
    std::vector<TH1F *> _shower_params;

    // Output file:
    std::ofstream _out;

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
