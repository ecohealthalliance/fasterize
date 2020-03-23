FROM rocker/geospatial:devel

RUN echo "deb http://http.us.debian.org/debian/ testing non-free contrib main" >> /etc/apt/sources.list \
 && apt-get update \
 && apt-get install -y --no-install-recommends \
  gcc-10 \
  g++-10 \
  curl

RUN curl -sL https://gist.githubusercontent.com/zyedidia/d4acfcc6acf2d0d75e79004fa5feaf24/raw/a43e603e62205e1074775d756ef98c3fc77f6f8d/install_micro.sh | bash -s linux64 /usr/bin
RUN installGithub.r r-spatial/sf

