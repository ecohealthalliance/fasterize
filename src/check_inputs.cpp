#include <RcppArmadillo.h>
// [[Rcpp::plugins(cpp11)]
// [[Rcpp::depends(RcppArmadillo)]]

void check_inputs(Rcpp::DataFrame &sf,
                    Rcpp::S4 &raster,
                    Rcpp::Nullable<std::string> field,
                    std::string fun,
                    double background,
                    Rcpp::Nullable<std::string> by,
                    Rcpp::List &polygons,
                    Rcpp::NumericVector &field_vals) {

  std::stringstream err_msg;

  if(!Rf_inherits(sf, "sf")) {
    err_msg << "sf must be of class sf." << std::endl;
  }

  polygons = sf[Rcpp::as<std::string>(sf.attr("sf_column"))];

  if(!(Rf_inherits(polygons, "sfc_MULTIPOLYGON") |
       Rf_inherits(polygons, "sfc_POLYGON"))) {
    err_msg << "sf geometry must be POLYGON or MULTIPOLYGON" << std::endl;
  }

  if(field.isNotNull()) {
    field_vals = sf[Rcpp::as<std::string>(field.get())];
  } else {
    field_vals = Rcpp::rep(1, polygons.size());
  }


  std::string err = err_msg.str();
  if(!err.empty()) {
    Rcpp::stop(err);
  }
}
