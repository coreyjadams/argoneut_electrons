//
// cint script to generate libraries
// Declaire namespace & classes you defined
// #pragma statement: order matters! Google it ;)
//

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class argomerge::CBAlgoMergeSmallToTrack+;
#pragma link C++ class argomerge::CBAlgoMergeSingleToBig+;
#pragma link C++ class argomerge::CBAlgoMergeInline+;
#pragma link C++ class argomerge::CBAlgoMergeDiffuse+;
#pragma link C++ class argomerge::CBAlgoMergeShortestDistance+;
#pragma link C++ class argomerge::CBAlgoMergeExtendBlob+;
#pragma link C++ class argomerge::CBAlgoProhibitBigToBig+;
#pragma link C++ class argomerge::CBAlgoProhibitStartToStart+;
#pragma link C++ class argomerge::CBAlgoProhibitTrackToTrack+;
#pragma link C++ class argomerge::CBAlgoProhibitBigStart+;
#pragma link C++ class argomerge::CBAlgoMergeWithinBoundary+;
#pragma link C++ class argomerge::CBAlgoMergeStartToEnd+;
#pragma link C++ class argomerge::CBAlgoProhibitTrack+;
#pragma link C++ class argomerge::CBAlgoProhibitOutOfCone+;

#pragma link C++ class argomerge::RemoveTrackHits+;

#pragma link C++ class larlite::HitToCluster+;
#pragma link C++ class larlite::DropSingles+;
#pragma link C++ class larlite::DropBadVertClusters+;
//ADD_NEW_CLASS ... do not change this line
#endif


