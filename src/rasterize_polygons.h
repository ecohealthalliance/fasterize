#ifndef RASTERIZE_POLYGONS
#define RASTERIZE_POLYGONS

#include <RcppArmadillo.h>

arma::mat rasterize_polygons(Rcpp::List &polygons,
                             Rcpp::NumericVector &field_values,
                             Rcpp::List raster_info,
                             double background);
#endif
