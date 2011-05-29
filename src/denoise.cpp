/*
  Copyright: (c) 2011 Matija Osrecki <matija.osrecki@fer.hr>
*/

#include <cmath>
#include <cstdio>
#include <algorithm>
#include <vector>

#include "denoise.h"

template <class T> inline T sqr(T x) { return x * x; }

namespace Denoise {

  void Denoiser::Result::addDenoised(double member) { 
    dns_signal.push_back(member); 
  }

  double Denoiser::Result::getDenoised(int index) { 
    assert(index >= 0 && index < (int)dns_signal.size());
    return dns_signal[index]; 
  }

  void Denoiser::denoiseMultiple(const std::vector<Signal> &input,
                                 std::vector<Result*> &res) {
    assert(res.size() == 0);

    for(int i = 0; i < (int)res.size(); ++i)
      res.push_back(denoise(input[i]));
  }

  void ICIDenoiser::Result::addLeft(int interval) { 
    left_interval.push_back(interval);
  }
    
  void ICIDenoiser::Result::addRight(int interval) {
    right_interval.push_back(interval);
  }

  int ICIDenoiser::Result::getLeft(int index) {
    assert(index >= 0 && index <= (int)left_interval.size());
    return left_interval[index];
  }

  int ICIDenoiser::Result::getRight(int index) {
    assert(index >= 0 && index <= (int)right_interval.size());
    return right_interval[index];
  }

  void ICIDenoiser::denoise_(const Signal &sig, Signal &res, 
                             std::vector<int> &interval) {
    const int len = (int)sig.size();
    
    // init variables
    double sum, avg, tavg, curr_sigma, rk;
    double maxlb, minub;

    // window size
    int k;

    for(int n = 0; n < len; ++n) {
      // init sum and average
      sum = avg = sig[n];

      // init max upper and min lower bounds
      maxlb = -1e10;
      minub = +1e10;

      // increment k - window size
      for(k = 2; n+k-1 < len; ++k) {
        // calculate current sum, temp. average and sigma
        sum += sig[n+k-1];
        tavg = sum / k;
        curr_sigma = sigma / sqrt(k);

        // recalculate max upper and min lower bounds
        minub = std::min(minub, tavg + gama * curr_sigma);
        maxlb = std::max(maxlb, tavg - gama * curr_sigma);

        // calculate new RICI parameter Rk
        rk = (minub - maxlb) / (2 * gama * curr_sigma);

        // break if ICI or RICI conditions achieved
        if(minub < maxlb || rk < rc) break;

        avg = tavg;
      }

      // add average and window size to solution
      res.push_back(avg);
      interval.push_back(k-1);
    }
  }

  Denoiser::Result* ICIDenoiser::denoise(const Signal &sig) {
    Result *res = new Result();

    // init result signals, and reversed signal for left way
    Signal res_left, res_right, sig_left(sig);
    std::reverse(sig_left.begin(), sig_left.end());

    // denoise right, then left way
    denoise_(sig, res_right, res->right_interval);
    denoise_(sig_left, res_left, res->left_interval);

    // reverse left way result data
    std::reverse(res->left_interval.begin(), res->left_interval.end());
    std::reverse(res_left.begin(), res_left.end());

    double b, br;
    int k, kr;

    for(int i = 0; i < (int)res_left.size(); ++i) {
      // get denoised signal members; br = reversed (left wise)
      b = res_right[i], br = res_left[i];

      // get window sizes; kr = reversed (left wise)
      k = res->getRight(i), kr = res->getLeft(i);

      // calculate final denoised member using weighted sum
      // weight is the window size
      res->addDenoised(b * k / (k+kr) + br * kr / (k+kr));
    }

    return res;
  }

}  // namespace
