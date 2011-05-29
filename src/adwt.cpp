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
#include "combine.h"
#include "sysutil.h"

using Combine::WindowCombiner;

static Signal* combineDenoise(const Signal &input, const ADWTSubSystem &subsys,
                              WindowCombiner *wcombiner) {

  Add2 adder;

  Signal *param = new Signal();
  Signal sigd;

  adder.add(input, subsys.sig_x2(), (subsys.x2_prefix() ? 1.0 : -1.0), sigd);
  wcombiner->setLpwData(sigd, subsys.sig_x4());
  wcombiner->combine(*param);

  return param;
}

/*
  Main function for ADWT calculation.
*/

void adwt(Signal &input, WindowCombiner *wcombiner, GuessMode adwt_mode,
          Signal &res, Signal &sig_a, Signal &sig_d) {

  Decimator sys_dec;

  // *** PREPARATION ***

  // adjust input size to even
  if(input.size() % 2 == 1) 
    input.push_back(0.0);

  // number of zeroes added to begining and end of Xe and Xo
  const int zeroes_front = 2;
  const int zeroes_back = 1;

  Signal sig_xe(zeroes_front);  // Xe(z)
  Signal sig_xo(zeroes_front);  // Xo(z)

  sys_dec.decimate(input, sig_xe, sig_xo);
  
  for(int i = 0; i < zeroes_back; ++i) {
    sig_xe.push_back(0);
    sig_xo.push_back(0);
  }

  // *** INTERPOLATION ***
  
  ADWTInterpolate sys_ip;
  sys_ip.process(sig_xe);
  
  // guess parameter b

  Signal *param_b = NULL;

  if(adwt_mode == GUESS_B) {
    param_b = combineDenoise(sig_xo, sys_ip, wcombiner);
    sys_ip.finalize(sig_xo, true, param_b, sig_d);
  } else {
    sys_ip.finalize(sig_xo, true, NULL, sig_d);
  }
  
  // *** UPDATE ***

  ADWTUpdate sys_up;
  sys_up.process(sig_d);

  // guess parameter c

  Signal *param_c = NULL;

  if(adwt_mode == GUESS_C) {
    param_c = combineDenoise(sig_xe, sys_up, wcombiner);
    sys_up.finalize(sig_xe, true, param_c, sig_a);
  } else {
    sys_up.finalize(sig_xe, true, NULL, sig_a);
  }

  // *** RECONSTRUCTION ***

  Signal sig_ye, sig_yo;

  sys_up.finalize(sig_a, false, param_c, sig_ye);

  sys_ip.process(sig_ye);
  sys_ip.finalize(sig_d, false, param_b, sig_yo);

  assert(res.size() == 0);
  
  for(int i = 0; i < (int)sig_ye.size(); ++i) {
    res.push_back(sig_ye[i]);
    res.push_back(sig_yo[i]);
  }

  // *** CLEAN ***

  if(param_b) delete param_b;
  if(param_c) delete param_c;
}

void ADWTSubSystem::finalize(const Signal &input, bool positive, 
                             const Signal *param, Signal &res) {
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

void ADWTInterpolate::process_(const Signal &input) {
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

bool ADWTInterpolate::x2_prefix() const { return false; }
bool ADWTInterpolate::x4_prefix() const { return true; }

void ADWTUpdate::process_(const Signal &input) {
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

bool ADWTUpdate::x2_prefix() const { return true; }
bool ADWTUpdate::x4_prefix() const { return false; }
