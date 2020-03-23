#ifndef FASTERIZE
#define FASTERIZE

#define ARMA_64BIT_WORD  //required to support arma vectors > 2GB
#include <RcppArmadillo.h>
// [[Rcpp::plugins(cpp11)]
// [[Rcpp::depends(RcppArmadillo)]]

extern Rcpp::S4 fasterize(Rcpp::DataFrame &sf,
                   Rcpp::S4 &raster,
                   std::string field,
                   std::string fun,
                   double background,
                   std::string by);
#endif
