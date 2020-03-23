#ifndef EDGELIST
#define EDGELIST

#include "edge.h"

extern void edgelist(Rcpp::RObject polygon, RasterInfo &ras, std::list<Edge> &edges);
#endif
