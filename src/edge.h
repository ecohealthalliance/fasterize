#ifndef EDGE
#define EDGE

#include <RcppArmadillo.h>
// [[Rcpp::plugins(cpp11)]
// [[Rcpp::depends(RcppArmadillo)]]

// A small object to hold basic info about raster dimensions
struct RasterInfo {
  double xmin, xmax, ymin, ymax, xres, yres, ncold;
  arma::uword nrow, ncol;

  RasterInfo(SEXP &raster) {
    if (Rf_inherits(raster, "BasicRaster")) {
    Rcpp::S4 extent = raster->slot("extent");
    xmin = extent.slot("xmin");
    xmax = extent.slot("xmax");
    ymin = extent.slot("ymin");
    ymax = extent.slot("ymax");
    nrow = raster.slot("nrows");
    ncol = raster.slot("ncols");
    } else {
      xmin = REAL(raster)[0];
      xmax = REAL(raster)[0];
      ymin = REAL(raster)[0];
      ymax = REAL(raster)[0];
      nrow = (arma::uword)REAL(raster)[0];
      ncol = (arma::uword)REAL(raster)[0];
      
    }
    ncold = static_cast<double>(ncol);

    if(raster.slot("rotated")) {
      Rcpp::stop("No current support for rotated rasters.");
      // Rcpp::S4 rotation = raster.slot("rotation");
      // Rcpp::NumericVector geotrans = rotation.slot("geotrans");
      // xres = geotrans[2];
      // yres = geotrans[4];
    } else {
    xres = (xmax - xmin)/ncold;
    yres = (ymax - ymin)/static_cast<double>(nrow);
    }
  }
};

// A data structure to hold only the neccessary information about a polygon
// edge needed to rasterize it
struct Edge {
  arma::uword ystart;  //the first matrix row intersected
  arma::uword yend;  //the matrix row below the end of the line
  long double dxdy; //change in x per y. Long helps with some rounding errors
  long double x; //the x location on the first matrix row intersected

  Edge(double x0, double y0, double x1, double y1, RasterInfo &ras,
       double y0c, double y1c) {
    //Convert from coordinate space to matrix row/column space. This is
    //already done for ys with y0c and y1c
    x0 = (x0 - ras.xmin)/ras.xres - 0.5; //convert from native to
    x1 = (x1 - ras.xmin)/ras.xres - 0.5; // units in the matrix

    //Make sure edges run from top of matrix to bottom, calculate value
    if(y1c > y0c) {
      ystart = static_cast<arma::uword>(std::max(y0c, 0.0));
      dxdy = (x1-x0)/(y1-y0);
      x = x0 + (static_cast<double>(ystart) - y0)*dxdy;
      yend = static_cast<arma::uword>(y1c);
    } else {
      ystart = static_cast<arma::uword>(std::max(y1c, 0.0));
      dxdy = (x0-x1)/(y0-y1);
      x = x1 + (static_cast<double>(ystart) - y1)*dxdy;
      yend = static_cast<arma::uword>(y0c);
    }
  }
};

// These two structs allow us to compare and sort edges by their coordinates.
struct less_by_ystart {
  inline bool operator() (const Edge& struct1, const Edge& struct2) {
    return (struct1.ystart < struct2.ystart); // ||
            //((struct1.ystart == struct2.ystart) && (struct1.x < struct2.x)));
  }
};

struct less_by_x {
  inline bool operator() (const Edge& struct1, const Edge& struct2) {
    return (struct1.x < struct2.x);
  }
};

#endif
