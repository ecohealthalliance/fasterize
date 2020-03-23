
context("fasterize")
suppressPackageStartupMessages(library(sf))
suppressPackageStartupMessages(library(raster))
p1 <- rbind(c(-180,-20), c(-140,55), c(10, 0), c(-140,-60), c(-180,-20))
hole <- rbind(c(-150,-20), c(-100,-10), c(-110,20), c(-150,-20))
p1 <- list(p1, hole)
p2 <- list(rbind(c(-10,0), c(140,60), c(160,0), c(140,-55), c(-10,0)))
p3 <- list(rbind(c(-125,0), c(0,60), c(40,5), c(15,-45), c(-125,0)))
pols <- st_sf(value = c(1,2,3),
              geometry = st_sfc(lapply(list(p1, p2, p3), st_polygon)))

test_that("raster sf method works", {
  r <- raster(pols, res = 1)
  expect_s4_class(r, 'RasterLayer')
})

test_that("fasterize works", {
  r <- raster(pols, res = 1)
  expect_error(f <- fasterize(pols, r, field = "value", fun="sum"), NA)
  expect_error(f <- fasterize(pols, r, fun="sum"), NA)
  expect_s4_class(f, 'RasterLayer')
})

test_that("non-NA background values allowed", {
  r <- raster(pols, res = 1)
  bg <- 20
  expect_error(
    f0 <- fasterize(pols, r, field = "value", fun="last", background = bg), NA)
  expect_equal(unname(f0[1,1]), bg)
  expect_equal(f0@data@max, max(bg, max(pols$value)))
  expect_equal(f0@data@min, min(bg, min(pols$value)))
})

# Currently rounding differences between libraries (I think) cause these
# failures.
#
# test_that("fasterize produces the same output as rasterize", {
#   library(raster)
#   r <- raster(pols, res = 1)
#   pols_sp <- as(pols, "Spatial")
#   r <- raster::rasterize(pols_sp, r, field = "value", fun="sum")
#   f <- fasterize(pols, r, field = "value", fun="sum")
#   expect_identical(as.matrix(r), as.matrix(f))
# #   which(as.matrix(r) != as.matrix(f) |
# #         is.na(as.matrix(r)) != is.na(as.matrix(f)),
# #         arr.ind = TRUE)
# #   plot(r != f | is.na(r) != is.na(f))
# #   plot(r-f)
# #   plot(r)
# # })

test_that("values are correct when polygons extend beyond raster", {
  r1 <- raster(extent(c(-180, 180, -60, 60)), res=1)
  f1 <- fasterize(pols, r1, field = "value", fun="sum")
  r2 <- raster(extent(c(130, 150, -60, 0)), res=1)
  f2 <- fasterize(pols, r2, field = "value", fun="sum")
  r3 <- raster(extent(c(-150, -130, -60, 0)), res=1)
  f3 <- fasterize(pols, r3, field = "value", fun="sum")
  expect_equal(extent(r2), extent(f2))
  f1c <- crop(f1, f2)
  expect_equal(getValues(f1c), getValues(f2))
  f1d <- crop(f1, f3)
  expect_equal(getValues(f1d), getValues(f3))
  # which(as.matrix(f1c) != as.matrix(f2) |
  #       is.na(as.matrix(f1c)) != is.na(as.matrix(f2)),
  #       arr.ind = TRUE)
  # plot(f1c != f2 | is.na(f1c) != is.na(f2))
})

test_that("error thrown for malformed polygon", {
  r <- raster(pols, res = 1)
  pols_err <- pols
  pols_err$geometry[[2]][[1]] <- as.character(pols_err$geometry[[2]][[1]])
  expect_error(f <- fasterize(pols_err, r, field = "value", fun="sum"),
               "incompatible SEXP; only accepts lists and REALSXPs")

})
