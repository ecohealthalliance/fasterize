#define ARMA_64BIT_WORD  //required to support arma vectors > 2GB
#include <RcppArmadillo.h>
// [[Rcpp::plugins(cpp11)]
// [[Rcpp::depends(RcppArmadillo)]]

typedef std::deque<std::array<double, 2>> Fragment;


//TODO: MAKE ALL SEGMENTS 32 bit integers to speed up? Will require
//a warning on max matrix sizes. Do after profiling. Chage add_segment
//math to double numbers and add/subtract only integers.
class FragmentList: std::list<Fragment> {
public:
  void grow(uint8_t square, arma::uword x, arma::uword y, bool smooth, arma::mat &raster_matrix, double value);
  void add_segment(double x0, double y0,  double y1, double y2);
};

void FragmentList::grow(uint8_t square, arma::uword x, arma::uword y, bool smooth, arma::mat &raster_matrix, double value) {
  switch(square) {
  case 0:
    break;
  case 1:
    this->add_segment(x - 1, y - 0.5, x - 0.5, y);
  case 2:
    this->add_segment(x - 0.5, y, x, y - 0.5);
  case 3:
    this->add_segment(x - 1, y - 0.5, x, y - 0.5);
  case 4:
    this->add_segment(x - 0.5, y - 1, x - 1, y - 0.5);
  case 5:
    this->add_segment(x - 0.5, y - 1, x - 0.5, y);
  case 6:
    //For this and case 9, in the case where we may smooth the contours
    //we want to disambiguate if a square with equal opposite corners is an
    //ithmus or a channel.
    if(smooth &&
       (raster_matrix.at(x, y) + raster_matrix.at(x, y - 1) +
       raster_matrix.at(x - 1, y) + raster_matrix.at(x - 1, y - 1))/4 <
         value) {
      this->add_segment(x - 0.5, y - 1, x - 1, y - 0.5);
      this->add_segment(x - 0.5, y, x, y - 0.5);
    } else {
      this->add_segment(x - 0.5, y - 1, x, y - 0.5);
      this->add_segment(x - 0.5, y, x - 1, y - 0.5);
    }
  case 7:
    this->add_segment(x - 0.5, y -1 , x, y - 0.5);
  case 8:
    this->add_segment(x, y - 0.5, x - 0.5, y - 1);
  case 9:
    if(smooth &&
       (raster_matrix.at(x, y) + raster_matrix.at(x, y - 1) +
       raster_matrix.at(x - 1, y) + raster_matrix.at(x - 1, y - 1))/4 <
         value) {
      this->add_segment(x, y - 0.5, x - 0.5, y - 1);
      this->add_segment(x - 1, y - 0.5, x - 0.5, y);
    } else {
      this->add_segment(x, y - 0.5, x - 0.5, y);
      this->add_segment(x - 1, y - 0.5, x - 0.5, y - 1);
    }
  case 10:
    this->add_segment(x - 0.5, y, x - 0.5, y - 1);
  case 11:
    this->add_segment(x - 1, y - 0.5, x - 0.5, y - 1);
  case 12:
    this->add_segment(x, y - 0.5, x - 1 , y - 0.5);
  case 13:
    this->add_segment(x, y - 0.5, x - 0.5, y);
  case 14:
    this->add_segment(x - 0.5, y, x - 1 , y - 0.5);
  case 15:
    break;
  }
}

