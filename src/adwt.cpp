/*
  Copyright: (c) 2011 Matija Osrecki <matija.osrecki@fer.hr>
 */

#include <cstdio>

#include <vector>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <utility>

#include "adwt.h"
#include "lpw.h"
#include "combine.h"
#include "denoise.h"
#include "sysutil.h"

/*
  Main function for ADWT calculation.
*/
void adwt(signal &input, signal &res) {
  Add2 sys_add;
  Decimator sys_dec;

  // *** PREPARATION ***

  // adjust input size to even
  if(input.size() % 2 == 1) 
    input.push_back(0.0);

  // number of zeroes added to begining and end of Xe and Xo
  const int zeroes_front = 2;
  const int zeroes_back = 1;

  signal sig_xe(zeroes_front);  // Xe(z)
  signal sig_xo(zeroes_front);  // Xo(z)

  sys_dec.decimate(input, sig_xe, sig_xo);
  
  for(int i = 0; i < zeroes_back; ++i) {
    sig_xe.push_back(0);
    sig_xo.push_back(0);
  }

  // *** INTERPOLATION ***
  
  ADWTInterpolate sys_ip;
  sys_ip.process(sig_xe);
  
  // *** PARAMETER GUESSING ***

  signal sig_yd;
  sys_add.add(sig_xo, sys_ip.sig_x2(), -1.0, sig_yd);

  Lpw *lsw = new Lsw(sig_yd, sys_ip.sig_x4());
  //WindowCombiner *wc = new DenoiserWindowCombiner(*lsw);
  WindowCombiner *wc = new ICIWindowCombiner(*lsw);

  signal b_res, b_dns;
  wc->combine(b_res);

  Denoiser *dns = new TwoWayICIDenoiser(4.4, 0.7, 0.2);
  dns->denoise(b_res, b_dns);

  // for(int i = 0; i < (int)b_res.size(); ++i) { 
  //   printf("(%lf %lf)", b_res[i], b_dns[i]);
  // }

  // add up

  signal sig_d;
  sys_ip.finalize(sig_xo, true, &b_dns, sig_d);
  //sys_ip.finalize(sig_xo, true, NULL, sig_d);
  
  // *** UPDATE ***

  signal sig_a;

  ADWTUpdate sys_up;
  sys_up.process(sig_d);
  sys_up.finalize(sig_xe, true, NULL, sig_a);

  // for(int i = 0; i < (int)sig_a.size(); ++i) {
  //   printf("(%f %f)", sig_a[i], sig_d[i]);
  // }

  // *** RECONSTRUCTION ***

  signal sig_ye, sig_yo;

  sys_up.finalize(sig_a, false, NULL, sig_ye);
  sys_ip.process(sig_ye);
  sys_ip.finalize(sig_d, false, &b_dns, sig_yo);
  //sys_ip.finalize(sig_d, false, NULL, sig_yo);

  assert(res.size() == 0);
  
  for(int i = 0; i < (int)sig_ye.size(); ++i) {
    res.push_back(sig_ye[i]);
    res.push_back(sig_yo[i]);
  }
}

void ADWTSubSystem::finalize(const signal &input, bool positive, 
                             const signal *param, signal &res) {
  assert(res.size() == 0);
  assert(input.size() == x2_.size());
  assert(param == NULL || param->size() == input.size());

  Sumator sys_sum(res);
  sys_sum.sumSignal(input, true);
  sys_sum.sumSignal(x2_, positive ? x2_prefix() : !x2_prefix());

  if(param) {
    sys_sum.sumMultiply(x4_, *param, positive ? x4_prefix() : !x4_prefix());
  } else {
    sys_sum.sumSignal(x4_, positive ? x4_prefix() : !x4_prefix());
  }
}

void ADWTInterpolate::process_(const signal &input) {
  // linear interpolation
  PolySystem systwin1(0.5);
  systwin1.addMember(0, 1);
  systwin1.addMember(-1, 1);

  systwin1.process(input, x2_);

  // cubic interpolation
  PolySystem systriple1(1.0/8);
  systriple1.addMember(-1, 1);
  systriple1.addMember(0, -2);
  systriple1.addMember(1, 1);

  systriple1.process(x2_, x4_);
}

bool ADWTInterpolate::x2_prefix() { return false; }
bool ADWTInterpolate::x4_prefix() { return true; }

void ADWTUpdate::process_(const signal &input) {
  // linear update
  PolySystem systwin2(0.25);
  systwin2.addMember(0, 1);
  systwin2.addMember(1, 1);

  systwin2.process(input, x2_);

  // cubic update
  PolySystem systriple1(1.0/8);
  systriple1.addMember(-1, 1);
  systriple1.addMember(0, -2);
  systriple1.addMember(1, 1);

  systriple1.process(x2_, x4_);
}

bool ADWTUpdate::x2_prefix() { return true; }
bool ADWTUpdate::x4_prefix() { return false; }
