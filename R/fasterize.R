#' Rasterize an sf object of polygons
#'
#' This is a high-performance replacement for [raster::rasterize()].
#'
#' @param polygons an [sf::sf()] object with a geometry column of POLYGON and/or
#' MULTIPOLYGON objects.
#' @param raster A raster template object. Can be created with [raster::raster()].
#' The fasterize package provides a method to create a raster object from
#' an sf object.
#' @param field A numeric value or vector or the name of a column in `polygons`,
#' providing a value for each of the polygons rasterized.
#' @param fun The name of a function by which to combine overlapping polygons.
#' Currently only takes "sum", but future versions will provide other
#' high-performance compiled functions as well as the ability to pass arbitrary
#' R functions
#' @param crop logical. If TRUE, the raster is cropped to the extent of the
#' polygon object provided.
#' @param background numeric. Value to put in the cells that are not covered by any of the features of x. Default is NA
#'
#' @return A raster of the same size, extent, resolution and projection as the
#' provided raster template.
#' @examples
#' library(sf)
#' library(fasterize)
#' p1 <- rbind(c(-180,-20), c(-140,55), c(10, 0), c(-140,-60), c(-180,-20))
#' hole <- rbind(c(-150,-20), c(-100,-10), c(-110,20), c(-150,-20))
#' p1 <- list(p1, hole)
#' p2 <- list(rbind(c(-10,0), c(140,60), c(160,0), c(140,-55), c(-10,0)))
#' p3 <- list(rbind(c(-125,0), c(0,60), c(40,5), c(15,-45), c(-125,0)))
#' pols <- st_sf(value = rep(1,3),
#'               geometry = st_sfc(lapply(list(p1, p2, p3), st_polygon)))
#' r <- raster(pols, res = 1)
#' r <- fasterize(pols, r, field = "value", fun="sum")
#' plot(r)
#' @importFrom raster xmin xmax ymin ymax res nrow ncol crs setValues
#' @importFrom sf st_as_sf st_crs st_sfc st_polygon st_intersection st_geometry
#' @export
fasterize <- function(polygons, raster, field=NULL, fun ="sum",
                      crop=FALSE, background=NA) {

  polygons <- st_as_sf(polygons)
  check_inputs(polygons, raster, field, fun)

  #Set the raster projection to the polygon projection
  if (!is.na(st_crs(polygons))) {
       raster::crs(raster) <- sp::CRS(st_crs(polygons)$proj4string)
  }

  raster_info <- list(
    xmin = xmin(raster),
    xmax = xmax(raster),
    ymin = ymin(raster),
    ymax = ymax(raster),
    xres = res(raster)[1],
    yres = res(raster)[2],
    nrow = nrow(raster),
    ncol = ncol(raster)
  )


  # Crop polygons to the extent of the raster. TODO: profile - seems klunky.
  if(crop) {
    extent_poly <- with(raster_info, {
      st_sfc(
        st_polygon(x = list(matrix(c(xmin, ymin,
                                     xmax, ymin,
                                     xmax, ymax,
                                     xmin, ymax,
                                     xmin, ymin), ncol=2, byrow = TRUE))),
        crs <- st_crs(polygons))

    })
    polygons <- suppressWarnings(st_intersection(polygons, extent_poly))
  }

  #Calculate essential values to pass to the C functions
  geometry <- st_geometry(polygons)
  n_polygons <- length(geometry)
  field_values <- get_fields(polygons, field, n_polygons)


  #Do the thing! Fast!
  raster_matrix <- rasterize_polygons(geometry,
                                      field_values,
                                      raster_info,
                                      background)

  setValues(raster, raster_matrix)
}

get_fields <- function(polygons, field, n_polygons) {

  if (is.numeric(field)) {
    field_data <- rep(field, length.out = n_polygons)
  } else if(is.character(field)) {
    field_data <- polygons[[field]]
    if(is.null(field_data)) {
      stop("Field name not found in provided sf data.")
    }
  }
  return(field_data)
}

#' @importFrom sf st_is
check_inputs <- function(polygons, raster, field, fun) {

  if(!("sf" %in% class(polygons))) {
    stop("fasterize requires sf inputs for polygons")
  }

  if(!all(st_is(polygons, c("POLYGON", "MULTIPOLYGON")))) {
    stop("fasterize currently only works on POLYGON and MULTIPOLYGON types")
  }

  if(fun != "sum") {
    stop("Only summing values is currently implemented")
  }
}
