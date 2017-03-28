#include <RcppArmadillo.h>

Rcpp::CharacterVector as_character(const SEXP vec) {
  if (Rcpp::as<Rcpp::RObject>(vec).inherits("factor")) {
    Rcpp::IntegerVector ints(vec);
    Rcpp::StringVector levels = ints.attr("levels");
    Rcpp::CharacterVector out(ints.size());
    for(int i = 0; i < ints.size(); i++) {
      out[i] =
        (ints[i] == NA_INTEGER) ? NA_STRING : levels[ints[i] - 1];
    }
    return out;
  } else {
    return Rf_coerceVector(vec, STRSXP);
  }
}

Rcpp::S4 brick(const Rcpp::S4 &raster) {
  Rcpp::Environment raster_pkg = Rcpp::Environment::namespace_env("raster");
  Rcpp::Function brick = raster_pkg["brick"];
  return brick(raster);
}
