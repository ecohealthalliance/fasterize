
context("input checks")
suppressPackageStartupMessages(library(sf))
library(geos)
p1 <- rbind(c(-180,-20), c(-140,55), c(10, 0), c(-140,-60), c(-180,-20))
hole <- rbind(c(-150,-20), c(-100,-10), c(-110,20), c(-150,-20))
p1 <- list(p1, hole)
p2 <- list(rbind(c(-10,0), c(140,60), c(160,0), c(140,-55), c(-10,0)))
p3 <- list(rbind(c(-125,0), c(0,60), c(40,5), c(15,-45), c(-125,0)))
pols <- st_sf(value = c(1,2,3),
              geometry = st_sfc(lapply(list(p1, p2, p3), st_polygon)))
r1 <- raster(pols, res=1)

## we now accept any wk-handled class
# test_that("fasterize needs class sf", {
#   pols_err <- pols
#   class(pols_err) <- "data.frame"
#   expect_error(fasterize(pols_err, r1), "sf must be of class sf")
# })

test_that("fasterize likes wkt/wkb/geos", {
  expect_s4_class(fasterize(wk::as_wkt(pols$geometry), r1), "BasicRaster")
  expect_s4_class(fasterize(wk::as_wkb(pols$geometry), r1), "BasicRaster")
  expect_s4_class(fasterize(geos::as_geos_geometry(pols$geometry), r1), "BasicRaster")
i <- seq_along(pols$geometry)
  expect_s4_class(fasterize(data.frame(a = i, g = wk::as_wkt(pols$geometry)), r1), "BasicRaster")
  expect_s4_class(fasterize(data.frame(a = i, wk::as_wkb(pols$geometry)), r1), "BasicRaster")
  expect_s4_class(fasterize(data.frame(a = i, geos::as_geos_geometry(pols$geometry)), r1), "BasicRaster")

    
})
test_that("fasterize needs polygons", {
  lines <- st_sf(value = c(1,2,3),
                 geometry = st_sfc(lapply(list(p1, p2, p3),
                                          function(x) st_linestring(x[[1]]))))
  expect_error(fasterize(lines, r1),
               "no polygon geometries to fasterize")
  
    lines_wkb <- data.frame(value = c(1,2,3),
                 geometry = wk::as_wkb(sf::st_cast(pols$geometry, "MULTILINESTRING")))

      expect_error(fasterize(lines_wkb, r1),
               "no polygon geometries to fasterize")
      
      

})

test_that("field value name is in sf object", {
  expect_error(fasterize(pols, r1, field="hello"), class="Rcpp::index_out_of_bounds")
})

test_that("rotated rasters not supported", {
  r1_err <- r1
  r1_err@rotated <- TRUE
  expect_error(fasterize(pols, r1_err),
               "No current support for rotated rasters.")
})
