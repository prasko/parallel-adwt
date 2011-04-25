/*
  Copyright: (c) 2011 Matija Osrecki <matija.osrecki@fer.hr>
*/

#ifndef ADWT_DENOISE_H
#define ADWT_DENOISE_H

#include <vector>

typedef std::vector<double> signal;

class Denoiser {
public:
  Denoiser() {}
  virtual void denoise(const signal &sig, signal &res) = 0;
};

class DenoiserICI : public Denoiser {
public:
  DenoiserICI() {}
  virtual void denoise(const signal &sig, signal &res);
};



#endif  // ADWT_DENOISE_H
