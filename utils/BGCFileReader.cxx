#ifndef BGCFILEREADER_CXX
#define BGCFILEREADER_CXX

#include "BGCFileReader.h"

namespace argoutils {

void BGCFileReader::setInputFile(std::string s) {
  _input_file = s;

  TFile *f = new TFile(_input_file.c_str());
  std::cout << "fChain is " << fChain << std::endl;
  f->GetObject("BootLegGraphClusterTree", fChain);


  initBranches();

}

void BGCFileReader::initBranches() {

  // Here we have to connect all the data to the correct branches
  // in the ttree

  starthit = 0;
  starthiterror = 0;
  endhit = 0;
  endhiterror = 0;
  hitwire = 0;
  hittime = 0;

  fChain->SetMakeClass(1);
  fChain->SetBranchAddress("run", &run, &b_run);
  fChain->SetBranchAddress("subrun", &subrun, &b_subrun);
  fChain->SetBranchAddress("event", &event, &b_event);
  fChain->SetBranchAddress("nplanes", &nplanes, &b_nplanes);
  fChain->SetBranchAddress("currplane", &currplane, &b_currentplane);
  fChain->SetBranchAddress("setid", &setid, &b_setid);
  fChain->SetBranchAddress("starthit", &starthit, &b_starthit);
  fChain->SetBranchAddress("starthiterror", &starthiterror, &b_starthiterror);
  fChain->SetBranchAddress("endhit", &endhit, &b_endhit);
  fChain->SetBranchAddress("endhiterror", &endhiterror, &b_endhiterror);
  fChain->SetBranchAddress("hitwire", &hitwire, &b_hitwire);
  fChain->SetBranchAddress("hittime", &hittime, &b_hittime);

}

// long BGCFileReader::LoadTree(long entry)
// {
//   // Set the environment to read one entry
//   if (!fChain) return -5;
//   long centry = fChain->LoadTree(entry);
//   if (centry < 0) return centry;
//   if (fChain->GetTreeNumber() != fCurrent) {
//     fCurrent = fChain->GetTreeNumber();
//   }
//   return centry;
// }

void BGCFileReader::readData() {

  if (fChain == 0) return;

  long nentries = fChain->GetEntriesFast();

  long nbytes = 0, nb = 0;
  for (long jentry = 0; jentry < nentries; jentry++) {
    // long ientry = LoadTree(jentry);
    // if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;
    // if (Cut(ientry) < 0) continue;

    // Now add this entry to the data list
    _clusterData[run][event].push_back(BGCData());

    _clusterData[run][event].back().run =  run;
    _clusterData[run][event].back().subrun =  subrun;
    _clusterData[run][event].back().event =  event;
    _clusterData[run][event].back().nplanes =  nplanes;
    _clusterData[run][event].back().currplane =  currplane;
    _clusterData[run][event].back().setid =  setid;
    _clusterData[run][event].back().starthit =  * starthit;
    _clusterData[run][event].back().starthiterror =  * starthiterror;
    _clusterData[run][event].back().endhit =  * endhit;
    _clusterData[run][event].back().endhiterror =  * endhiterror;
    _clusterData[run][event].back().hitwire =  * hitwire;
    _clusterData[run][event].back().hittime =  * hittime;
  }

  std::cout << "Finished reading " << nentries
            << " entries, with " << nbytes
            << " bytes." << std::endl;
}

}

#endif
