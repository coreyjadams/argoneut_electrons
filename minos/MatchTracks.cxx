#ifndef ERTOOL_ERFILTERMATCHTRACKS_CXX
#define ERTOOL_ERFILTERMATCHTRACKS_CXX

#include "MatchTracks.h"
#include "DataFormat/storage_manager.h"
#include "TVector3.h"
#include "math.h"

namespace larlite {

  bool MatchTracks::initialize() {

	  std::cout<<"\nIntializing "<<std::endl;

	  SetFclParams(20.0,35.0,1.0,1.0,0.5,5.0);

	  fAnaTree=nullptr;
	  PrepareTTree();

	  _count =0;

      fDiffDirCosX = new TH1D("fDiffDirCosX","Diff: DirCosX", 100,-2.0,2.0);
      fDiffDirCosY = new TH1D("fDiffDirCosY","Diff: DirCosY", 100,-2.0,2.0);
      fDiffDirCosZ = new TH1D("fDiffDirCosZ","Diff: DirCosZ", 100,-2.0,2.0);
      fDiffR       = new TH1D("fDiffR","Diff: Radial XY",200,0.0,200.0);
      fDiffTotal       = new TH1D("fDiffTotal","Diff: Radial XY/cos(theta)",200,0.0,200.0);
      fDiffXvDiffY = new TH2D("fDiffXvDiffY","Diff: X v Y",200,-100.0,100.0,200,-100.0,100.0);
      fT962_DirCosX   = new TH1D("fT962_DirCosX","T962 Track DirCosX",100, -1.0,1.0); 
      fT962_DirCosY   = new TH1D("fT962_DirCosY","T962 Track DirCosY",100, -1.0,1.0); 
      fT962_DirCosZ   = new TH1D("fT962_DirCosZ","T962 Track DirCosZ",100, -1.0,1.0); 
      fT962_StartZY =   new TH2D("fT962_StartZY","T962 Track Start ZY",100,-5.0,95.0,50,-25.0,25.0);
      fT962_EndZY =     new TH2D("fT962_EndZY","T962 Track End ZY",100,-5.0,95.0,50,-25.0,25.0);
      fT962_StartXY =   new TH2D("fT962_StartXY","T962 Track Start XY",60,-5.0,55.0,50,-25.0,25.0);
      fT962_EndXY =     new TH2D("fT962_EndXY","T962 Track End XY",60,-5.0,55.0,50,-25.0,25.0);
      fT962_StartZX =   new TH2D("fT962_StartZX","T962 Track Start ZX",100,-5.0,95.0,60,-5.0,55.0);
      fT962_EndZX =     new TH2D("fT962_EndZX","T962 Track End ZX",100,-5.0,95.0,60,-5.0,55.0);
      fT962_Ntracks =   new TH1D("fT962_Ntracks","T962 # Tracks",20,0.0,20.0);
      fMINOS_Ntracks =   new TH1D("fMINOS_Ntracks","MINOS # Tracks",20,0.0,20.0);
      fMinos_DirCosX   = new TH1D("fMinos_DirCosX","MINOS Track DirCosX",100, -1.0,1.0); 
      fMinos_DirCosY   = new TH1D("fMinos_DirCosY","MINOS Track DirCosY",100, -1.0,1.0); 
      fMinos_DirCosZ   = new TH1D("fMinos_DirCosZ","MINOS Track DirCosZ",100, -1.0,1.0); 
      fMinos_XY     = new TH2D("fMinos_XY","MINOS Start XY",600,-300.0,300.0,600,-300.0,300.0);
      fMinosErange_Pos = new TH1D("fMinosErange_Pos","MINOS + Charge Tracks: Erange",1000,0.0,10000.0);
      fMinosErange_Neg = new TH1D("fMinosErange_Neg","MINOS - Charge Tracks: Erange",1000,0.0,10000.0);
      fMinosMom_Pos = new TH1D("fMinosMom_Pos","MINOS + Charge Tracks: Momentum",1000,0.0,10000.0);
      fMinosMom_Neg = new TH1D("fMinosMom_Neg","MINOS - Charge Tracks: Momentum",1000,0.0,10000.0);
      fDiffXvD = new TH2D("fDiffXvD","Diff: X Pred. - MINOS x",100,-200.0,200.0,100,90.0,190.0);
      fDiffYvD = new TH2D("fDiffYvD","Diff: Y Pred. - MINOS y",100,-200.0,200.0,100,90.0,190.0);  

    return true;
  }

