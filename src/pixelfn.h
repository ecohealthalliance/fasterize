#ifndef PIXELFN
#define PIXELFN

#include <RcppArmadillo.h>
// [[Rcpp::plugins(cpp11)]
// [[Rcpp::depends(RcppArmadillo)]]

//Declare a type for a pointer to a function to update a pixel
typedef void (*PixelFn)(arma::mat &raster, arma::uword x, arma::uword y, double &value);
extern void sumvalues(arma::mat &raster, arma::uword x, arma::uword y, double &value);
extern void firstvalues(arma::mat &raster, arma::uword x, arma::uword y, double &value);
extern void lastvalues(arma::mat &raster, arma::uword x, arma::uword y, double &value);
extern void minvalues(arma::mat &raster, arma::uword x, arma::uword y, double &value);
extern void maxvalues(arma::mat &raster, arma::uword x, arma::uword y, double &value);
extern void countvalues(arma::mat &raster, arma::uword x, arma::uword y, double &value);
extern void anyvalues(arma::mat &raster, arma::uword x, arma::uword y, double &value);
extern PixelFn set_pixelfn(std::string fstr);

#endif
