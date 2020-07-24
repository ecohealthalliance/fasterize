## Response to comments

- Fixes LTO issues

## Test environments (2020-07-24)
* local OS X install, R 4.0.2
* via r-rub: 
   windows-x86_64 (r-devel)
   ubuntu-gcc (r-release)
   fedora-clang (r-devel) 
   debian-linux-x86_64-gcc-san (r-devel)


## R CMD check results

Valid URLs disallowed in checks due to scraping:

* checking CRAN incoming feasibility ... NOTE
Maintainer: 'Noam Ross <ross@ecohealthalliance.org>'

  URL: https://dx.doi.org/10.1145/1465611.1465619
    From: man/fasterize.Rd
    Status: 403
    Message: Forbidden

Found the following (possibly) invalid DOIs:
  DOI: 10.1145/1465611.1465619
    From: DESCRIPTION
    Status: Forbidden
Found the following (possibly) invalid URLs:
    Message: 403

