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

  while(pos < n-1) {
    const double eps = 0.05;
    const int max_wsize = n/5;

    int count = 0;
    double sum = 0.0;

    int curr_wsize = n/500;

    // guess initial average value
    while(count < 10 && pos + count < n-1) {
      sum += lpw_.getCoef(3+curr_wsize, pos);
      ++count;
    }

    // increase window size
    while(pos < n-1) {
      double param;
      while(curr_wsize < max_wsize) {
        param = lpw_.getCoef(3+curr_wsize, pos);
        curr_wsize++;

        if(fabs(param - (sum/count)) < eps) {
          res.push_back(param);
          sum += param;
          break;
        }
      }

      ++pos;
      ++count;

      if(curr_wsize == max_wsize) {
        res.push_back(param);
        sum += param;
        break;
      }
    }

    //    printf("%d\n", pos);

    // decrease window size
    while(pos < n-1) {

      double param;
      for(; curr_wsize > 0; --curr_wsize) {
        param = lpw_.getCoef(3+curr_wsize, pos);
        
        if(fabs(param - (sum/count)) < eps) {
          res.push_back(param);
          sum += param;
          break;
        }
      }

      ++pos;
      ++count;

      if(curr_wsize == 0) {
        res.push_back(param);
        break;
      }
    }
  }
}


