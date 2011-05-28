/*
  Runs ICI Denoiser with preffered parameters on an array, then
  outputs denoised signal.

  Uses stdin and stdout.

  Copyright: (c) 2011 Matija Osrecki <matija.osrecki@fer.hr>
 */

#include <cassert>
#include <cstdio>

#include <vector>

#include "denoise.h"

int main() {
  int n;
  scanf("%d", &n);

  std::vector<double> signal(n);
  for(int i = 0; scanf("%lf", &signal[i]) == 1; ++i);

  Denoise::ICIDenoiser denoiser;
  Denoise::Denoiser::Result *result = denoiser.denoise(signal);

  for(int i = 0; i < n; ++i)
    printf("%lf\n", result->getDenoised(i));

  return 0;
}
