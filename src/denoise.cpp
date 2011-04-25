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
  const double gama = 4.4;
  const int len = (int)sig.size();
  const double rc = 0.1;

  for(int n = 0; n < len; ++n) {
    double sum = sig[n];
    double avg = 0.0;

    double maxlb = -1e10;
    double minub = +1e10;

    for(int k = 2; n+k-1 < len; ++k) {
      sum += sig[n+k-1];

      double tavg = sum / k;
      double sigma = 0.0;

      for(int j = n; j < n+k; ++j)
        sigma += sqr(sig[j] - tavg);

      sigma = sqrt(sigma / k);

      minub = std::min(minub, tavg + gama * sigma);
      maxlb = std::max(maxlb, tavg - gama * sigma);

      double rk = (minub-maxlb) / (2 * gama * sigma);

      if(minub < maxlb || rk < rc) break;

      avg = tavg;
    }

    res.push_back(avg);
  }
}
