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
  DenoiserICI(double gama=4.4, double rc=0.85) : gama_(gama), rc_(rc) {}
  virtual void denoise(const signal &sig, signal &res);

private:
  double gama_;
  double rc_;
};



#endif  // ADWT_DENOISE_H
