#include <RcppArmadillo.h>
// [[Rcpp::plugins(cpp11)]
// [[Rcpp::depends(RcppArmadillo)]]

//Declare a type for a pointer to a function to update a pixel.
typedef void (*PixelFn)(arma::mat &raster, arma::uword x, arma::uword y, double &value);

//Declare functions for each of the different pixel updating options that take
//the same arguments and so can of that type

//Add the value, or just set it if current value is NA
void sumvalues(arma::mat &raster, arma::uword x, arma::uword y, double &value) {
  raster.at(y, x) =
    //it's probably dangerous to use Rcpp::internal
    (Rcpp::internal::Rcpp_IsNA(raster.at(y, x)) |
      Rcpp::internal::Rcpp_IsNA(value)) ?
      value : (raster.at(y, x) + value);
}

//First value: only set the value if its currently NA
void firstvalues(arma::mat &raster, arma::uword x, arma::uword y, double &value) {
  if(Rcpp::internal::Rcpp_IsNA(raster.at(y, x))) {
    raster.at(y, x) = value;
  }
}

//Last value: Always set the value
void lastvalues(arma::mat &raster, arma::uword x, arma::uword y, double &value) {
  raster.at(y, x) = value;
}

//Only set the value if it is smaller than current
void minvalues(arma::mat &raster, arma::uword x, arma::uword y, double &value) {
  if(Rcpp::internal::Rcpp_IsNA(raster.at(y, x)) | (raster.at(y, x) > value)) {
    raster.at(y, x) = value;
  }
}

//Only set the value if it is larger than current
void maxvalues(arma::mat &raster, arma::uword x, arma::uword y, double &value) {
  if(Rcpp::internal::Rcpp_IsNA(raster.at(y, x)) | (raster.at(y, x) < value)) {
    raster.at(y, x) = value;
  }
}

//Add one to layer
void countvalues(arma::mat &raster, arma::uword x, arma::uword y, double &value) {
  if(Rcpp::internal::Rcpp_IsNA(raster.at(y, x))) {
    raster.at(y, x) = 1;
  } else {
    raster.at(y, x) += 1;
  }
}

//Just mark presence
void anyvalues(arma::mat &raster, arma::uword x, arma::uword y, double &value) {
    raster.at(y, x) = 1;
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
  } else if (fstr == "count") {
    pixel_fn = &countvalues;
  } else if (fstr == "any") {
    pixel_fn = &anyvalues;
  } else {
    std::stringstream err_msg;
    err_msg<< "'fun' has an invalid value: " << fstr <<
      ". See ?fasterize for allowed values" << std::endl;
    Rcpp::stop(err_msg.str());
  }
  return pixel_fn;
}


