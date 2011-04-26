/*
  Copyright: (c) 2011 Matija Osrecki <matija.osrecki@fer.hr>
*/

#ifndef ADWT_DENOISE_H
#define ADWT_DENOISE_H

#include <vector>
#include <utility>

typedef std::vector<double> signal;

struct Denoiser {
  Denoiser() {}
  virtual void denoise(const signal &sig, signal &res) = 0;
};


struct ICIDenoiser : public Denoiser {
 ICIDenoiser(double gama=4.4, double rc=0.85, double sigma=0.2) : 
   gama_(gama), rc_(rc), sigma_(sigma) {}

  virtual void denoise(const signal &sig, signal &res);

private:
  void denoise1_(const signal &sig, std::vector< std::pair<double,int> > &res);

  double gama_;
  double rc_;
  double sigma_;
};



#endif  // ADWT_DENOISE_H
