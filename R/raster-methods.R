setOldClass("sf")

#' Create a raster from an sf object
#'
#' See [raster::raster()] for more details.
#'
#' @importFrom methods setMethod
#' @importMethodsFrom raster raster
#' @param x an sf object
#' @param origin the origin point of the output raster
#' @param ... Additional arguments, see [raster::raster()] for more details.
#' @aliases raster
#' @importFrom sp CRS
#' @importFrom raster origin extend crop extent
#' @export
setMethod('raster', signature(x='sf'),
          function(x, origin, ...){
            bb <- attr(x[[attr(x, "sf_column")]], "bbox")[c("xmin", "xmax", "ymin", "ymax")]
            ext <- raster::extent(bb)
            r <- raster::raster(ext, ...)

            r@crs <- CRS(attr(x[[attr(x, "sf_column")]], "crs")$proj4string)
            if (!missing(origin)) {
              raster::origin(r) <- origin
              r <- raster::extend(r, 1)
              r <- raster::crop(r, ext, snap='out')
            }
            r
          }
)


#' raster::plot
#'
#' re-exported `plot` method from `raster` package to avoid failing
#' when trying to plot raster objects without having `raster` loaded
#' first.
#'
#' @name plot
#' @rdname plot
#' @keywords internal
#' @export
#' @importFrom raster plot
NULL
