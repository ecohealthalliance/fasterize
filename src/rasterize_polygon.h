#ifndef RASTERIZE_POLYGON
#define RASTERIZE_POLYGON

#include <RcppArmadillo.h>
#include "edge.h"

void rasterize_polygon(arma::mat &raster,
                       SEXP polygon,
                       double &value,
                        RasterInfo &ras);
#endif
