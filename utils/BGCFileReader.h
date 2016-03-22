/**
 * \file BGCFileReader.h
 *
 * \ingroup utils
 *
 * \brief Class def header for a class BGCFileReader
 *
 * @author cadams
 */

/** \addtogroup utils

    @{*/
#ifndef BGCFILEREADER_H
#define BGCFILEREADER_H

#include <iostream>
#include <vector>
#include <map>
#include "TTree.h"
#include "TFile.h"


/**
   \class BGCFileReader
   User defined class BGCFileReader ... these comments are used to generate
   doxygen documentation!
 */

namespace argoutils{

class BGCData {


public:

  int                  run;
  int                  subrun;
  int                  event;
  unsigned int         nplanes;
  int                  currplane;
  int                  setid;
  std::vector<double>  starthit;
  std::vector<double>  starthiterror;
  std::vector<double>  endhit;
  std::vector<double>  endhiterror;
  std::vector<double>  hitwire;
  std::vector<double>  hittime;


};

class BGCFileReader {

public:

  /// Default constructor
  BGCFileReader() {}

  /// Default destructor
  ~BGCFileReader() {}

  void setInputFile(std::string s);

  void initBranches();

  void readData();

  long LoadTree(long entry);

  std::map< int, std::map< int,  std::vector<BGCData> > > getData(){return _clusterData;}

private:

  std::vector< BGCData > _bgc_data_set;

  std::string _input_file;

  TTree          *fChain;   //!pointer to the analyzed TTree or TChain

  // Storing the cluster info in a map of [run][event][vectorOfClusters]

  std::map< int, std::map< int,  std::vector<BGCData> > > _clusterData;


  // Declaration of leaf types
  Int_t           run;
  Int_t           subrun;
  Int_t           event;
  UInt_t          nplanes;
  Int_t           currplane;
  Int_t           setid;
  std::vector<double>  *starthit;
  std::vector<double>  *starthiterror;
  std::vector<double>  *endhit;
  std::vector<double>  *endhiterror;
  std::vector<double>  *hitwire;
  std::vector<double>  *hittime;

  // List of branches
  TBranch        *b_run;   //!
  TBranch        *b_subrun;   //!
  TBranch        *b_event;   //!
  TBranch        *b_nplanes;   //!
  TBranch        *b_currentplane;   //!
  TBranch        *b_setid;   //!
  TBranch        *b_starthit;   //!
  TBranch        *b_starthiterror;   //!
  TBranch        *b_endhit;   //!
  TBranch        *b_endhiterror;   //!
  TBranch        *b_hitwire;   //!
  TBranch        *b_hittime;   //!

};


}

#endif
/** @} */ // end of doxygen group

