#ifndef LARLITE_VERTEXDIFF_CXX
#define LARLITE_VERTEXDIFF_CXX

#include "VertexDiff.h"
#include "DataFormat/storage_manager.h"
#include "TVector3.h"
#include "math.h"



namespace larlite {

  bool VertexDiff::initialize() {

    //
    // This function is called in the beggining of event loop
    // Do all variable initialization you wish to do here.
    // If you have a histogram to fill in the event loop, for example,
    // here is a good place to create one on the heap (i.e. "new TH1D"). 
    //

    // event = 0;
    output.open("3dout");
    // out2.open("2dout");

    fHvx = new TH1D("fHvx","2dreco x",30,0,65);
    fHvy = new TH1D("fHvy","2dreco y",30,-30,30);
    fHvz = new TH1D("fHvz","2dreco z",50,0,100);
    fHtx = new TH1D("fHtx","3dreco x",30,0,65);
    fHty = new TH1D("fHty","3dreco y",30,-30,30);
    fHtz = new TH1D("fHtz","3dreco z",50,0,100);
    fHmcx = new TH1D("fHmcx","mc x",50,-30,80);
    fHmcy = new TH1D("fHmcy","mc y",60,-60,60);
    fHmcz = new TH1D("fHmcz","mc z",90,-50,130);
    fHdiffX = new TH1D("fHdiffX","difference in x",50,-50,50);
    fHdiffY = new TH1D("fHdiffY","difference in y",50,-50,50);
    fHdiffZ = new TH1D("fHdiffZ","difference in z",100,-100,100);
    fHminX = new TH1D("fHminX","smallest difference in x",50,-50,50);
    fHminY = new TH1D("fHminY","smallest difference in y",50,-50,50);
    fHminZ = new TH1D("fHminZ","smallest difference in z",100,-100,100);
    fHTrdiffX = new TH1D("fHTrdiffX","difference in x",50,-50,50);
    fHTrdiffY = new TH1D("fHTrdiffY","difference in y",50,-50,50);
    fHTrdiffZ = new TH1D("fHTrdiffZ","difference in z",100,-100,100);
    fHTrminX = new TH1D("fHTrminX","smallest difference in x",50,-50,50);
    fHTrminY = new TH1D("fHTrminY","smallest difference in y",50,-50,50);
    fHTrminZ = new TH1D("fHTrminZ","smallest difference in z",100,-100,100);
    fHtot = new TH1D("fHtot","magnitude of difference",50,0,100);
    fHTrtot = new TH1D("fHTrtot","magnitude of difference in Track",50,0,100);
    fHmintot = new TH1D("fHmintot","magnitude of smallest difference",50,0,100);
    fHTrmintot = new TH1D("fHTrmintot","magnitude of smallest difference in Track",50,0,100);

    return true;
  }
  
