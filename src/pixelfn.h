#ifndef PIXELFN
#define PIXELFN

#include <RcppArmadillo.h>

//Declare a type for a pointer to a function to update a pixel
typedef void (*PixelFn)(arma::mat &raster, int x, int y, double &value);
void sumvalues(arma::mat &raster, int x, int y, double &value);
void firstvalues(arma::mat &raster, int x, int y, double &value);
void lastvalues(arma::mat &raster, int x, int y, double &value);
void minvalues(arma::mat &raster, int x, int y, double &value);
void maxvalues(arma::mat &raster, int x, int y, double &value);
void countvalues(arma::mat &raster, int x, int y, double &value);
void anyvalues(arma::mat &raster, int x, int y, double &value);
PixelFn set_pixelfn(std::string fstr);

#endif
