/*
  Copyright: (c) 2011 Matija Osrecki <matija.osrecki@fer.hr>
*/

#include <cmath>
#include <cstdio>
#include <algorithm>
#include <vector>

#include "denoise.h"

template <class T> inline T sqr(T x) { return x * x; }

void OneWayICIDenoiser::denoise(const signal &sig, signal &res) {
  const double gama = gama_;
  const double sigma_noise = sigma_;

  const int len = (int)sig.size();
  const double rc = rc_;

  double sum, avg, tavg, sigma, rk;
  double maxlb, minub;

  int k;

  interval_right_.clear();
  result_ = &res;
  
  for(int n = 0; n < len; ++n) {
    sum = avg = sig[n];

    maxlb = -1e10;
    minub = +1e10;

    for(k = 2; n+k-1 < len; ++k) {
      sum += sig[n+k-1];
      tavg = sum / k;

      sigma = sigma_noise / k;

      minub = std::min(minub, tavg + gama * sigma);
      maxlb = std::max(maxlb, tavg - gama * sigma);

      rk = (minub - maxlb) / (2 * gama * sigma);

      if(minub < maxlb || rk < rc) break;

      avg = tavg;
    }

    res.push_back(avg);
    interval_right_.push_back(k-1);
  }
}

void TwoWayICIDenoiser::denoise(const signal &sig, signal &res) {
  signal sig_left(sig);
  std::reverse(sig_left.begin(), sig_left.end());

  std::vector<double> res_left, res_right;
  
  this->OneWayICIDenoiser::denoise(sig_left, res_left);

  interval_left_.resize(interval_right_.size());

  reverse_copy(interval_right_.begin(), interval_right_.end(), 
               interval_left_.begin());

  std::reverse(res_left.begin(), res_left.end());

  this->OneWayICIDenoiser::denoise(sig, res_right);

  double b, br;
  int k, kr;
  for(int i = 0; i < (int)res_left.size(); ++i) {
    b = res_right[i];
    br = res_left[i];

    k = interval_right_[i];
    kr = interval_left_[i];

    res.push_back(b * k / (k+kr) + br * kr / (k+kr));
  }

  result_ = &res;
}
