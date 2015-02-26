/**
 * \file MatchTracks.h
 *
 * \ingroup minos
 * 
 * \brief Class def header for a class MatchTracks
 *
 * @author ariana Hackenburg
 */

/** \addtogroup minos

    @{*/

#ifndef MATCHTRACKS_H 
#define MATCHTRACKS_H 

#include "Analysis/ana_base.h"
#include "DataFormat/track.h"
#include "DataFormat/minos.h"
#include "TH2D.h"
#include "TH1D.h"

namespace larlite {

  /**
     \class MatchTracks
     User custom SPAFilter class made by kazuhiro

   */
  class MatchTracks : public ana_base {
  
  public:

    /// Default constructor
    MatchTracks(){ _name="MatchTracks", _fout=0;}

    /// Default destructor
    virtual ~MatchTracks(){};

    /// Select function
    virtual bool analyze (storage_manager * storage);
	
	virtual bool initialize();

	virtual bool finalize();
      
  protected:
    
	bool Compare(const larlite::track&,const larlite::minos&, double &xpred, double &ypred, double &rdiff, double &totaldiff, double &totaldiff2);

	bool EndsOnBoundary(const larlite::track&);

	//Sets the 6 params below according to default fcl file params (matchtracksmodule.fcl) 
	void SetFclParams(float dZ, float dXY, float dCosx, float dCosy, float dCosz, float dBoundary)
		{
			fdZ = dZ;
			fdXY = dXY;
			fdCosx = dCosx;
			fdCosy = dCosy;
			fdCosz = dCosz;
			fdBoundary = dBoundary;
			}

 	double fdZ;    //max. z distance between projected T962 track and MINOS track
    double fdXY;   //max. radial distance between projected T962 track and MINOS track
    double fdCosx; //tolerance in dircosX matching
    double fdCosy; //tolerance in dircosY matching
    double fdCosz; //tolerance in dircosZ matching
    double fdBoundary; //distance from a boundary to be considered a track that "ends on a boundary"

	  TH1D* fDiffDirCosX;
      TH1D* fDiffDirCosY;
      TH1D* fDiffDirCosZ;
      TH1D* fDiffR;
      TH1D* fDiffTotal;
      TH2D* fDiffXvDiffY;
      TH1D* fT962_DirCosX;
      TH1D* fT962_DirCosY;
      TH1D* fT962_DirCosZ;
      TH2D* fT962_StartZY;
      TH2D* fT962_EndZY;
      TH2D* fT962_StartXY;
      TH2D* fT962_EndXY;
      TH2D* fT962_StartZX;
      TH2D* fT962_EndZX;
      TH1D* fT962_Ntracks;
      TH1D* fMINOS_Ntracks; 
      TH1D* fMinos_DirCosX;
      TH1D* fMinos_DirCosY;
      TH1D* fMinos_DirCosZ;
      TH2D* fMinos_XY;
      TH1D* fMinosErange_Pos;
      TH1D* fMinosErange_Neg;
      TH1D* fMinosMom_Pos;
      TH1D* fMinosMom_Neg;
      TH2D* fDiffXvD;
      TH2D* fDiffYvD;


  };
}
#endif

/** @} */ // end of doxygen group 
