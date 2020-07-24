#ifndef UTILS
#define UTILS

#include <RcppArmadillo.h>
// [[Rcpp::plugins(cpp11)]
// [[Rcpp::depends(RcppArmadillo)]]

extern Rcpp::CharacterVector as_character(const Rcpp::RObject vec);
extern Rcpp::S4 brick(const Rcpp::S4 &raster);

#endif
