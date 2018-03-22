## Response to comments

This is a second resubmission.  Per request, I have quoted all package names and
added parentheses to all function names in the DESCRIPTION.  I have also found
and added a citation and DOI for the algorithm implemented.

In addition, sf has been moved from `Imports:` to `Suggests:` and I have
updated documentation.  We have also added ORCIDs for authors.  The package
has been tested again and passes R CMD check on local OSX and all r-hub
platforms.

## Test environments
* local OS X install, R 3.4.4
* via r-rub: 
   windows-x86_64 (r-devel)
   ubuntu-gcc (r-release)
   fedora-clang (r-devel) 
   debian-linux-x86_64-gcc-san (r-devel)

## R CMD check results

* checking CRAN incoming feasibility ... NOTE
Maintainer: 'Noam Ross <ross@ecohealthalliance.org>'

New submission

Possibly mis-spelled words in DESCRIPTION:
  al (13:13)  (This is from 'et al.' in the citation.)


