/*
  Copyright: (c) 2011 Matija Osrecki <matija.osrecki@fer.hr>
*/

#include <cmath>
#include <cstdio>
#include <algorithm>
#include <vector>

#include "denoise.h"

template <class T> inline T sqr(T x) { return x * x; }

void ICIDenoiser::denoise1_(const signal &sig, 
                            std::vector< std::pair<double,int> > &res) {
  const double gama = gama_;
  const double sigma_noise = sigma_;

  const int len = (int)sig.size();
  const double rc = rc_;

  double sum, avg;
  double maxlb, minub;

  double tavg, sigma;
  double rk;

  int k;

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

    res.push_back(std::make_pair(avg, k-1));    
  }
}

void ICIDenoiser::denoise(const signal &sig, signal &res) {
  signal sig_rev(sig);
  std::reverse(sig_rev.begin(), sig_rev.end());
  
  std::vector< std::pair<double, int> > res_right, res_left;

  denoise1_(sig, res_right);
  denoise1_(sig_rev, res_left);

  std::reverse(res_left.begin(), res_left.end());

  for(int i = 0; i < (int)res_left.size(); ++i) {
    double b = res_right[i].first, br = res_left[i].first;
    int k = res_right[i].second, kr = res_left[i].second;

    res.push_back(b * k / (k+kr) + br * kr / (k+kr));
  }
}
