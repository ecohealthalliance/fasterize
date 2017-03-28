#ifndef RASTERIZE_POLYGON
#define RASTERIZE_POLYGON

#include <RcppArmadillo.h>
#include "edge.h"
#include "pixelfn.h"

void rasterize_polygon(arma::mat &raster,
                       SEXP polygon,
                       double &value,
                       RasterInfo &ras,
                       PixelFn &pixel_function);
#endif
