/*
  Copyright: (c) 2011 Matija Osrecki <matija.osrecki@fer.hr>
 */

#include <cassert>
#include <cstdio>
#include <vector>

#include "adwt.h"
#include "denoise.h"

int main() {
  int width;

  scanf("%d", &width);

  signal input(width);
  
  for(int i = 0; i < width; ++i) {
    scanf("%lf", &input[i]);
  }
  
  // Denoiser *dns = new DenoiserICI;
  // signal res;
  // dns->denoise(input, res);
  // assert(input.size() == res.size());
  // for(int i = 0; i < (int)res.size(); ++i)
  //   printf("%lf ", res[i]);
  // printf("\n");

  signal result;
  adwt(input, result);

  for(int i = 0; i < (int)result.size(); ++i) {
    printf("%lf\n", result[i]);
  }
  
  return 0;
}
