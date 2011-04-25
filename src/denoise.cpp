/*
  Copyright: (c) 2011 Matija Osrecki <matija.osrecki@fer.hr>
*/

#include <cmath>
#include <cstdio>
#include <algorithm>
#include <vector>

#include "denoise.h"

template <class T> inline T sqr(T x) { return x * x; }

void DenoiserICI::denoise(const signal &sig, signal &res) {
  const double gama = gama_;
  const int len = (int)sig.size();
  const double rc = rc_;

  double sum, avg, sqrs;
  double maxlb, minub;

  double tavg, sigma;
  double rk;

  for(int n = 0; n < len; ++n) {
    sum = avg = sig[n];
    sqrs = sqr(sig[n]);

    maxlb = -1e10;
    minub = +1e10;

    for(int k = 2; n+k-1 < len; ++k) {
      sum += sig[n+k-1];
      sqrs += sqr(sig[n+k-1]);
      tavg = sum / k;

      sigma = sqrt((sqrs - 2 * tavg * sum + k * sqr(tavg)) / (k-1));

      minub = std::min(minub, tavg + gama * sigma);
      maxlb = std::max(maxlb, tavg - gama * sigma);

      rk = (minub-maxlb) / (2 * gama * sigma);

      if(minub < maxlb || rk < rc) break;

      avg = tavg;
    }

    res.push_back(avg);
  }
}
