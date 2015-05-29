#ifndef RECOTOOL_CFALGOWIREOVERLAP_CXX
#define RECOTOOL_CFALGOWIREOVERLAP_CXX

#include "CFAlgoWireOverlap.h"

namespace cmtool {

  //-------------------------------------------------------
  CFAlgoWireOverlap::CFAlgoWireOverlap() : CFloatAlgoBase()
  //-------------------------------------------------------
  {
    _w2cm = larutil::GeometryUtilities::GetME()->WireToCm();
    _t2cm = larutil::GeometryUtilities::GetME()->TimeToCm();

    SetDebug(false) ;
    SetVerbose(false) ;
    RequireThreePlanes(false) ;
  }

  //-----------------------------
  void CFAlgoWireOverlap::Reset()
  //-----------------------------
  {

  }

  //----------------------------------------------------------------------------------------------
  float CFAlgoWireOverlap::Float(const std::vector<const cluster::ClusterParamsAlg*> &clusters)
  //----------------------------------------------------------------------------------------------
  {
    
    // This ensures the algorithm works only if # clusters is > 2 (and not =2)
    // You may take out this block if you want to allow matching using clusters from only 2 planes.
    if(_require_3planes && clusters.size()==2) return -1;
    if( clusters.size() != 2 ) return -1; 
    
    // First, find the showers on the collection plane:
//    cluster::ClusterParamsAlg const *  shower;
    int nshowers = 0;
    // Look for showers on the collection plane:
    for(auto const& c : clusters){
      if (ts.isShower(*c) && c -> Plane() == 1){
   //     shower = c;
        nshowers++;
	}
      }

    if (nshowers != 1) return -1.0;

    //Planes: U == 0; V == 1
    //Get hits for both clusters
    std::vector<std::vector<larutil::PxHit> > Hits(2, std::vector<larutil::PxHit>());

    for (size_t c=0; c < clusters.size(); c++)
      Hits.at( clusters.at(c)->Plane() ) = clusters.at(c)->GetHitVector();

    std::vector<int> StartWires;
    std::vector<int> EndWires;
    std::vector<double> StartTime; 
    std::vector<double> EndTime; 

    //loop over number of planes and pre-fill StartWires and EndWires
    for (int pl=0; pl < 2; pl++){
      StartWires.push_back(9999);
      EndWires.push_back(0);

      StartTime.push_back(9999);
      EndTime.push_back(0);
    }   

    for (size_t h=0; h < Hits.size(); h++){
      for ( auto& hit: Hits.at(h) ){
        if ( Hits.at(h).size() == 0 ){
          std::cout << "Need to insert fake hit ranges...";
        }   
        else{
          if ( int(hit.w / _w2cm) < StartWires.at(h) ) { StartWires.at(h) = int(hit.w / _w2cm)  ; }
          if ( int(hit.w / _w2cm) > EndWires.at(h)   ) { EndWires.at(h)   = int(hit.w / _w2cm)  ; }

	  if ( hit.t < StartTime.at(h)  ) { StartTime.at(h)  = hit.t  ; }
	  if ( hit.t > EndTime.at(h)    ) { EndTime.at(h)    = hit.t  ; }
        }   


      }//for all hits in range
    }//for all hit-lists (i.e. for all clusters)

    double * UWorldStart(0) ;
    double * UWorldEnd(0) ;
    double * VWorldStart(0) ;
    double * VWorldEnd(0) ;

    //Get world coordinates of both clusters' start, end points. Remember shower is in plane V
    WireIDtoWorld(StartWires.at(0), StartTime.at(0), 30., UWorldStart); 
    WireIDtoWorld(EndWires.at(0), EndTime.at(0), 30., UWorldEnd); 
    WireIDtoWorld(StartWires.at(1), StartTime.at(1), 150., VWorldStart); 
    WireIDtoWorld(EndWires.at(1), EndTime.at(1), 150., VWorldEnd); 

    double wireDistU =0;
    double wireDistV =0;

    //Adjust coordinates if necessary so Start(End) represents earliest(latest) in world coordinates.  
    for(int i=0; i<2; i++){		
	wireDistV = VWorldEnd[0] - VWorldStart[0] ;
	wireDistU = UWorldEnd[0] - UWorldStart[0] ;

	if( wireDistV < 0 ){
	    wireDistV *= -1 ;	
	    double temp = 0 ;
	    temp = VWorldEnd[0] ;
	    VWorldEnd[0] = VWorldStart[0] ;
	    VWorldStart[0] = temp ;
	    } 
	else if( wireDistU < 0 ){
	    wireDistU *= -1 ;	
	    double temp = 0 ;
	    temp = UWorldEnd[0] ;
	    UWorldEnd[0] = UWorldStart[0] ;
	    UWorldStart[0] = temp ;
	    } 
    
	}

    //Compare Z world coordinates of start and end of V plane shower and U cluster   
    std::vector<float> showerRange(2,0), otherRange(2,0) ;

    showerRange.front() = VWorldStart[0];
    showerRange.back() = VWorldEnd[0] ;

    otherRange.front() = UWorldStart[0];
    otherRange.back() = UWorldEnd[0];

    float score = getScore(showerRange,otherRange);
    // std::cout << "Score is " << score << std::endl;
    return score;

   }


