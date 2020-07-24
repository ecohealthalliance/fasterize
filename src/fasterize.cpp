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

//' Rasterize an sf object of polygons
//'
//' Rasterize set of polygons
//'
//' This is a high-performance replacement for [raster::rasterize()].
//'
//' The algorithm is based on the method described in course materials provided
//' by [Wayne O. Cochran](https://labs.wsu.edu/wayne-cochran/). The algorithm
//' is originally attributed to
//' [Wylie et al. (1967)](https://dx.doi.org/10.1145/1465611.1465619).
//'
//' @param sf an [sf::sf()] object with a geometry column of POLYGON and/or
//' MULTIPOLYGON objects.
//' @param raster A raster  object. Used as a template for the raster output.
//' Can be created with [raster::raster()].
//' The fasterize package provides a method to create a raster object from
//' an sf object.
//' @param field character. The name of a column in `sf`,
//' providing a value for each of the polygons rasterized. If NULL (default),
//' all polygons will be given a value of 1.
//' @param fun character. The name of a function by which to combine overlapping
//' polygons. Currently takes "sum", "first", "last", "min", "max", "count", or
//' "any".  Future versions may include more functions or the ability to pass
//' custom R/C++ functions. If you need to summarize by a different function,
//' use `by=` to get a RasterBrick and then [raster::stackApply()] or
//' [raster::calc()] to summarize.
//' @param background numeric. Value to put in the cells that are not covered by
//' any of the features of x. Default is NA.
//' @param by character.  The name of a column in `sf` by which to aggregate
//' layers.  If set, fasterize will return a RasterBrick with as many layers
//' as unique values of the `by` column.
//' @return A raster of the same size, extent, resolution and projection as the
//' provided raster template.
//' @references Wylie, C., Romney, G., Evans, D., & Erdahl, A. (1967).
//'   Half-tone perspective drawings by computer. Proceedings of the November
//'   14-16, 1967, Fall Joint Computer Conference. AFIPS '67 (Fall).
//'   <https://dx.doi.org/10.1145/1465611.1465619>
//' @examples
//' library(sf)
//' library(fasterize)
//' p1 <- rbind(c(-180,-20), c(-140,55), c(10, 0), c(-140,-60), c(-180,-20))
//' hole <- rbind(c(-150,-20), c(-100,-10), c(-110,20), c(-150,-20))
//' p1 <- list(p1, hole)
//' p2 <- list(rbind(c(-10,0), c(140,60), c(160,0), c(140,-55), c(-10,0)))
//' p3 <- list(rbind(c(-125,0), c(0,60), c(40,5), c(15,-45), c(-125,0)))
//' pols <- st_sf(value = rep(1,3),
//'               geometry = st_sfc(lapply(list(p1, p2, p3), st_polygon)))
//' r <- raster(pols, res = 1)
//' r <- fasterize(pols, r, field = "value", fun="sum")
//' plot(r)
//' @export
// [[Rcpp::export]]
Rcpp::S4 fasterize(Rcpp::DataFrame &sf,
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
    rasterdata.slot("nlayers") = n_layers;
    rasterdata.slot("names") = layernames;
    //Update other raster slots
    rasterdata.slot("min") = raster_array.min();
    rasterdata.slot("max") = raster_array.max();
    rasterdata.slot("inmemory") = true;
    rasterdata.slot("fromdisk") = false;
    rasterdata.slot("haveminmax") = true;

    // new sf only stores ()$input and ()$wkt so we have no basis to grab
    // a PROJ.4 string from that, just assume they are the same
    // - this wrongly would *assign* the sf projection to the raster if it
    // was not NA before MDSumner 2020-03-02
    // Rcpp::CharacterVector sfproj4 =
    //   Rcpp::as<Rcpp::StringVector>(
    //     Rcpp::as<Rcpp::List>(polygons.attr("crs"))["proj4string"]
    //   );
    // if(sfproj4[0] != NA_STRING) {
    //   Rcpp::S4 rcrs(raster1.slot("crs"));
    //   rcrs.slot("projargs") = sfproj4;
    // }

    return raster1;

    //For the case of collapsing all to one layer
  } else {
    // Do this again because the raster object is different,
    Rcpp::S4 raster1 = Rcpp::clone(raster);
    n_layers = 1;
    Rcpp::S4 rasterdata(raster1.slot("data"));
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
    rasterdata.slot("min") = raster_array.min();
    rasterdata.slot("max") = raster_array.max();
    rasterdata.slot("inmemory") = true;
    rasterdata.slot("fromdisk") = false;
    rasterdata.slot("haveminmax") = true;
    rasterdata.slot("names") = "layer";

    // Rcpp::CharacterVector sfproj4 =
    //   Rcpp::as<Rcpp::StringVector>(
    //     Rcpp::as<Rcpp::List>(polygons.attr("crs"))["proj4string"]
    //   );
    // if(sfproj4[0] != NA_STRING) {
    //   Rcpp::S4 rcrs(raster1.slot("crs"));
    //   rcrs.slot("projargs") = sfproj4;
    // }

    return raster1;
  }
}




