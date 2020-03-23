
context("group-by operations")
suppressPackageStartupMessages(library(sf))
suppressPackageStartupMessages(library(raster))
p1 <- rbind(c(-180,-20), c(-140,55), c(10, 0), c(-140,-60), c(-180,-20))
hole <- rbind(c(-150,-20), c(-100,-10), c(-110,20), c(-150,-20))
p1 <- list(p1, hole)
p2 <- list(rbind(c(-10,0), c(140,60), c(160,0), c(140,-55), c(-10,0)))
p3 <- list(rbind(c(-125,0), c(0,60), c(40,5), c(15,-45), c(-125,0)))
pols <- st_sf(value = c(1,2,3),
              by_1 = c("a", "a", "b"),
              by_2 = c(1, 1, 2),
              by_3 = factor(c("a", "a", "b")),
              geometry = st_sfc(lapply(list(p1, p2, p3), st_polygon)),
              stringsAsFactors = FALSE)
r1 <- raster(pols, res=1)

test_that("'by' argument works", {
  expect_error(
    rb <-fasterize(pols, r1, field="value", fun="sum", by ="by_1"),NA)
  expect_equal(names(rb), unique(pols$by_1))
  expect_equal(ncol(rb@data@values), length(unique(pols$by_1)))
})

test_that("'by' layers are equivalent to layers generated separately", {
  rba <- fasterize(pols, r1, field="value", fun="sum", by ="value")
  for(i in nrow(pols))
  expect_equal(as.raster(rba[[i]]),
               as.raster(fasterize(pols[i,], r1, field="value", fun="sum")))
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
  r <- raster(pols, res = 1)
  bg <- 20
  expect_error(
    f0 <- fasterize(pols, r, field = "value", fun="last", background = bg,
                    by = "by_1"), NA)
  expect_equal(unname(f0[[1]][1,1]), bg)
  expect_equal(f0@data@max, max(bg, max(pols$value)))
  expect_equal(f0@data@min, min(bg, min(pols$value)))
})


