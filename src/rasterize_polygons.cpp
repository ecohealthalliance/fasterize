#include <RcppArmadillo.h>
#include "edge.h"
#include "pixelfn.h"
#include "edgelist.h"
#include "rasterize_polygon.h"
#include "rasterize_polygons.h"

// Rasterize set of polygons
// Based on https://ezekiel.encs.vancouver.wsu.edu/~cs442/lectures/rasterization/polyfill/polyfill.pdf #nolint
// [[Rcpp::plugins(cpp11)]
// [[Rcpp::depends(RcppArmadillo)]]
// [[Rcpp::export]]
Rcpp::S4 rasterize_polygons(Rcpp::S4 &raster,
                            Rcpp::List &polygons,
                            Rcpp::NumericVector &field_values,
                            std::string fun,
                            double background) {

  //Move this R list into a C++ struct
  RasterInfo ras(raster);
  Rcpp::S4 rasterdata = raster.slot("data");
  PixelFn pixel_function = set_pixelfn(fun);

  //Create the empty raster to fill
  arma::mat raster_matrix(ras.nrow, ras.ncol);
  raster_matrix.fill(NA_REAL);

  //Rasterize each polygon
  Rcpp::List::iterator p = polygons.begin();
  Rcpp::NumericVector::iterator i = field_values.begin();
  int layer = 0;

  for(; p != polygons.end(); ++p, ++i, ++layer) {
    rasterize_polygon(raster_matrix, Rcpp::as<Rcpp::List>(*p)[0], (*i), layer,
                      ras, pixel_function);
  }

  //Fill in the empty cells
  if(!R_IsNA(background)) {
    raster_matrix.replace(NA_REAL, background);
  }

  rasterdata.slot("values") = raster_matrix.t();
  rasterdata.slot("min") = raster_matrix.min();
  rasterdata.slot("max") = raster_matrix.max();
  rasterdata.slot("inmemory") = true;
  rasterdata.slot("fromdisk") = false;
  rasterdata.slot("haveminmax") = true;
  return rasterdata;
}