 void MatchTracks::PrepareTTree(){
	if(!fAnaTree){
		fAnaTree = new TTree("ana_tree","");
		fAnaTree->Branch("_count",&_count,"count/I");
	}

	}



  bool MatchTracks::analyze(storage_manager* storage)
  {
	 _count++;
	fAnaTree->Fill();
	 
     auto MinosTrackHandle = storage->get_data<larlite::event_minos>("minos"); //tracks");
     auto LArTrackHandle = storage->get_data<larlite::event_track>("ct"); //mcreco");
     if(MinosTrackHandle->size()==0 || LArTrackHandle->size()==0)
 		return -1;
 
    // Associations:
	AssUnit_t ass;
    AssSet_t minos_to_argoTrack;
    minos_to_argoTrack.reserve(MinosTrackHandle->size());
	event_track lar_tag[MinosTrackHandle->size()] ;

	fT962_Ntracks->Fill(LArTrackHandle->size());
	fMINOS_Ntracks->Fill(MinosTrackHandle->size());

//	std::cout<<"size of tracks : "<<LArTrackHandle->size()<<std::endl;
	 int exiting=0;
  	 for(unsigned int i=0; i<LArTrackHandle->size();++i){
         if(EndsOnBoundary(LArTrackHandle->at(i))) ++exiting;
        }

	 std::cout<<"Exiting T962 tracks: "<<exiting<<std::endl;	

//the following makes sure that only 1 argoneut track is assigned to one minos track and that the match is the strongest (in terms of projected radial difference and angle between the tracks) among the candidate matches       
      for(unsigned int i=0; i<LArTrackHandle->size();++i)
      {
//		assn.clear() ;
		auto lar_track = LArTrackHandle->at(i);
         double totaldiff2=1000000.;
         double rdiff_best = -999.0;
         double xdiff_best = -999.0;
         double ydiff_best = -999.0;
         unsigned int matchnumber = 999;
         if(!EndsOnBoundary(LArTrackHandle->at(i))) continue;//track doesn't leave TPC

	     std::vector<double> larStart(3,0), larEnd(3,0);
	     larStart[0] = lar_track.vertex_at(0)[0];
	     larStart[1] = lar_track.vertex_at(0)[1];
	     larStart[2] = lar_track.vertex_at(0)[2];
	     larEnd[0] = lar_track.vertex_at(lar_track.n_point()-1)[0];
	     larEnd[1] = lar_track.vertex_at(lar_track.n_point()-1)[1];
	     larEnd[2] = lar_track.vertex_at(lar_track.n_point()-1)[2];
	  
	     std::vector<double> lardirectionStart(3,0), lardirectionEnd(3,0);
	     lardirectionStart[0] = lar_track.direction_at(0)[0];	
	     lardirectionStart[1] = lar_track.direction_at(0)[1];	
	     lardirectionStart[2] = lar_track.direction_at(0)[2];	
	     lardirectionEnd[0] = lar_track.direction_at(lar_track.n_point()-1)[0];	
	     lardirectionEnd[1] = lar_track.direction_at(lar_track.n_point()-1)[1];	
	     lardirectionEnd[2] = lar_track.direction_at(lar_track.n_point()-1)[2];	

//		std::cout<<"size of tracks : "<<MinosTrackHandle->size()<<std::endl;

         for(unsigned int j=0; j<MinosTrackHandle->size();++j)
         {
            if((100.0*MinosTrackHandle->at(j).ftrkVtxZ)>fdZ) continue;//MINOS track too far into detector
	
 //some plots to check alignment
            for(int n = 0; n<100;++n){
               double D = 90.0 + n*1.0;
               double dz = D - larEnd[2]+(100.0 * MinosTrackHandle->at(j).ftrkVtxZ);//z-difference between end of T962 track and
               
               double l = dz/(lardirectionEnd[2]);//3-d distance between end of T962 track and begin of MINOS track
               
               double x_pred = l*lardirectionEnd[0]+larEnd[0];//predicted x-pos. of T962 track at z-position equal to
               //start of MINOS track
               double y_pred = l*lardirectionEnd[1]+larEnd[1];//predicted y-pos. of T962 track at z-position equal to
               //start of MINOS track
               
               fDiffXvD->Fill((100.0*MinosTrackHandle->at(j).ftrkVtxX - x_pred),D);
               fDiffYvD->Fill((100.0*MinosTrackHandle->at(j).ftrkVtxY - y_pred),D);
            } 

			double xdiff,ydiff,rdiff,totaldiff;   
            bool match = Compare(LArTrackHandle->at(i),MinosTrackHandle->at(j),xdiff,ydiff,rdiff,totaldiff,totaldiff2);  
            if(match){
               matchnumber = j;//keep track of best match found so far
               rdiff_best = rdiff;
               xdiff_best = xdiff;
               ydiff_best = ydiff;
            }
	  	  }//loop over minos tracks

         //Check if best match was found, then associate it to Track
         if(matchnumber!=999){
			auto minostrack = MinosTrackHandle->at(matchnumber);

			std::cout<<"Filling things"<<std::endl;
            fDiffR->Fill(rdiff_best);
            fDiffTotal->Fill(totaldiff2);
            fDiffXvDiffY->Fill(xdiff_best,ydiff_best);
            fDiffDirCosX->Fill(lardirectionStart[0]-minostrack.ftrkdcosx);
            fDiffDirCosY->Fill(lardirectionStart[1]-minostrack.ftrkdcosy);
            fDiffDirCosZ->Fill(lardirectionStart[2]-minostrack.ftrkdcosz);
            fT962_DirCosX->Fill(lardirectionStart[0]);
            fT962_DirCosY->Fill(lardirectionStart[1]);
            fT962_DirCosZ->Fill(lardirectionStart[2]);
            fT962_StartZY->Fill(larStart[2],larStart[1]);
            fT962_EndZY->Fill(larEnd[2],larEnd[1]);
            fT962_StartXY->Fill(larStart[0],larStart[1]);
            fT962_EndXY->Fill(larEnd[0],larEnd[1]);
            fT962_StartZX->Fill(larStart[2],larStart[0]);
            fT962_EndZX->Fill(larEnd[2],larEnd[0]);
            fMinos_DirCosX->Fill(minostrack.ftrkdcosx);
            fMinos_DirCosY->Fill(minostrack.ftrkdcosy);
            fMinos_DirCosZ->Fill(minostrack.ftrkdcosz);
            fMinos_XY->Fill(100.0*minostrack.ftrkVtxX,100.0*minostrack.ftrkVtxY);
            if(minostrack.fcharge==1){
               fMinosErange_Pos->Fill(1000.0*minostrack.ftrkErange);
               fMinosMom_Pos->Fill(1000.0*minostrack.ftrkmom);
            }
            if(minostrack.fcharge==-1){
               fMinosErange_Neg->Fill(1000.0*minostrack.ftrkErange);
               fMinosMom_Neg->Fill(1000.0*minostrack.ftrkmom);
            }


		   //make Association between T962 track and matched MINOS track
			std::cout<<"making association..."<<std::endl;
 		   lar_tag->push_back(LArTrackHandle->at(i));
 		   ass.push_back(lar_tag->size()-1);
    	   minos_to_argoTrack.push_back(ass);  
			std::cout<<"association made successfully"<<std::endl;
         }//if(matchnumber!=999)

	  }//loop over T962 tracks

    MinosTrackHandle->set_association(lar_tag->id(),minos_to_argoTrack);
			std::cout<<"or was it?"<<std::endl;

	return true;
	
  }

bool MatchTracks::Compare(const larlite::track& lar_track,const larlite::minos& minos_track, double &xpred, double &ypred, double &rdiff, double &totaldiff, double &totaldiff2){

   double D=(90*0.5)+(42.4*2.54)-5.588; //distance from the front (upstream) of the TPC to the 1st Minos plane 
                                        //(this minus number is the one we measured with Mitch)
   double x_offset=117.4; // previously 116.9;
   double y_offset=19.3; // previously  20.28;
	
   std::vector<double> larStart(3,0), larEnd(3,0);
   larStart[0] = lar_track.vertex_at(0)[0];
   larStart[1] = lar_track.vertex_at(0)[1];
   larStart[2] = lar_track.vertex_at(0)[2];
   larEnd[0] = lar_track.vertex_at(lar_track.n_point()-1)[0];
   larEnd[1] = lar_track.vertex_at(lar_track.n_point()-1)[1];
   larEnd[2] = lar_track.vertex_at(lar_track.n_point()-1)[2];

//   lar_track->Extent(larStart,larEnd);//put xyz coordinates at begin/end of track into vectors(?)
	
   //std::cout << "larStart = (" << larStart[0] << "," << larStart[1] << "," << larStart[2] << ")" << std::endl;
   //std::cout << "larEnd = (" << larEnd[0] << "," << larEnd[1] << "," << larEnd[2] << ")" << std::endl;
	
   std::vector<double> lardirectionStart(3,0), lardirectionEnd(3,0);
   lardirectionStart[0] = lar_track.direction_at(0)[0];	
   lardirectionStart[1] = lar_track.direction_at(0)[1];	
   lardirectionStart[2] = lar_track.direction_at(0)[2];	
   lardirectionEnd[0] = lar_track.direction_at(lar_track.n_point()-1)[0];	
   lardirectionEnd[1] = lar_track.direction_at(lar_track.n_point()-1)[1];	
   lardirectionEnd[2] = lar_track.direction_at(lar_track.n_point()-1)[2];	
//   lar_track->Direction(lardirectionStart,lardirectionEnd);
	
   //begin of MINOS track...in centimeters
   double dz = D - larEnd[2]+(100.0 * minos_track.ftrkVtxZ);//z-difference between end of T962 track and
	
   double l = dz/(lardirectionEnd[2]);//3-d distance between end of T962 track and begin of MINOS track
	
   double x_pred = l*lardirectionEnd[0]+larEnd[0];//predicted x-pos. of T962 track at z-position equal to
	
   //start of MINOS track
   double y_pred = l*lardirectionEnd[1]+larEnd[1];//predicted y-pos. of T962 track at z-position equal to
	
   //start of MINOS track
   double dx = 100.0*minos_track.ftrkVtxX - x_offset - x_pred;
   double dy = 100.0*minos_track.ftrkVtxY + y_offset - y_pred;
	
   if(100.0*minos_track.ftrkVtxZ > fdZ){
      //std::cout << "MINOS track #" << minos_track.ftrkIndex << " is too far into the detector.  Fail!" << std::endl;
      return false;//MINOS track starts too far into the detector 
   }

if((x_pred + x_offset)>297.7 
      || (x_pred+x_offset)<-187.45 
      || (y_pred-y_offset)>181.71 
      || (y_pred-y_offset)<-100.29){
      return false;//predicted coordinates lie outside the MINOS detector boundaries
   }
	
   if(sqrt(dx*dx + dy*dy) > fdXY){
      return false;//predicted coordinates too far from XY-pos. at start of MINOS track
   }
	
   if( fabs(lardirectionEnd[0]-minos_track.ftrkdcosx) > fdCosx){
      return false;
   }
	
   if( fabs(lardirectionEnd[1]-minos_track.ftrkdcosy) > fdCosy){
      return false;
   }
	
   if( fabs(lardirectionEnd[2]-minos_track.ftrkdcosz) > fdCosz){
      return false;
   }
	
	
   xpred = dx;
   ypred = dy;
   rdiff = sqrt(dx*dx + dy*dy);
   //totaldiff is a measure of the agreement between the ArgoNeuT projected track and the MINOS track based on radial distance and angle. totaldiff= rdiff/cos(theta)  
   totaldiff=fabs(rdiff/((lardirectionEnd[0]*minos_track.ftrkdcosx)+(lardirectionEnd[1]*minos_track.ftrkdcosy)+(lardirectionEnd[2]*minos_track.ftrkdcosz)));
	
   if(totaldiff2>totaldiff){
      totaldiff2 = totaldiff;//new best match found
      return true;
   }
	
   else return false;//found match, but not best match
	
}
	

bool MatchTracks::EndsOnBoundary(const larlite::track& lar_track){

   std::vector<double> larStart (3,0); 
   std::vector<double> larEnd (3,0); 

   larStart[0] = lar_track.vertex_at(0)[0];
   larStart[1] = lar_track.vertex_at(0)[1];
   larStart[2] = lar_track.vertex_at(0)[2];
   larEnd[0] = lar_track.vertex_at(lar_track.n_point()-1)[0];
   larEnd[1] = lar_track.vertex_at(lar_track.n_point()-1)[1];
   larEnd[2] = lar_track.vertex_at(lar_track.n_point()-1)[2];

   std::vector<double> lardirectionStart (3,0);
   std::vector<double> lardirectionEnd (3,0);
   lardirectionStart[0] = lar_track.direction_at(0)[0];	
   lardirectionStart[1] = lar_track.direction_at(0)[1];	
   lardirectionStart[2] = lar_track.direction_at(0)[2];	
   lardirectionEnd[0] = lar_track.direction_at(lar_track.n_point()-1)[0];	
   lardirectionEnd[1] = lar_track.direction_at(lar_track.n_point()-1)[1];	
   lardirectionEnd[2] = lar_track.direction_at(lar_track.n_point()-1)[2];	

      if(fabs(larEnd[0])<fdBoundary
         || fabs(47.-larEnd[0])<fdBoundary 
         || fabs(larEnd[1]+20.)<fdBoundary
         || fabs(20.-larEnd[1])<fdBoundary
         || fabs(larEnd[2])<fdBoundary
         || fabs(90.-larEnd[2])<fdBoundary ){
		//std::cout<<"returning true..."<<std::endl;
         return true;  
		}

      else {
//		std::cout<<"returning false..."<<std::endl;
		return false;
		}
  }