  //-----------------------------------------------------------------------------------------------
  float CFAlgoWireOverlap::getScore( std::vector<float> showerRange, std::vector<float> otherRange)
  //-----------------------------------------------------------------------------------------------
  {
    // Find out how much these two intervals overlap, and normalize 
    // it to the shower range
    float overlap = -1.0;

    if (otherRange.back()  - showerRange.front() >= 0 && 
         showerRange.back() - otherRange.front()  >= 0)  
    { // overlap
      overlap = fmax(showerRange.front(), otherRange.front())
              - fmin(showerRange.back(), otherRange.back());
    }
    return overlap / (showerRange.back() - showerRange.front());
  }

  //------------------------------
  void CFAlgoWireOverlap::Report()
  //------------------------------
  {
  }
    
  //------------------------------------------------------------------------------------
  void CFAlgoWireOverlap::WireIDtoWorld(int wireID, double time, int angle, double * yz)
  //------------------------------------------------------------------------------------
  {
    //So far we've assumed that both planes are seen in wire and time space where time is vert and wire is horizontal.
    //Here we use wireID to calculate the center of that wire in world coordinates. Then, to calculate the 
    //actual location of the start/end of cluster in world coordinates, need to add in the time offset. 

    double h = larutil::Geometry::GetME()->DetHalfHeight() * 2 ;   //40 cm
    double l = larutil::Geometry::GetME()->DetLength() ;	   //90 cm
    float p = 0.4 ; //Pitch of wires is 4mm
    int N = 240 ;   //Number of wires

    //Time offsets to add back in below.  The minus sign is here to decrease code below. The offset adds to the center
    //of the wire in world coordinates only when time > length/2; in this case we must also adjust the time so to only add the appropriate offset. Otherwise, it lessens. Hence the minus sign. 
    double offsetY = -time * cos(angle);  
    double offsetZ = -time * sin(angle); 

    int sign = 0 ; 
    if( cos(angle) >= 0 )
	sign = 1;
    else
	sign = -1;  

    double length = 0; 
    //For variable length wires that terminate on the left.
    //Z coord is 0, Y coord is 1
    if ( wireID <= 86 ) {
       length = 0.5*h/sin(angle) + 0.5*l*abs(1/cos(angle)) - (N-1)/2*p/sin(angle)*abs(1/cos(angle)) + p*wireID/sin(angle)*abs(1/cos(angle)); 
       if ( time > length / 2 ){
	  time -= (length / 2) ;
	  offsetY = time*cos(angle) ; 
	  offsetZ = time*sin(angle) ; 
	  }
	  
       yz[0] = offsetZ + -0.25*l + 0.25*h*abs(1/tan(angle)) - (N-1)/4*p/sin(angle) + 0.5*p*wireID/sin(angle) ;   
       yz[1] = offsetY + 0.25*h*sign - 0.25*l*tan(angle) + (N-1)/4*p/cos(angle) - 0.5*p*wireID/cos(angle) ;   
	}	
    //For variable length wires that terminate on the left.
    else if ( wireID > 154) {
       if ( time > length / 2 ){
	  time -= (length / 2) ;
	  offsetY = time*cos(angle) ; 
	  offsetZ = time*sin(angle) ; 
	  }
       length = 0.5*h/sin(angle) + 0.5*l*abs(1/cos(angle)) - (N-1)/2*p/sin(angle)*abs(1/cos(angle)) - p*wireID/sin(angle)*abs(1/cos(angle)); 
       yz[0] = offsetZ + 0.25*l - 0.25*h*abs(1/tan(angle)) - (N-1)/4*p/sin(angle) + 0.5*p*wireID/sin(angle) ;   
       yz[1] = offsetY + -0.25*h*sign + 0.25*l*tan(angle) + (N-1)/4*p/cos(angle) - 0.5*p*wireID/cos(angle) ;   
	}
    //For variable length wires that terminate on the left.
    else{
       if ( time > length / 2 ){
	  time -= (length / 2) ;
	  offsetY = time*cos(angle) ; 
	  offsetZ = time*sin(angle) ; 
	  }
	length = h / sin(angle) ;
	yz[0] = offsetZ + -(N-1)/2*p/sin(angle) + p*wireID/sin(angle) ;
	yz[1] = offsetY + 0. ;
	}

    }  
}
#endif
