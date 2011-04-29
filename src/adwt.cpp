/*
  Copyright: (c) 2011 Matija Osrecki <matija.osrecki@fer.hr>
 */

#include <cstdio>

#include <vector>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <utility>

#include "lpw.h"
#include "combine.h"
#include "denoise.h"
#include "sysutil.h"

/*
  Main function for ADWT calculation.
*/
void adwt(signal &input, signal &A, signal &D) {

  Multiply2 mul;
  Add2 adder;
  Decimator decim;

  // *** PREPARATION ***

  // adjust input size to even
  if(input.size() % 2 == 1) 
    input.push_back(0.0);

  // number of zeroes added to begining and end of Xe and Xo
  const int zeroes_front = 2;
  const int zeroes_back = 1;

  signal xe(zeroes_front);  // Xe(z)
  signal xo(zeroes_front);  // Xo(z)

  decim.decimate(input, xe, xo);
  
  for(int i = 0; i < zeroes_back; ++i) {
    xe.push_back(0);
    xo.push_back(0);
  }

  // *** INTERPOLATION ***
  
  signal x2, x4, yd;

  // linear interpolation
  PolySystem systwin1(0.5);
  systwin1.addMember(0, 1);
  systwin1.addMember(-1, 1);

  systwin1.process(xe, x2);

  // cubic interpolation
  PolySystem systriple1(1.0/8);
  systriple1.addMember(-1, 1);
  systriple1.addMember(0, -2);
  systriple1.addMember(1, 1);

  systriple1.process(x2, x4);

  // calculate Yd(z)
  adder.add(xo, x2, -1.0, yd);

  // guess parameter b = Yd(z) / U(z)
  
  Lpw *lsw = new Lsw(yd, x4);
  //  ConfidenceInterval *ci = new ConfidenceInterval(0.005);
  // ConfidenceInterval *ci = new TunnelInterval(0.05, 0.005, 0.005);
  // WindowCombiner *wc = new SimpleWindowCombiner(*lsw, *ci);

  WindowCombiner *wc = new ICIWindowCombiner(*lsw);

  signal b_res;
  wc->combine(b_res);

  signal b_dns;
  Denoiser *dns = new ICIDenoiser(4.4, 0.7, 0.2);
  dns->denoise(b_res, b_dns);

  for(int i = 0; i < (int)b_res.size(); ++i)
    printf("(%lf %lf)", b_res[i], b_dns[i]);
  printf("\n");

  signal ub;

  mul.multiply(x4, b_res, ub);

  // calculate D(z)
  adder.add(ub, yd, 1.0, D);

  // *** UPDATE ***

  signal u2, u4, ya;

  // linear update
  PolySystem systwin2(0.25);
  systwin2.addMember(0, 1);
  systwin2.addMember(1, 1);
  systwin2.process(D, u2);

  // cubic update
  systriple1.process(u2, u4);

  // calculate Ya(z)
  adder.add(xe, u2, 1.0, ya);

  // calculate A(z)
  adder.add(ya, u4, -1.0, A);
}
