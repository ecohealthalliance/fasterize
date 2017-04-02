#ifndef RASTERIZE_POLYGON
#define RASTERIZE_POLYGON

#define ARMA_64BIT_WORD  //required to support arma vectors > 2GB
#include <RcppArmadillo.h>
#include "edge.h"
#include "pixelfn.h"
// [[Rcpp::plugins(cpp11)]
// [[Rcpp::depends(RcppArmadillo)]]

void rasterize_polygon(arma::mat &raster,
                       Rcpp::RObject polygon,
                       double &value,
                       RasterInfo &ras,
                       PixelFn &pixel_function);
#endif
