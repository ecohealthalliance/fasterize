context("funs")
suppressPackageStartupMessages(library(sf))

p1 <- rbind(c(-180,-20), c(-140,55), c(10, 0), c(-140,-60), c(-180,-20))
hole <- rbind(c(-150,-20), c(-100,-10), c(-110,20), c(-150,-20))
p1 <- list(p1, hole)
p2 <- list(rbind(c(-10,0), c(140,60), c(160,0), c(140,-55), c(-10,0)))
p3 <- list(rbind(c(-125,0), c(0,60), c(40,5), c(15,-45), c(-125,0)))
vals <- 1:3
pols <- st_sf(value = vals,
              geometry = st_sfc(lapply(list(p1, p2, p3), st_polygon)))
r <- raster(pols, res = 1)

funs <- c("sum", "first", "last", "min", "max", "count", "any")
outval <- c(sum(vals), vals[1], vals[3], min(vals), max(vals),
            length(vals), any(as.logical(vals)))

for (i in seq_along(funs)) {

  test_that(paste(funs[1], "function works"), {
    rastout <- fasterize(pols, r, field = "value", fun = funs[i])
    expect_equal(unname(rastout[60,172]), outval[i])
  })

}

test_that("disallowed aggregation function is rejected", {
  invalid_fn_name <- "yo"
  expect_error(
    fasterize(pols, r, field = "value", fun =  invalid_fn_name),
    paste0("'fun' has an invalid value: ", invalid_fn_name)
  )
})