void FragmentList::add_segment(double x0, double y0,  double x1, double y1) {
  bool flag = true;
  if(this->size()) {  //If we already started a fragment...
    //loop through fragments looking to match up with one
    for(auto fi = this->begin(); fi != this->end(); ++fi) {
      //check if connects to front
      if((*fi).front()[0] == x1 && (*fi).front()[1] == y1) {
        std::array<double, 2> newfront = {x0, y0};
        // If continuing the same slope, drop the most recent point before
        // adding the new one
        if((x1 - x0) == ((*fi)[1][0] - x1) &&
           (y1 - y0) == ((*fi)[1][1] - y1)) {
             (*fi).pop_front();
        }
        (*fi).push_front(newfront);
        flag = false;
        break;
        //check if connects to back
      } else if ((*fi).back()[0] == x0 && (*fi).back()[1] == y0) {
        std::array<double, 2> newback = {x1, y1};
        //check slope again
        if((x1 - x0) == (x0 - (*fi).end()[-2][0]) &&
           (y1 - y0) == (y0 - (*fi).end()[-2][1])) {
             (*fi).pop_back();
        }
        (*fi).push_front(newback);
        flag = false;
        break;
      }
    }
  }
  //if nothing is found, or no fragments have started, start a new fragment
  if(flag) {
    std::array<double, 2> front = {x0, y0};
    std::array<double, 2> back = {x1, y1};
    Fragment newfrag;
    newfrag.push_back(start);
    newfrag.push_back(end);
    this->push_back(newfrag);
  }
}

FragmentList marching_squares(arma::mat &raster_matrix, double value, bool close) {
  arma::uword x, y = 0;
  uint8_t t0, t1, t2, t3;  //bottom-left, bottom-right, top-right, top-left
  FragmentList frags;

  if(close) {
    t1 = raster_matrix.at(x, y) >= value;  //top-left corner
    frags.grow(t1 << 1);
    while (x++ < (raster_matrix.n_cols - 1)) { //top row
      t0 = t1;
      t1 = raster_matrix.at(x + 1, y) >= value;
      frags.grow(t0 | t1 << 1);
    }
    frags.grow(t1 << 0);  //top-right corner
  } else {
    t0 = raster_matrix.at(x + 1, y) >= value;
  }

  while (y++ < (raster_matrix.n_rows - 1)) {

    x = 0;                      //left side
    t1 = raster_matrix.at(x, y) >= value;
    t2 = raster_matrix.at(x, y - 1) >= value;
    if(close) {
      frags.grow(t1 << 1 | t2 << 2);
    }

    while (x++ < (raster_matrix.n_cols - 1)) {  //middle cells
      t0 = t1;
      t1 = raster_matrix.at(x, y) >= value;
      t3 = t2;
      t3 = raster_matrix.at(x, y - 1) >= value;
      frags.grow(t0 | t1 << 1 | t2 << 2 | t3 << 3);
    }
    if(close) {
      frags.grow(t1 | t2 << 3);  // right side
    }
  }

  if(close) {
    x = 0;
    t2 = raster_matrix.at(x, y) >= value;  //bottom-left corner
    frags.grow(t2 << 2);
    while(x++ < (raster_matrix.n_cols - 1)) { //bottom row
      t3 = t2;
      t2 = raster_matrix.at(x, y) >= value;
      frags.grow(t2 << 2 | t3 << 3);
    }
    frags.grow(t2 << 3);
  }

  return(frags);
}

// [[Rcpp::export]]
arma::mat polygonize(Rcpp::RObject raster, bool close) {

  bool from_raster;
  arma::mat raster_matrix;
  if(raster.isS4() && raster.inherits("RasterLayer")) {
    from_raster = true;
  } else if(!raster.isObject() && raster.hasAttribute("dim")) {
    from_raster = false;
  } else {
    Rcpp::stop("polygonize currently only supports RasterLayer or matrix inputs");  //#nolint
  }

  if(from_raster) {
    Rcpp::S4 raster_s4(raster);
    Rcpp::S4 raster_data = Rcpp::S4(raster_s4.slot("data"));
    Rcpp::NumericVector raster_vals(raster_data.slot("values"));
    raster_matrix = arma::mat(raster_vals.begin(),
                              raster.slot("nrows"), raster.slot("ncols"),
                              false, true);
  } else {
    Rcpp::NumericMatrix raster_nmat(raster);
    raster_matrix = arma::mat(raster_nmat.begin(),
                              raster_nmat.nrow(), raster_nmat.ncol(),
                              false, true);
  }

  double value = 0;
  FragmentList frags = marching_squares(raster_matrix, value, close);

  return(sf_out);
}


// You can include R code blocks in C++ files processed with sourceCpp
// (useful for testing and development). The R code will be automatically
// run after the compilation.
//

