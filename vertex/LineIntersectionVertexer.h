/**
 * \file LineIntersectionVertexer.h
 *
 * \ingroup vertex
 *
 * \brief Class def header for a class LineIntersectionVertexer
 *
 * @author cadams
 */

/** \addtogroup vertex

    @{*/

#ifndef LARLITE_LINEINTERSECTIONVERTEXER_H
#define LARLITE_LINEINTERSECTIONVERTEXER_H

#include "Analysis/ana_base.h"
#include "ClusterRecoUtil/Alg/DefaultParamsAlg.h"
#include "ClusterRecoUtil/Base/CRUHelper.h"
#include "LArUtil/GeometryHelper.h"


namespace argovertex {
/**
   \class LineIntersectionVertexer
   User custom analysis class made by SHELL_USER_NAME
 */

class interceptCand {
public:
  interceptCand();
  interceptCand(larutil::Point2D p, float s, size_t c1, size_t c2) :
    intercept(p),
    strenght(s),
    cluster_index_1(c1),
    cluster_index_2(c2)
  {}

  larutil::Point2D intercept;
  float strenght;
  size_t cluster_index_1;
  size_t cluster_index_2;
};

class bound{
public:
  bound(){}
  float x_min;
  float x_max;
  float y_min;
  float y_max;
};

class LineIntersectionVertexer : public larlite::ana_base {


public:

  /// Default constructor
  LineIntersectionVertexer();

  /// Default destructor
  virtual ~LineIntersectionVertexer() {}

  /** IMPLEMENT in LineIntersectionVertexer.cc!
      Initialization method to be called before the analysis event loop.
  */
  virtual bool initialize();

  /** IMPLEMENT in LineIntersectionVertexer.cc!
      Analyze a data event-by-event
  */
  virtual bool analyze(larlite::storage_manager* storage);

  /** IMPLEMENT in LineIntersectionVertexer.cc!
      Finalize method to be called after all events processed.
  */
  virtual bool finalize();


  larutil::Point2D IntersectionPoint(cluster::cluster_params & clust1,
                            cluster::cluster_params & clust2);


protected:

  bool isInPlane(larutil::Point2D &, unsigned int);

  cluster::DefaultParamsAlg _params_alg ;
  cluster::CRUHelper _cru_helper;

  size_t _min_hits;

  std::string _cluster_producer;

  std::vector<::argovertex::bound> boundaries;

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
