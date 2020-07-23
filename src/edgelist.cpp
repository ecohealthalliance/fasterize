#include <RcppArmadillo.h>
#include "edge.h"
#include "edgelist.h"
// [[Rcpp::plugins(cpp11)]
// [[Rcpp::depends(RcppArmadillo)]]

//  Builds an edge list from a polygon or multipolygon
void edgelist(Rcpp::RObject polygon, RasterInfo &ras, std::list<Edge> &edges) {
  //std::list<Edge> edges;
  double y0, y1, y0c, y1c;
  //iterate recursively over the list
  switch(polygon.sexp_type()) {
  case REALSXP: {
    //if the object is numeric, it an Nx2 matrix of polygon nodes.
    Rcpp::NumericMatrix poly(polygon);
    //Add edge to list if it's not horizontal and is in the raster
    for(int i = 0; i < (poly.nrow() - 1); ++i) {
      y0 = (ras.ymax - poly(i, 1))/ras.yres - 0.5;
      y1 = (ras.ymax - poly(i+1, 1))/ras.yres - 0.5;
      if(y0 > 0 || y1 > 0) {  //only both with edges that are in the raster
        y0c = std::ceil(y0);
        y1c = std::ceil(y1);
        if(y0c != y1c) {  //only bother with non-horizontal edges
          edges.push_back(Edge(poly(i    , 0), y0,
                               poly(i + 1, 0), y1, ras, y0c, y1c));
        }
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
      edgelist(Rcpp::wrap(*it), ras, edges);
    }
    break;
  }
  default: {
    Rcpp::stop("incompatible SEXP; only accepts lists and REALSXPs");
  }
  }
}
