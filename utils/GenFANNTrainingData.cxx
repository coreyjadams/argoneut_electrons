#ifndef LARLITE_GENFANNTRAININGDATA_CXX
#define LARLITE_GENFANNTRAININGDATA_CXX

#include "GenFANNTrainingData.h"

#include "ClusterRecoUtil/Alg/DefaultParamsAlg.h"

#include "DataFormat/cluster.h"
#include "DataFormat/mctruth.h"
#include "DataFormat/mcshower.h"

// #include "ClusterRecoUtil/Alg/ClusterParamsAlg.h"


namespace argoutils {

  bool GenFANNTrainingData::initialize() {

    //
    // This function is called in the beggining of event loop
    // Do all variable initialization you wish to do here.
    // If you have a histogram to fill in the event loop, for example,
    // here is a good place to create one on the heap (i.e. "new TH1D"). 
    //

    // Initialize the histograms for the track/shower variables:


    // Initialize the file to write to:

    _out.open (_tempFileName);
    _n_training_items = 0;

    // create a dummy CPAN to find out the length of the output:
    ::cluster::cluster_params params;
    std::vector<float> tempVec;
    params.GetFANNVector(tempVec);

    // Prepare the histograms:
    std::vector<std::string> legend = params.GetFANNVectorTitle();
    _track_params.resize(legend.size());
    _shower_params.resize(legend.size());

    for (unsigned int i = 0; i < legend.size(); i ++ ){
      char name[20];
      sprintf(name,"track_%i",i);
      _track_params.at(i) = new TH1F(name,legend.at(i).c_str(),50,-2,2);
      _track_params.at(i) -> SetLineColor(kBlue);
      sprintf(name,"shower_%i",i);
      _shower_params.at(i) = new TH1F(name,legend.at(i).c_str(),50,-2,2);
      _shower_params.at(i) -> SetLineColor(kRed);
    }

    _n_inputs = tempVec.size();
    _n_outputs = 2;

    return true;
  }
  
  bool GenFANNTrainingData::analyze(::larlite::storage_manager* storage) {
  
    //
    // Do your event-by-event analysis here. This function is called for 
    // each event in the loop. You have "storage" pointer which contains 
    // event-wise data. To see what is available, check the "Manual.pdf":
    //
    // http://microboone-docdb.fnal.gov:8080/cgi-bin/ShowDocument?docid=3183
    // 
    // Or you can refer to Base/DataFormatConstants.hh for available data type
    // enum values. Here is one example of getting PMT waveform collection.
    //
    // event_fifo* my_pmtfifo_v = (event_fifo*)(storage->get_data(DATA::PMFIFO));
    //
    // if( event_fifo )
    //
    //   std::cout << "Event ID: " << my_pmtfifo_v->event_id() << std::endl;
    //

    // This module requires both the cluster data and the mctruth data
    // check for both and only proceed if both exists
    auto ev_truth = storage->get_data<::larlite::event_mctruth>("generator");
    if (ev_truth -> size() == 0) {
      std::cout << "No mctruth info available!\n";
      exit(-1);
    }
    // auto ev_clust = storage->get_data<event_cluster>(_cluster_producer);
  
    // Get the list of particles and find the primary particle (mother == 0):
    auto particles = ev_truth -> front().GetParticles();

    ::larlite::mcpart primary;
    for (auto & part : particles){
      // std::cout << "ID is " << part.TrackId() 
      // << ", Pdg is: " << part.PdgCode() 
      // << ", Mother is " << part.Mother() <<std::endl;
      if (part.Mother() == -1){
        primary = part;
        break;
      }
    }

    // Decide on whether this particle is a track, shower, or junk
    // If junk, just bail!

    // Get the pdg of the primary:
    int pdg = primary.PdgCode();
    // std::cout << "Primary pdg is " << pdg << "\n";
    clusterClassification answer;
    if (abs(pdg) == 11) answer = kShower;
    else if(abs(pdg) == 13) answer = kTrack;
    else return false;

    // Determine if junk - TODO

    // // Make a cut on the amount of deposited energy of a shower (requires mcshower)
    // if (answer == kShower){
    //   // Get the mcshower:
    //   auto mc_shower = storage->get_data<event_mcshower>("mcreco");
    //   std::cout << "N showers is : " << mc_shower -> size() << std::endl;
    //   if (mc_shower->size() == 0){
    //     std::cerr << "Didn't find showers, error!\n";
    //     return false;
    //   }
    //   if (mc_shower->size() > 1){
    //     std::cerr << "Found multiple showers, error!\n";
    //     return false;
    //   }
    //   if ((mc_shower->front().PdgCode()) != 11){
    //     std::cerr << "Not an electron shower, error!\n";
    //     return false;
    //   }
    //   if (mc_shower->front().DetProfile().E() < 0.2){
    //     std::cout << "Message: skipping this event since the deposited energy is low.\n";
    //     return false;;
    //   }
    // }

    // Have the answer, now get the input to FANN

    // Use cruhelper to generate the list of CPAN:
    std::vector<::cluster::cluster_params> clusterVec;
    helper.GenerateParams(storage,_cluster_producer,clusterVec);
    if (clusterVec.size() == 0){
      std::cout << "No clusters obtained!\n";
      return false;
    }

    // Need a params alg:
    ::cluster::DefaultParamsAlg alg;

    // Find the biggest cluster:
    ::cluster::cluster_params * mainCluster = 0;
    size_t current_hits = 0;
    for ( auto & clust : clusterVec){
      alg.FillParams(clust);
      // std::cout << "This plane is " << clust.Plane() << "\n";
      if (clust.plane_id.Plane == 0) continue;
      if (clust.hit_vector.size() > current_hits){
        mainCluster = &(clust);
        current_hits = mainCluster -> hit_vector.size();
      }
    }

    if (mainCluster == 0 ) return false;

    // Want to require at least, say, 40 hits to be a shower or track?
    if (mainCluster -> hit_vector.size() < 40) return false;

    // mainCluster -> FillParams();

    std::vector<float> input;
    mainCluster -> GetFANNVector(input);
    // Print the info for debugging:
    // std::cout << "\n\nEvent: " << ev_truth -> event_id() 
    //           << ", plane " << mainCluster -> Plane()
    //           << std::endl;
    // if (answer == kTrack)
    //   std::cout << "This is a track.\n";
    // else
    //   std::cout << "This is a shower.\n";

    // mainCluster -> PrintFANNVector();

    // std::cout << "\n\n";

    // Have the answer, have the input.  Write it to file;
    int i = 0;
    for(auto f : input){
      _out << f << " ";
      if (answer == kShower)
        _shower_params.at(i) -> Fill(f);
      if (answer == kTrack)
        _track_params.at(i) -> Fill(f);
      i++;
    }
    _out << "\n";
    if (answer == kShower){
      _out << "1 0" << "\n";
    }
    else
      _out << "0 1" << "\n";

    _n_training_items ++;

    return true;
  }

