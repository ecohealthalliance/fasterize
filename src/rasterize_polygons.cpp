#include <RcppArmadillo.h>
#include "edge.h"
#include "edgelist.h"
#include "rasterize_polygon.h"
#include "rasterize_polygons.h"

// Rasterize set of polygons
// Based on https://ezekiel.encs.vancouver.wsu.edu/~cs442/lectures/rasterization/polyfill/polyfill.pdf #nolint
// [[Rcpp::plugins(cpp11)]
// [[Rcpp::depends(RcppArmadillo)]]
// [[Rcpp::export]]
arma::mat rasterize_polygons(Rcpp::List &polygons,
                             Rcpp::NumericVector &field_values,
                             Rcpp::List raster_info,
                             double background) {

  //Move this R list into a C++ struct
  RasterInfo ras(raster_info);

  //Create the empty raster to fill
  arma::mat raster_matrix(ras.nrow, ras.ncol);
  raster_matrix.fill(NA_REAL);

  //Rasterize each polygon
  Rcpp::List::iterator p = polygons.begin();
  Rcpp::NumericVector::iterator i = field_values.begin();

  for(; p != polygons.end(); ++p, ++i) {
    rasterize_polygon(raster_matrix, Rcpp::as<Rcpp::List>(*p)[0], (*i), ras);
  }

  //Fill in the empty cells
  if(!R_IsNA(background)) {
    raster_matrix.replace(NA_REAL, background);
  }
  return raster_matrix;

}
