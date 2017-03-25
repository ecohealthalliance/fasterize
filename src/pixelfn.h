#ifndef PIXELFN
#define PIXELFN

#include <RcppArmadillo.h>

//Declare a type for a pointer to a function to update a pixel
typedef void (*PixelFn)(arma::mat &raster, int x, int y, double &value, int layer);
void sumvalues(arma::mat &raster, int x, int y, double &value, int layer);
void firstvalues(arma::mat &raster, int x, int y, double &value, int layer);
void lastvalues(arma::mat &raster, int x, int y, double &value, int layer);
PixelFn set_pixelfn(std::string fstr);

#endif
