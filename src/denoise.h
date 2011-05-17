/*
  Copyright: (c) 2011 Matija Osrecki <matija.osrecki@fer.hr>
*/

#ifndef ADWT_DENOISE_H
#define ADWT_DENOISE_H

#include <vector>
#include <utility>

typedef std::vector<double> signal;


struct Denoiser {
  Denoiser() : result_(NULL) {}
  virtual void denoise(const signal &sig, signal &res) = 0;

protected:
  signal *result_;
};

struct ICIDenoiser : public Denoiser {
 ICIDenoiser(double gama=4.4, double rc=0.85, double sigma=0.2) : 
   Denoiser(), gama_(gama), rc_(rc), sigma_(sigma) {}

  virtual void denoise(const signal &sig, signal &res) = 0;

protected:
  const double gama_;
  const double rc_;
  const double sigma_;
};

struct OneWayICIDenoiser : public ICIDenoiser {
 OneWayICIDenoiser(double gama=4.4, double rc=0.85, double sigma=0.2) : 
   ICIDenoiser(gama, rc, sigma) {}

  virtual void denoise(const signal &sig, signal &res);

  const std::vector<int>& intervalRight() const { 
    return interval_right_; 
  }

protected:
  std::vector<int> interval_right_;
};

struct TwoWayICIDenoiser : public OneWayICIDenoiser {
 TwoWayICIDenoiser(double gama=4.4, double rc=0.85, double sigma=0.2) : 
   OneWayICIDenoiser(gama, rc, sigma) {}

  void denoise(const signal &sig, signal &res);

  const std::vector<int>& intervalLeft() const {
    return interval_left_;
  }

private:
  std::vector<int> interval_left_;
};

#endif  // ADWT_DENOISE_H
