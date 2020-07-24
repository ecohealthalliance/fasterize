#ifndef RASTERIZE_POLYGON
#define RASTERIZE_POLYGON

#include <RcppArmadillo.h>
#include "edge.h"
#include "pixelfn.h"
// [[Rcpp::plugins(cpp11)]
// [[Rcpp::depends(RcppArmadillo)]]

extern void rasterize_polygon(arma::mat &raster,
                       Rcpp::RObject polygon,
                       double &value,
                       RasterInfo &ras,
                       PixelFn &pixel_function);
#endif
