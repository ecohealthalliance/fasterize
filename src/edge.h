#ifndef EDGE
#define EDGE

#include <RcppArmadillo.h>

// A small object to hold basic info about raster dimensions
struct RasterInfo {
  double xmin, ymax, xres, yres;
  int nrow, ncol;

  RasterInfo(Rcpp::List &raster_info) {
    xmin = Rcpp::as<double>(raster_info["xmin"]);
    ymax = Rcpp::as<double>(raster_info["ymax"]);
    xres = Rcpp::as<double>(raster_info["xres"]);
    yres = Rcpp::as<double>(raster_info["yres"]);
    nrow = Rcpp::as<int>(raster_info["nrow"]);
    ncol = Rcpp::as<int>(raster_info["ncol"]);
  }
};

// A data structure to hold only the neccessary information about a polygon
// edge needed to rasterize it
struct Edge {
  int ystart;  //the first matrix row intersected
  int yend;  //the matrix row below the end of the line
  double dxdy; //change in x per y
  double x; //the x location on the first matrix row intersected

  Edge(double x0, double y0, double x1, double y1, RasterInfo &ras,
       int y0c, int y1c) {
    //Convert from coordinate space to matrix row/column space. This is
    //already done for ys with y0c and y1c
    x0 = (x0 - ras.xmin)/ras.xres - 0.5; //convert from native to
    x1 = (x1 - ras.xmin)/ras.xres - 0.5; // units in the matrix

    //Make sure edges run from top of matrix to bottom, calculate value
    if(y1c > y0c) {
      ystart = y0c;
      dxdy = (x1-x0)/(y1-y0);
      x = x0 + (ystart - y0)*dxdy;
      yend = y1c;
    } else {
      ystart = y1c;
      dxdy = (x0-x1)/(y0-y1);
      x = x1 + (ystart - y1)*dxdy;
      yend = y0c;
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
