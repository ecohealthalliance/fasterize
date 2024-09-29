#include <RcppArmadillo.h>
#include "edge.h"
#include "check_inputs.h"
#include "pixelfn.h"
#include "edgelist.h"
#include "rasterize_polygon.h"
#include "fasterize.h"
#include "utils.h"
// [[Rcpp::plugins(cpp11)]
// [[Rcpp::depends(RcppArmadillo)]]

// [[Rcpp::export]]
Rcpp::S4 fasterize_cpp(Rcpp::DataFrame &sf,
                   Rcpp::S4 &raster,
                   Rcpp::Nullable<std::string> field = R_NilValue,
                   std::string fun = "last",
                   double background = NA_REAL,
                   Rcpp::Nullable<std::string> by = R_NilValue) {

  Rcpp::List polygons;
  Rcpp::NumericVector field_vals;
  check_inputs(sf, raster, field, fun, background, by,
               polygons, field_vals);  // Also fills in polygons and field_vals

  //set up things we'll use later
  Rcpp::List::iterator p;
  Rcpp::NumericVector::iterator f;

  RasterInfo ras(raster);
  PixelFn pixel_function = set_pixelfn(fun);
  arma::uword n_layers;
  //  If there is a `by` argument, we calculate unique values and set up
  //  a RasterBrick and a multi-layered output
  if (by.isNotNull()) {

    //An elaborate way to extract the 'by' column as vector<string>
    std::vector<std::string> by_vals =
      Rcpp::as<std::vector<std::string>>(
        as_character(sf[Rcpp::as<std::string>(by)])
      );
    //Loop over the by column, create a map of unique vals/layer indices
    std::unordered_map<std::string, arma::uword> by_map;
    arma::uword x = 0;
    std::vector< std::string >::iterator b;

    for (b = by_vals.begin(); b != by_vals.end(); ++b) {
      if (by_map.emplace(std::make_pair(*b, x)).second) {
        ++x;
      }
    }
    //Set our output values to be appropriate for multiple layers
    n_layers = by_map.size();
    Rcpp::S4 raster1 = brick(raster);

    //Create the output values in the raster slot but share memory with
    //Armadillo array for operations
    Rcpp::S4 rasterdata(raster1.slot("data"));
    Rcpp::S4 rasterfile(raster1.slot("file"));
    rasterdata.slot("values") =
      Rcpp::NumericMatrix(ras.nrow * ras.ncol, n_layers);
    arma::cube raster_array(
        Rcpp::NumericVector(rasterdata.slot("values")).begin(),
        ras.ncol, ras.nrow, n_layers, false, true);
    raster_array.fill(NA_REAL);

    //Do our rasterizing on each shape, assigning output to its unique
    //value layer
    p = polygons.begin();
    f = field_vals.begin();
    b = by_vals.begin();
    for(; p != polygons.end(); ++p, ++f, ++b) {
      rasterize_polygon(raster_array.slice(by_map[(*b)]), (*p), (*f),
                        ras, pixel_function);
    }
    //Update brick-specific raster slots
    Rcpp::StringVector layernames(n_layers);
    for(
      std::unordered_map<std::string, arma::uword>::iterator m = by_map.begin();
      m != by_map.end(); ++m
    ) {
      layernames[(*m).second] = (*m).first;
    }
    if(!R_IsNA(background)) {
      raster_array.replace(NA_REAL, background);
    }

    //Fill in the empty cells
    rasterfile.slot("name") = "";
    rasterdata.slot("nlayers") = n_layers;
    rasterdata.slot("names") = layernames;
    //Update other raster slots
    rasterdata.slot("min") = raster_array.min();
    rasterdata.slot("max") = raster_array.max();
    rasterdata.slot("inmemory") = true;
    rasterdata.slot("fromdisk") = false;
    rasterdata.slot("haveminmax") = true;


    return raster1;

    //For the case of collapsing all to one layer
  } else {
    // Do this again because the raster object is different,
    Rcpp::S4 raster1 = Rcpp::clone(raster);
    n_layers = 1;
    Rcpp::S4 rasterdata(raster1.slot("data"));
    Rcpp::S4 rasterfile(raster1.slot("file"));
    rasterdata.slot("values") =
      Rcpp::NumericVector(ras.nrow * ras.ncol * n_layers);
    arma::cube raster_array(
        Rcpp::NumericVector(rasterdata.slot("values")).begin(),
        ras.ncol, ras.nrow, n_layers, false, true);
    raster_array.fill(NA_REAL);

    //Rasterize but always assign to the one layer
    p = polygons.begin();
    f = field_vals.begin();
    for(; p != polygons.end(); ++p, ++f) {
      rasterize_polygon(raster_array.slice(0), (*p), (*f),
                        ras, pixel_function);
    }

    //Fill in the empty cells
    if(!R_IsNA(background)) {
      raster_array.replace(NA_REAL, background);
    }

    //Update other raster slots
    rasterfile.slot("name") = "";
    rasterdata.slot("min") = raster_array.min();
    rasterdata.slot("max") = raster_array.max();
    rasterdata.slot("inmemory") = true;
    rasterdata.slot("fromdisk") = false;
    rasterdata.slot("haveminmax") = true;
    rasterdata.slot("names") = "layer";


    return raster1;
  }
}