  bool VertexDiff::analyze(storage_manager* storage) {
  
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


        /// Store data from the root file into Handle variables
        auto VertexHandle = storage->get_data<larlite::event_vertex>("feature");
        auto TrackHandle = storage->get_data<larlite::event_track>("ct");
        // auto MCHandle = storage->get_data<larlite::event_mctrack>("mcreco");
        auto MCHandle = storage->get_data<larlite::event_mctruth>("generator");
        auto neutrino = MCHandle->at(0).GetNeutrino().Nu();

        TrminX = 1000;
        TrminY = 1000;
        TrminZ = 1000;
        minX = 1000;
        minY = 1000;
        minZ = 1000;

        // output<<"event "<<event<<"\n";
        // out2<<"event "<<event<<"\n";

        // for (unsigned int i = 0; i < TrackHandle->size(); ++i)
        // {
            /* code */

        for (unsigned int i = 0; i < TrackHandle->size(); i++)
        {

            if(TrackHandle->size() != 0){
                
                auto track = TrackHandle->at(i);
                auto vertStart = track.vertex_at(0);

                if(track.vertex_at(1).Z() < track.vertex_at(0).Z()){

                    vertStart = track.vertex_at(track.n_point()-1);
                }

                trackX = vertStart.X();
                trackY = vertStart.Y();
                trackZ = vertStart.Z();
                fHtx->Fill(trackX);
                fHty->Fill(trackY);
                fHtz->Fill(trackZ);
            }

            if(TrackHandle->size() != 0 && MCHandle->size() != 0){

                if(mcX > 0 && mcX < 47) fHTrdiffX->Fill(trackX-mcX);
                if(mcY > -20 && mcY < 20) fHTrdiffY->Fill(trackY-mcY);
                if(mcZ > 0 && mcZ < 90) fHTrdiffZ->Fill(trackZ-mcZ);

                if ( mcX > 0 && mcX < 47 && mcY > -20 && mcY < 20 && mcZ > 0 && mcZ < 90){

                    fHTrtot->Fill(pow(pow(trackX-mcX,2)+pow(trackY-mcY,2)+pow(trackZ-mcZ,2),0.5));
                }
            }

            if(std::abs(TrminX) > std::abs(trackX-mcX)) TrminX = trackX-mcX;
            if(std::abs(TrminY) > std::abs(trackY-mcY)) TrminY = trackY-mcY;
            if(std::abs(TrminZ) > std::abs(trackZ-mcZ)) TrminZ = trackZ-mcZ;

            // output<<"X: "<<trackX<<" MC X: "<<mcX<<" Y: "<<trackY<<" MC Y: "<<mcY<<" Z: "<<trackZ<<" MC Z: "<<mcZ<<"\n";
        }

        fHTrminX->Fill(TrminX);
        fHTrminY->Fill(TrminY);
        fHTrminZ->Fill(TrminZ);

        fHTrmintot->Fill(pow(pow(TrminX,2)+pow(TrminY,2)+pow(TrminZ,2),0.5));

            // output<<"vertex at: "<<trackVertex.X()<<", "<<trackVertex.Y()<<", "<<trackVertex.Z()<<"\n";
        // }

        // for (unsigned int j = 0; j < VertexHandle->size(); ++j)
        // {
        //     /* code */
        //     auto twodvert = VertexHandle->at(j);
        //     out2<<"vertex at: "<<twodvert.X()<<", "<<twodvert.Y()<<", "<<twodvert.Z()<<"\n";

        // }

        /// Compare the first vertex with mctruth
        if (MCHandle->size() != 0){

            mcX = neutrino.Trajectory().front().X();
            mcY = neutrino.Trajectory().front().Y();
            mcZ = neutrino.Trajectory().front().Z();

            if(mcX > 0 && mcX < 47) fHmcx->Fill(mcX);
            if(mcY > -20 && mcY < 20) fHmcy->Fill(mcY);
            if(mcZ > 0 && mcZ < 90) fHmcz->Fill(mcZ);
        }

        // if(VertexHandle->size() != 0){

        //     vertX = VertexHandle->at(0).X();
        //     vertY = VertexHandle->at(0).Y();
        //     vertZ = VertexHandle->at(0).Z();

        //     fHvx->Fill(vertX);
        //     fHvy->Fill(vertY);
        //     fHvz->Fill(vertZ);
        // }

        // if(VertexHandle->size() != 0 && MCHandle->size() != 0){

        //     if(mcX > 0 && mcX < 47) fHdiffX->Fill(vertX-mcX);
        //     if(mcY > -20 && mcY < 20) fHdiffY->Fill(vertY-mcY);
        //     if(mcZ > 0 && mcZ < 90) fHdiffZ->Fill(vertZ-mcZ);

        //     if ( mcX > 0 && mcX < 47 && mcY > -20 && mcY < 20 && mcZ > 0 && mcZ < 90){

        //         fHtot->Fill(pow(pow(vertX-mcX,2)+pow(vertY-mcY,2)+pow(vertZ-mcZ,2),0.5));
        //     }
        // }

        /// Compare all vertexes with mctruth
        for (unsigned int i = 0; i < VertexHandle->size(); i++)
        {
            if(VertexHandle->size() != 0){

                vertX = VertexHandle->at(i).X();
                vertY = VertexHandle->at(i).Y();
                vertZ = VertexHandle->at(i).Z();

                fHvx->Fill(vertX);
                fHvy->Fill(vertY);
                fHvz->Fill(vertZ);
            }

            if(VertexHandle->size() != 0 && MCHandle->size() != 0){

                if(mcX > 0 && mcX < 47) fHdiffX->Fill(vertX-mcX);
                if(mcY > -20 && mcY < 20) fHdiffY->Fill(vertY-mcY);
                if(mcZ > 0 && mcZ < 90) fHdiffZ->Fill(vertZ-mcZ);

                if ( mcX > 0 && mcX < 47 && mcY > -20 && mcY < 20 && mcZ > 0 && mcZ < 90){

                    fHtot->Fill(pow(pow(vertX-mcX,2)+pow(vertY-mcY,2)+pow(vertZ-mcZ,2),0.5));
                }
            }

            if(std::abs(TrminX) > std::abs(vertX-mcX)) minX = vertX-mcX;
            if(std::abs(TrminY) > std::abs(vertY-mcY)) minY = vertY-mcY;
            if(std::abs(TrminZ) > std::abs(vertZ-mcZ)) minZ = vertZ-mcZ;
        }
        event++;
        // std::cout<<"particle "<<MCHandle->at(i).GetNeutrino().Nu().PdgCode()<<std::endl;

        fHminX->Fill(minX);
        fHminY->Fill(minY);
        fHminZ->Fill(minZ);

        fHmintot->Fill(pow(pow(minX,2)+pow(minY,2)+pow(minZ,2),0.5));

    return true;
  }

  bool VertexDiff::finalize() {

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

    output.close();
    // out2.close();


    /// Write historams
    fHvx->Write();
    fHvy->Write();
    fHvz->Write();
    fHtx->Write();
    fHty->Write();
    fHtz->Write();
    fHmcx->Write();
    fHmcy->Write();
    fHmcz->Write();
    fHdiffX->Write();
    fHdiffY->Write();
    fHdiffZ->Write();
    fHminX->Write();
    fHminY->Write();
    fHminZ->Write();
    fHTrdiffX->Write();
    fHTrdiffY->Write();
    fHTrdiffZ->Write();
    fHTrminX->Write();
    fHTrminY->Write();
    fHTrminZ->Write();
    fHtot->Write();
    fHTrtot->Write();
    fHmintot->Write();
    fHTrmintot->Write();

    return true;
  }

}
#endif
