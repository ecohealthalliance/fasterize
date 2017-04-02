#ifndef PIXELFN
#define PIXELFN

#define ARMA_64BIT_WORD  //required to support arma vectors > 2GB
#include <RcppArmadillo.h>
// [[Rcpp::plugins(cpp11)]
// [[Rcpp::depends(RcppArmadillo)]]

//Declare a type for a pointer to a function to update a pixel
typedef void (*PixelFn)(arma::mat &raster, arma::uword x, arma::uword y, double &value);
void sumvalues(arma::mat &raster, arma::uword x, arma::uword y, double &value);
void firstvalues(arma::mat &raster, arma::uword x, arma::uword y, double &value);
void lastvalues(arma::mat &raster, arma::uword x, arma::uword y, double &value);
void minvalues(arma::mat &raster, arma::uword x, arma::uword y, double &value);
void maxvalues(arma::mat &raster, arma::uword x, arma::uword y, double &value);
void countvalues(arma::mat &raster, arma::uword x, arma::uword y, double &value);
void anyvalues(arma::mat &raster, arma::uword x, arma::uword y, double &value);
PixelFn set_pixelfn(std::string fstr);

#endif
