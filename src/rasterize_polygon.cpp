#include <RcppArmadillo.h>
#include "edge.h"
#include "edgelist.h"
#include "pixelfn.h"
#include "rasterize_polygon.h"

// Rasterize a single polygon
// Based on https://ezekiel.encs.vancouver.wsu.edu/~cs442/lectures/rasterization/polyfill/polyfill.pdf #nolint
// [[Rcpp::plugins(cpp11)]
// [[Rcpp::depends(RcppArmadillo)]]
void rasterize_polygon(arma::mat &raster, SEXP polygon, double &poly_value,
                       RasterInfo &ras, PixelFn &pixel_function) {

  std::list<Edge>::iterator it;
  int counter, xstart, xend, xpix;

  //Create the list of all edges of the polygon, fill and sort it
  std::list<Edge> edges;
  edgelist(polygon, ras, edges);
  edges.sort(less_by_ystart());

  // Initialize an empty list of "active" edges
  std::list<Edge> active_edges;

  //Start at the top of the first edge
  int yline = std::max(0, edges.front().ystart);
  //drop any edges out of bounds
  while(edges.front().yend < yline) {
    edges.pop_front();
  }

  //Main loop
  while((edges.size() > 0 | active_edges.size() > 0) &
        (yline < raster.n_rows)) {
    // Transfer any edges starting on this row from edges to active edges
    while(edges.size() && (edges.front().ystart <= yline)) {
      active_edges.splice(active_edges.end(), edges, edges.begin());
    }
    //Sort active edges by x position of their intersection with the row
    active_edges.sort(less_by_x());

    //Iterate over active edges, fill between odd and even edges.
    counter = 0;
    for(it = active_edges.begin();
        it != active_edges.end();
        it++) {
      counter++;
      if (counter % 2) {
        xstart = ceil((*it).x);
      } else {
        xend = ceil((*it).x);
        for(xpix = xstart; xpix < xend; ++xpix) {
          //note x/y switched here as raster objects store values this way
          pixel_function(raster, yline, xpix, poly_value);
        }
      }
    }
    //Advance the horizontal row
    yline++;

    //Drop edges above the horizontal line, update the x-positions of the
    //intercepts of edges for the next row
    it = active_edges.begin();
    while(it != active_edges.end()) {
      if((*it).yend <= yline) {
        it = active_edges.erase(it);
      } else {
        (*it).x += (*it).dxdy;
        it++;
      }
    }
  }
}


//Export this for debugging
// arma::mat rasterize_polygon2(arma::mat raster, SEXP polygon,
//                              double &poly_value, Rcpp::List raster_info) {
//   RasterInfo ras(raster_info);
//   arma::mat raster_matrix = arma::mat(ras.nrow, ras.ncol).fill(NA_REAL);
//   rasterize_polygon(raster_matrix, polygon, poly_value, ras);
//   return raster_matrix;
// }