 bool MatchTracks::finalize() {

   if(_fout) {
        _fout->cd();
 		fDiffDirCosX->Write();
 		fDiffDirCosY->Write();
 		fDiffDirCosZ->Write();
 		fDiffR->Write();
 		fDiffTotal->Write();
 		fDiffXvDiffY->Write();
 		fT962_DirCosX->Write();
 		fT962_DirCosY->Write();
 		fT962_DirCosZ->Write();
 		fT962_StartZY->Write();
 		fT962_EndZY->Write();
 		fT962_StartXY->Write();
 		fT962_EndXY->Write();
 		fT962_StartZX->Write();
 		fT962_EndZX->Write();
 		fT962_Ntracks->Write();
 		fMINOS_Ntracks->Write();
 		fMinos_DirCosX->Write();
 		fMinos_DirCosY->Write();
 		fMinos_DirCosZ->Write();
 		fMinos_XY->Write();
 		fMinosErange_Pos->Write();
 		fMinosErange_Neg->Write();
 		fMinosMom_Pos->Write();
 		fMinosMom_Neg->Write();
 		fDiffXvD->Write();
 		fDiffYvD->Write();
		fAnaTree->Write();
     }   
   
     else
       print(larlite::msg::kERROR,__FUNCTION__,"Did not find an output file pointer!!! File not opened?");

	delete fAnaTree ;
   
    return true;
  }


}

#endif