  bool GenFANNTrainingData::finalize() {

    // This function is called at the end of event loop.
    // Do all variable finalization you wish to do here.
    // If you need, you can store your ROOT class instance in the output
    // file. You have an access to the output file through "_fout" pointer.
    //
    // Say you made a histogram pointer h1 to store. You can do this:
    //
    // if(_fout) { _fout->cd(); h1->Write(); }
    //
    // else 
    //   print(MSG::ERROR,__FUNCTION__,"Did not find an output file pointer!!! File not opened?");
    //

    // write the output histograms:
    if(_fout) { 
      TCanvas * canv = new TCanvas("canv","canv",800,600);

      _fout->cd(); 
      for (unsigned int i = 0; i < _track_params.size(); i++){
        _track_params.at(i) -> Write();
        _shower_params.at(i) -> Write();
        // Make a combined histogram:
        canv -> cd();
        canv -> Clear();
        _track_params.at(i)  -> Draw();
        _shower_params.at(i) -> Draw("same");
        canv -> Write();
      } 

    }

    // Close the output file:
    _out.close();

    // Unfortunately, the first line of the fann training file needs to know
    // the number of training data sets in the file.  But of course we don't know
    // that until the very end.
    // Solution: since the training files aren't crazy, read it in and write it out
    // with the right header.  Only happens once per run so no big deal.

    if (_output_file_name == ""){
      _output_file_name = "defaultFannTrainingFile.txt";
    }
    std::ofstream trueOutputFile;
    trueOutputFile.open(_output_file_name);
    // Write the header information:
    trueOutputFile  << _n_training_items << " " << _n_inputs 
                    << " " << _n_outputs << "\n";

    // reopen the old file:
    std::ifstream _in;
    _in.open(_tempFileName);
    std::string line;

    // copy the other file:
    while (getline(_in,line))
    {
      trueOutputFile << line << "\n";
    }

    // close the output file:
    trueOutputFile.close();
    _in.close();

    // delete the temporary file:
    std::remove(_tempFileName.c_str());


    return true;
  }

}
#endif
