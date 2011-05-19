/*
  Copyright: (c) 2011 Matija Osrecki <matija.osrecki@fer.hr>
 */

#include <cassert>
#include <cstdio>
#include <vector>

#include "lpw.h"

using std::vector;

template <class T> inline T sqr(const T x) { return x * x; }

void Lsw::init() {
  xxacc_.push_back(0.0);
  xyacc_.push_back(0.0);

  for(int i = 0; i < n_; ++i) {
    xxacc_.push_back(xxacc_[i] + x_->at(i) * x_->at(i));
    xyacc_.push_back(xyacc_[i] + x_->at(i) * y_->at(i));
  }
}

inline std::pair<int,int> bounds(const int wsize, const int wpos, const int n) {
  assert(wsize >= 2 && wsize <= 2*n && wpos >= 0 && wpos < n);
  return std::make_pair(wpos - (wsize+1)/2, wpos + wsize/2);
}

double Lsw::getCoef(const int wsize, const int wpos) {
  assert(y_ && x_);

  std::pair<int,int> b = bounds(wsize, wpos, n_);
  return (xyacc_[b.second+1] - xyacc_[b.first+1]) / 
    (xxacc_[b.second+1] - xxacc_[b.first+1]);
}

void Lsw::setLpwData(const signal &y, const signal &x) {
  assert(y.size() == x.size());

  y_ = &y;
  x_ = &x;
  n_ = y.size();

  init();
}
