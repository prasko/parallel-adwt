/*
  Copyright: (c) 2011 Matija Osrecki <matija.osrecki@fer.hr>
 */

#include <cmath>
#include <cstdio>
#include <algorithm>
#include <vector>

#include "combine.h"


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


