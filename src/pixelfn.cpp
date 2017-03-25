#include <RcppArmadillo.h>

//Declare a type for a pointer to a function to update a pixel.
typedef void (*PixelFn)(arma::mat &raster, int x, int y, double &value, int layer);

//Declare functions for each of the different pixel updating options that take
//the same arguments and so can of that type

//Add the value, or just set it current value is NA
void sumvalues(arma::mat &raster, int x, int y, double &value, int layer) {
  raster.at(y, x) =
    //it's probably dangerous to use Rcpp::internal
    (Rcpp::internal::Rcpp_IsNA(raster.at(y, x)) |
      Rcpp::internal::Rcpp_IsNA(value)) ?
      value : (raster.at(y, x) + value);
}

//First value: only set the value if its currently NA
void firstvalues(arma::mat &raster, int x, int y, double &value, int layer) {
  if(Rcpp::internal::Rcpp_IsNA(raster.at(y, x))) {
    raster.at(y, x) = value;
  }
}

//Last value: Always set the value
void lastvalues(arma::mat &raster, int x, int y, double &value, int layer) {
  raster.at(y, x) = value;
}

//Only set the value if it is smaller than current
void minvalues(arma::mat &raster, int x, int y, double &value, int layer) {
  if(Rcpp::internal::Rcpp_IsNA(raster.at(y, x)) | (raster.at(y, x) > value)) {
    raster.at(y, x) = value;
  }
}

//Only set the value if it is larger than current
void maxvalues(arma::mat &raster, int x, int y, double &value, int layer) {
  if(Rcpp::internal::Rcpp_IsNA(raster.at(y, x)) | (raster.at(y, x) < value)) {
    raster.at(y, x) = value;
  }
}

//Set an empty PixelFn function pointer to one of those functions based on
//a string
PixelFn set_pixelfn(std::string fstr) {
  PixelFn pixel_fn;
  if (fstr == "sum") {
    pixel_fn = &sumvalues;
  } else if (fstr == "first") {
    pixel_fn = &firstvalues;
  } else if (fstr == "last") {
    pixel_fn = &lastvalues;
  } else if (fstr == "min") {
    pixel_fn = &minvalues;
  } else if (fstr == "max") {
    pixel_fn = &maxvalues;
  } else {
    std::stringstream err_msg;
    err_msg<< "'fun' has an invalid value: " << fstr <<
      ". See ?fasterize for allowed values" << std::endl;
    Rcpp::stop(err_msg.str());
  }
  return pixel_fn;
}


