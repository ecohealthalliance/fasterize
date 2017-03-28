#ifndef UTILS
#define UTILS

#include <RcppArmadillo.h>

Rcpp::CharacterVector as_character(const SEXP vec);
Rcpp::S4 brick(const Rcpp::S4 &raster);

#endif
