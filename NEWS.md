# fasterize 1.0.5

* Input geometries may now be of type "sfc_GEOMETRY" or other mixed types. Any non polygon
 geometry is dropped with a message. 
 
* 'field' may now be a numeric, integer, or factor vector or (as before) a name of a column in 
  the input data frame. This means we can `fasterize(vec, r, field = seq_along(vec))` or
  `fasterize(df, r, field = seq_len(dim(df)[1L]))` or with any values we like. 

* Now supporting any geometry or dataframe input supported by wk (wkb, wkt, geos, as well as sf). 

* Namespaced documentation fixes thanks to CRAN. 

* Removed raster raster() method, not needed (raster handles sf now to obtain the 6 numbers and a string). 

# fasterize 1.0.4

* New maintainer Michael Sumner. 

* Fixes a number of -Wconversion warnings (uword/double from edge.h, and int/size_t from edgelist.cpp) listed in #46. 

* Fixes CRAN warnings on incorrect use of bitwise logical operator #44. 

* Fixes CRAN warnings on unused LazyData property. 

* Fixes incorrect file metadata issue #41 by PR contributed by Antoine Stevens #42. 

* Now using 'main' branch as default. 

# fasterize 1.0.3

* Fixes link-time-optimization issues

# fasterize 1.0.2

* Modifies C code to comply with gcc-10 requirements

# fasterize 1.0.0

* First public release.
