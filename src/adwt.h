/*
  Copyright: (c) 2011 Matija Osrecki <matija.osrecki@fer.hr>
*/

#ifndef ADWT_ADWT_H
#define ADWT_ADWT_H

#include <vector>

class WindowCombiner;

typedef std::vector<double> signal;

enum GuessMode { NONE, GUESS_B, GUESS_C };

void adwt(signal &input, WindowCombiner *wcombiner, GuessMode adwt_mode,
          signal &res, signal &sig_a, signal &sig_d);

struct ADWTSubSystem {
  ADWTSubSystem() {}

  void process(const signal &input) {
    x2_.clear();
    x4_.clear();
    process_(input);
  }

  const signal &sig_x2() const { return x2_; }
  const signal &sig_x4() const { return x4_; }

  void finalize(const signal &input, bool positive, 
                const signal *param, signal &res);


  virtual bool x2_prefix() const = 0;
  virtual bool x4_prefix() const = 0;

protected:
  virtual void process_(const signal &input) = 0;

  signal x2_;
  signal x4_;
};

struct ADWTInterpolate : public ADWTSubSystem {
  ADWTInterpolate() {}
  void process_(const signal &input);

  bool x2_prefix() const;
  bool x4_prefix() const;
};

struct ADWTUpdate : public ADWTSubSystem {
  ADWTUpdate() {}
  void process_(const signal &input);  

  bool x2_prefix() const;
  bool x4_prefix() const;
};


#endif  // ADWT_ADWT_H
