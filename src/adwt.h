/*
  Copyright: (c) 2011 Matija Osrecki <matija.osrecki@fer.hr>
*/

#ifndef ADWT_ADWT_H
#define ADWT_ADWT_H

#include <vector>

namespace Combine {
  class WindowCombiner;
}

using Combine::WindowCombiner;

typedef std::vector<double> Signal;

enum GuessMode { NONE, GUESS_B, GUESS_C };

void adwt(Signal &input, WindowCombiner *wcombiner, GuessMode adwt_mode,
          Signal &res, Signal &sig_a, Signal &sig_d);

struct ADWTSubSystem {
  ADWTSubSystem() {}

  void process(const Signal &input) {
    x2_.clear();
    x4_.clear();
    process_(input);
  }

  const Signal &sig_x2() const { return x2_; }
  const Signal &sig_x4() const { return x4_; }

  void finalize(const Signal &input, bool positive, 
                const Signal *param, Signal &res);


  virtual bool x2_prefix() const = 0;
  virtual bool x4_prefix() const = 0;

protected:
  virtual void process_(const Signal &input) = 0;

  Signal x2_;
  Signal x4_;
};

struct ADWTInterpolate : public ADWTSubSystem {
  ADWTInterpolate() {}
  void process_(const Signal &input);

  bool x2_prefix() const;
  bool x4_prefix() const;
};

struct ADWTUpdate : public ADWTSubSystem {
  ADWTUpdate() {}
  void process_(const Signal &input);  

  bool x2_prefix() const;
  bool x4_prefix() const;
};


#endif  // ADWT_ADWT_H
