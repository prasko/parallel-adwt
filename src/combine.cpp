/*
  Copyright: (c) 2011 Matija Osrecki <matija.osrecki@fer.hr>
 */

#include <cmath>
#include <cstdio>
#include <algorithm>
#include <vector>

#include "combine.h"
#include "denoise.h"

using Denoise::ICIDenoiser;

namespace Combine {

  void DenoiserWindowCombiner::combine(std::vector<double> &res) {
    int n = lpw_.length();

    std::vector<double> ratios(n);
    res.resize(n);

    double ilr_ratio;

    for(int wsize = 100; wsize < n/10; wsize = (double)1.31 * wsize) {
      std::vector<double> windowed_signal;

      fprintf(stderr, "%d\n", wsize);

      for(int i = 0; i < n; ++i) {
        windowed_signal.push_back(lpw_.getCoef(wsize, i));
      }
    
      ICIDenoiser::Result *denoise_res = 
        dynamic_cast<ICIDenoiser::Result*>(denoiser_.denoise(windowed_signal));

      int left, right;
      for(int i = 0; i < n; ++i) {
        left = denoise_res->getLeft(i);
        right = denoise_res->getRight(i);

        ilr_ratio = 1.0 / std::min((double)right / left, (double)left / right);
      
        res[i] += ilr_ratio * denoise_res->getDenoised(i);
        ratios[i] += ilr_ratio;
      }
    }

    // za pojedini wsize, pomnozim vrijednosti parametara sa min(il/ir, ir/il)
    // onda to sve zbrojim i pomnozim sa sumom svih tih min

    for(int i = 0; i < n; ++i) {
      res[i] /= ratios[i];
    }
  }

  void SimpleWindowCombiner::combine(std::vector<double> &res) {
    int pos = 0;
    int n = lpw_.length();

    while(pos < n) {
      const int max_wsize = n/5;
      const int init_take = 10;

      int count = 0;
      double sum = 0.0;

      int curr_wsize = n/500;

      // guess initial average value
      while(count < init_take && pos + count < n) {
        sum += lpw_.getCoef(3+curr_wsize, pos);
        ++count;
      }

      // increase window size
      while(pos < n-1) {
        double param;
        while(curr_wsize < max_wsize) {
          param = lpw_.getCoef(3+curr_wsize, pos);
          curr_wsize++;

          if(interval_.inside(count-init_take, param - (sum/count))) {
            res.push_back(param);
            sum += param;
            break;
          }
        }

        if(curr_wsize == max_wsize) break;

        ++pos;
        ++count;
      }

      int tcount = count-init_take;

      // decrease window size
      while(pos < n) {

        double param;
        for(; curr_wsize > 0; --curr_wsize) {
          param = lpw_.getCoef(3+curr_wsize, pos);
        
          if(interval_.inside(2*tcount-(count-init_take), param - (sum/count))) {
            res.push_back(param);
            sum += param;
            break;
          }
        }

        if(curr_wsize == 0) break;

        ++pos;
        ++count;
      }
    }
  }

  void ICIWindowCombiner::combine_(
                                   std::vector<std::pair<double,int> > &res, int dir=0) {

    static const double gama = gama_;
    static const double base_sigma = sigma_;
    static const double rc = rc_;
  
    const int len = lpw_.length();
    const int max_wsize = len/10;
    const int min_wsize = std::min(100, len/100+3);
  
    double maxlb, minub;
    double avg, sum, tavg, rk, sigma;
    int k, wsize;

    for(int n = 0; n < len; ++n) {
      sum = avg = 0;

      maxlb = -1e10;
      minub = +1e10;

      wsize = min_wsize;

      for(k = 1; n+k-1 < len; wsize++,  k++) {
        if(dir == 0) {
          sum += lpw_.getCoef(std::min(wsize, max_wsize), n+k-1);
        } else {
          sum += lpw_.getCoef(std::min(wsize, max_wsize), len-(n+k));
        }

        tavg = sum / k;

        sigma = base_sigma / sqrt(k);

        minub = std::min(minub, tavg + gama * sigma);
        maxlb = std::max(maxlb, tavg - gama * sigma);

        rk = (minub - maxlb) / (2 * gama * sigma);

        if(minub < maxlb || (rici_ && rk < rc)) break;  // ICI or RICI

        avg = tavg;
      }
    
      res.push_back(std::make_pair(avg, k));
    }
  }

  void ICIWindowCombiner::combine(std::vector<double> &res) {
    std::vector< std::pair<double, int> > res_right, res_left;

    combine_(res_right, 0);
    combine_(res_left, 1);

    std::reverse(res_left.begin(), res_left.end());
  
    for(int i = 0; i < (int)res_left.size(); ++i) {
      double b = res_right[i].first, br = res_left[i].first;
      int k = res_right[i].second, kr = res_left[i].second;

      res.push_back(b * k / (k+kr) + br * kr / (k+kr));
    }
  }

}  // namespace
