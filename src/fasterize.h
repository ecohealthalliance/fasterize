#ifndef FASTERIZE
#define FASTERIZE

#include <RcppArmadillo.h>
// [[Rcpp::plugins(cpp11)]
// [[Rcpp::depends(RcppArmadillo)]]

extern Rcpp::S4 fasterize(Rcpp::DataFrame &sf,
                   SEXP &raster,
                   std::string field,
                   std::string fun,
                   double background,
                   std::string by);
#endif
