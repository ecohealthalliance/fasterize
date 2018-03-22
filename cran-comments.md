<<<<<<< HEAD
## Resubmission
This is a resubmission. In this version I have:

-  Found and fixed an the empty link in the vignette.

in response to the comment:

Found the following (possibly) invalid URLs:
   URL:
     From: inst/doc/using-fasterize.html
     Message: Empty URL

Please fix and resubmit.

I have re-tested in the following environments
* local OS X install, R 3.4.4
* Linux and Wondows, devel and release, via r-hub

## R CMD check results

0 errors | 0 warnings | 1 note

N  checking CRAN incoming feasibility
   Maintainer: 'Noam Ross <ross@ecohealthalliance.org>'
   
   New submission
   
=======
## Test environments
* local OS X install, R 3.4.4
* ubuntu 14.04 (on r-hub), R 3.4.4
* win-builder (devel and release)

## R CMD check results

* checking CRAN incoming feasibility ... NOTE
Maintainer: 'Noam Ross <ross@ecohealthalliance.org>'

New submission

Possibly mis-spelled words in DESCRIPTION:
  al (13:13)  (This is from 'et al.' in the citation.)

## Response to comments

This is a second resubmission.  Per request, I have quoted all package names and
added parentheses to all function names in the DESCRIPTION.  I have also found
and added a citation and DOI for the algorithm implemented.

In addition, sf has been moved from `Imports:` to `Suggests:` and I have
updated documentation.  We have also added ORCIDs for authors.  The package
has been tested again and passes R CMD check on local OSX and all r-hub
platforms.
>>>>>>> low-dep
