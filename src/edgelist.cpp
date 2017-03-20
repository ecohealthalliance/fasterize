#include <RcppArmadillo.h>
#include "edge.h"
#include "edgelist.h"

//  Builds an edge list from a polygon or multipolygon
// [[Rcpp::plugins(cpp11)]
// [[Rcpp::depends(RcppArmadillo)]]
void edgelist(SEXP polygon, RasterInfo &ras, std::list<Edge> &edges) {
  //std::list<Edge> edges;
  double y0, y1;
  int y0c, y1c;
  //iterate recursively over the list
  switch( TYPEOF(polygon) ) {
  case REALSXP: {
    //if the object is numeric, it an Nx2 of polygon nodes.
    arma::mat poly = Rcpp::as<arma::mat>(polygon);
    //Add edge to list if it's not horizontal
    for(int i = 0; i < (poly.n_rows - 1); ++i) {
      y0 = (ras.ymax - poly(i, 1))/ras.yres - 0.5;
      y1 = (ras.ymax - poly(i+1, 1))/ras.yres - 0.5;
      y0c = std::ceil(y0);
      y1c = std::ceil(y1);
      if(y0c != y1c) {
        edges.push_back(Edge(poly(i    , 0), y0,
                             poly(i + 1, 0), y1, ras, y0c, y1c));
      }
    }
    break;
  };
  case VECSXP: {
    //if the object is a list, recurse over that
    Rcpp::List polylist = Rcpp::as<Rcpp::List>(polygon);
    for(Rcpp::List::iterator it = polylist.begin();
        it != polylist.end();
          ++it) {
      //std::list<Edge> tmp = edgelist(Rcpp::wrap(*it), ras);
      edgelist(Rcpp::wrap(*it), ras, edges);
      //edges.insert(edges.end(), tmp.begin(), tmp.end());
    }
    break;
  }
  default: {
    Rcpp::stop("incompatible SEXP; only accepts lists and REALSXPs");
  }
  }
}

//Export this function for debugging:
// bool edgelist2(SEXP polygon, Rcpp::List raster_info) {
// RasterInfo ras(raster_info);
//   std::list<Edge> edges = edgelist(polygon, ras);
//   return true;
//     }
