#ifndef RASTERIZE_POLYGONS
#define RASTERIZE_POLYGONS

#include <RcppArmadillo.h>

Rcpp::S4 rasterize_polygons(Rcpp::S4 &raster,
                            Rcpp::List &polygons,
                            Rcpp::NumericVector &field_values,
                            Rcpp::List raster_info,
                            double background);
#endif
