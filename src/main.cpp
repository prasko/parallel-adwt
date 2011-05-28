/*
  Copyright: (c) 2011 Matija Osrecki <matija.osrecki@fer.hr>
 */

#include <cassert>
#include <cstdio>
#include <vector>

#include "adwt.h"
#include "combine.h"

int main() {
  int width;

  scanf("%d", &width);

  signal input(width);
  
  for(int i = 0; i < width; ++i) {
    scanf("%lf", &input[i]);
  }

  Lpw *lpw = new Lsw();
  Combine::WindowCombiner *wc = 
    new Combine::ICIWindowCombiner(*lpw, 4.4, 0.85, 0.2, true);

  signal result, A, D;
  adwt(input, wc, GUESS_C, result, A, D);

  for(int i = 0; i < (int)result.size(); ++i) {
    printf("%lf\n", result[i]);
  }

  delete lpw;
  delete wc;
  
  return 0;
}
