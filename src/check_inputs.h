#ifndef CHECK_INPUTS
#define CHECK_INPUTS

#include <RcppArmadillo.h>
// [[Rcpp::plugins(cpp11)]
// [[Rcpp::depends(RcppArmadillo)]]

extern void check_inputs(Rcpp::DataFrame &sf,
                    Rcpp::S4 &raster,
                    Rcpp::Nullable<std::string> field,
                    std::string fun,
                    double background,
                    Rcpp::Nullable<std::string> by,
                    Rcpp::List &polygons,
                    Rcpp::NumericVector &field_vals);

#endif
