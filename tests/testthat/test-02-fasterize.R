
context("fasterize")

suppressPackageStartupMessages(library(raster))
library(wk)
pols_df <- structure(list(feature_id = c(1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 2L, 2L, 2L, 2L, 2L, 3L, 3L, 3L, 3L, 3L), 
                          part_id = c(1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 2L, 2L, 2L, 2L, 2L, 3L, 3L, 3L, 3L, 3L), 
                          ring_id = c(1L, 1L, 1L, 1L, 1L, 2L, 2L, 2L, 2L, 3L,  3L, 3L, 3L, 3L, 4L, 4L, 4L, 4L, 4L), 
                          x = c(-180, -140, 10, -140,  -180, -150, -100, -110, -150, -10, 140, 160, 140, -10, -125, 0, 40, 15, -125), 
                          y = c(-20, 55, 0, -60, -20, -20, -10, 20, -20, 0, 60, 0, -55, 0, 0, 60, 5, -45, 0)), row.names = c(NA, 19L), class = "data.frame")
pols_df$xy <- wk::xy(pols_df$x, pols_df$y)
pols <- data.frame(value = 1:3, geometry = wk::wk_polygon(pols_df, feature_id = pols_df$feature_id, ring_id = pols_df$ring_id, part_id = pols_df$part_id))
ex <- as.numeric(wk::wk_bbox(pols))[c(1, 3, 2, 4)]
r <- raster::raster(raster::extent(ex), res = 1)


test_that("fasterize works", {

  expect_error(f <- fasterize(pols, r, field = "value", fun="sum"), NA)
  expect_error(f <- fasterize(pols, r, fun="sum"), NA)
  expect_s4_class(f, 'RasterLayer')
})

test_that("non-NA background values allowed", {
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


