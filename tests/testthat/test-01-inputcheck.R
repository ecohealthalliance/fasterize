
context("input checks")

library(geos)
# p1 <- rbind(c(-180,-20), c(-140,55), c(10, 0), c(-140,-60), c(-180,-20))
# hole <- rbind(c(-150,-20), c(-100,-10), c(-110,20), c(-150,-20))
# p1 <- list(p1, hole)
# p2 <- list(rbind(c(-10,0), c(140,60), c(160,0), c(140,-55), c(-10,0)))
# p3 <- list(rbind(c(-125,0), c(0,60), c(40,5), c(15,-45), c(-125,0)))
# pols <- st_sf(value = c(1,2,3),
#               geometry = st_sfc(lapply(list(p1, p2, p3), st_polygon)))

##wk::wk_coords(pols) ## ...
pols_df <- structure(list(feature_id = c(1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 2L, 2L, 2L, 2L, 2L, 3L, 3L, 3L, 3L, 3L), 
                          part_id = c(1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 1L, 2L, 2L, 2L, 2L, 2L, 3L, 3L, 3L, 3L, 3L), 
                          ring_id = c(1L, 1L, 1L, 1L, 1L, 2L, 2L, 2L, 2L, 3L,  3L, 3L, 3L, 3L, 4L, 4L, 4L, 4L, 4L), 
                          x = c(-180, -140, 10, -140,  -180, -150, -100, -110, -150, -10, 140, 160, 140, -10, -125, 0, 40, 15, -125), 
                          y = c(-20, 55, 0, -60, -20, -20, -10, 20, -20, 0, 60, 0, -55, 0, 0, 60, 5, -45, 0)), row.names = c(NA, 19L), class = "data.frame")
pols_df$xy <- wk::xy(pols_df$x, pols_df$y)
pols <- wk::wk_polygon(pols_df, feature_id = pols_df$feature_id, ring_id = pols_df$ring_id, part_id = pols_df$part_id)
#plot(pols)
ex <- as.numeric(wk::wk_bbox(pols_df))[c(1, 3, 2, 4)]
r1 <- raster::raster(raster::extent(ex), res = 1)
lines <- wk::wk_linestring(pols)
## we now accept any wk-handled class
# test_that("fasterize needs class sf", {
#   pols_err <- pols
#   class(pols_err) <- "data.frame"
#   expect_error(fasterize(pols_err, r1), "sf must be of class sf")
# })

test_that("fasterize likes wkt/wkb/geos", {
  expect_s4_class(fasterize(wk::as_wkt(pols), r1), "BasicRaster")
  expect_s4_class(fasterize(wk::as_wkb(pols), r1), "BasicRaster")
  expect_s4_class(fasterize(geos::as_geos_geometry(pols), r1), "BasicRaster")
i <- seq_along(pols)
  expect_s4_class(fasterize(data.frame(a = i, g = wk::as_wkt(pols)), r1), "BasicRaster")
  expect_s4_class(fasterize(data.frame(a = i, wk::as_wkb(pols)), r1), "BasicRaster")
  expect_s4_class(fasterize(data.frame(a = i, geos::as_geos_geometry(pols)), r1), "BasicRaster")

    
})
test_that("fasterize needs polygons", {

  expect_error(fasterize(lines, r1),
               "no polygon geometries to fasterize")
  
    lines_wkb <- data.frame(value = c(1),
                 geometry = wk::as_wkb(lines))

      expect_error(fasterize(lines_wkb, r1),
               "no polygon geometries to fasterize")
      
      

})

test_that("field value name is in handleable object", {
  expect_error(fasterize(pols, r1, field="hello"))
})

test_that("rotated rasters not supported", {
  r1_err <- r1
  r1_err@rotated <- TRUE
  expect_error(fasterize(pols, r1_err),
               "No current support for rotated rasters.")
})


vals <- 1:3
funs <- c("sum", "first", "last", "min", "max", "count", "any")
outval <- c(sum(vals), vals[1], vals[3], min(vals), max(vals),
            length(vals), any(as.logical(vals)))

pols <- data.frame(value = vals, g = pols)
for (i in seq_along(funs)) {
  
  test_that(paste(funs[1], "function works"), {
    rastout <- fasterize(pols, r1, field = "value", fun = funs[i])
    expect_equal(unname(rastout[60,172]), outval[i])
  })
  
}

test_that("disallowed aggregation function is rejected", {
  invalid_fn_name <- "yo"
  expect_error(
    fasterize(pols, r1, field = "value", fun =  invalid_fn_name),
    paste0("'fun' has an invalid value: ", invalid_fn_name)
  )
})

pols$by_1 = c("a", "a", "b")
pols$by_2 = c(1, 1, 2)
pols$by_3 = factor(c("a", "a", "b"))



test_that("'by' argument works", {
  expect_error(
    rb <-fasterize(pols, r1, field="value", fun="sum", by ="by_1"), NA)
  expect_equal(names(rb), unique(pols$by_1))
  expect_equal(ncol(rb@data@values), length(unique(pols$by_1)))
})

test_that("'by' layers are equivalent to layers generated separately", {
  rba <- fasterize(pols, r1, field="value", fun="sum", by ="value")
  for(i in 1:nrow(pols)) {
    expect_equal(raster::as.raster(rba[[i]]),
                 raster::as.raster(fasterize(pols[i,], r1, field="value", fun="sum")))
  }
})

test_that("'by' can handle non-character fields", {
  expect_error(
    rb_n <- fasterize(pols, r1, field="value", fun="sum", by ="by_2"), NA)
  expect_error(
    rb_f <- fasterize(pols, r1, field="value", fun="sum", by ="by_3"), NA)
  expect_equal(rb_n@data@names, unique(as.character(pols$by_2)))
  expect_equal(names(rb_f), unique(as.character(pols$by_3)))
})

test_that("non-NA background values allowed with by", {
  r <- r1
  bg <- 20
  expect_error(
    f0 <- fasterize(pols, r, field = "value", fun="last", background = bg,
                    by = "by_1"), NA)
  expect_equal(unname(f0[[1]][1,1]), bg)
  expect_equal(f0@data@max, max(bg, max(pols$value)))
  expect_equal(f0@data@min, min(bg, min(pols$value)))
})



