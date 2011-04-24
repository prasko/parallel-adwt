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
    xxacc_.push_back(xxacc_[i] + x_[i] * x_[i]);
    xyacc_.push_back(xyacc_[i] + x_[i] * y_[i]);
  }
}

inline std::pair<int,int> bounds(const int wsize, const int wpos, const int n) {
  assert(wsize >= 2 && wsize <= 2*n && wpos >= 0 && wpos < n);
  return std::make_pair(wpos - (wsize+1)/2, wpos + wsize/2);
}

double Lsw::getCoef(const int wsize, const int wpos) {
  std::pair<int,int> b = bounds(wsize, wpos, n_);
  return (xyacc_[b.second+1] - xyacc_[b.first+1]) / 
    (xxacc_[b.second+1] - xxacc_[b.first+1]);
}

double* lswc(const vector<double> &y, const vector<double> &x) {
  assert(x.size() == y.size());

  int n = (int)x.size();

  // partial sums for scalar products X * X and X * Y

  vector<double> xxacc(1);  // xxacc[i] = xxacc[i-1] + Xt * X, xxacc[0] = 0
  vector<double> xyacc(1);  // xyacc[i] = xyacc[i-1] + Xt * Y, xyacc[0] = 0

  for(int i = 0; i < n; ++i) {
    xxacc.push_back(xxacc[i] + x[i] * x[i]);
    xyacc.push_back(xyacc[i] + x[i] * y[i]);
  }

  double *result = new double[(n-1) * n];

  // for each window size 2 * wsize - 1, wsize = [2, n]
  for(int wsize = 2; wsize <= n; ++wsize) {

    // iterate window position
    for(int wpos = 0; wpos < n; ++wpos) {
      std::pair<int,int> b = bounds(wsize*2-1, wpos, n);

      result[(wsize - 2)*n + wpos] = 
        (xyacc[b.second+1] - xyacc[b.first+1]) / 
        (xxacc[b.second+1] - xxacc[b.first+1]);
    }
  }

  return result;
}
