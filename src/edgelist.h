#ifndef EDGELIST
#define EDGELIST

#include "edge.h"

void edgelist(SEXP polygon, RasterInfo &ras, std::list<Edge> &edges);
//bool edgelist2(SEXP polygon, Rcpp::List raster_info);
#endif
