#include <RcppArmadillo.h>

//Declare a type for a pointer to a function to update a pixel
typedef void (*PixelFn)(arma::mat &raster, int x, int y, double &value, int layer);

//Different strokes for different folks: A function for each fun option

void sumvalues(arma::mat &raster, int x, int y, double &value, int layer) {
  raster.at(y, x) =
    //it's probably dangerous to use Rcpp::internal
    Rcpp::internal::Rcpp_IsNA(raster.at(y, x)) ?
    value : (raster.at(y, x) + value);
}

//Only set the value if its currently NA
void firstvalues(arma::mat &raster, int x, int y, double &value, int layer) {
  if(Rcpp::internal::Rcpp_IsNA(raster.at(y, x))) {
    raster.at(y, x) = value;
  }
}

//Always set the value
void lastvalues(arma::mat &raster, int x, int y, double &value, int layer) {
  raster.at(y, x) = value;
}

PixelFn set_pixelfn(std::string fstr) {
  PixelFn pixel_fn;
  if (fstr == "sum") {
    pixel_fn = &sumvalues;
  } else if (fstr == "first") {
    pixel_fn = &firstvalues;
  } else if (fstr == "last") {
    pixel_fn = &lastvalues;
  } else {
    std::stringstream err_msg;
    err_msg<< "'fun' has an invalid value: " << fstr << std::endl;
    Rcpp::stop(err_msg.str());
  }
  return pixel_fn;
}


