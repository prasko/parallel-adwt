/*
  Copyright: (c) 2011 Matija Osrecki <matija.osrecki@fer.hr>
*/

#ifndef ADWT_ADWT_H
#define ADWT_ADWT_H

#include <vector>

typedef std::vector<double> signal;

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

protected:
  virtual void process_(const signal &input) = 0;
  virtual bool x2_prefix() = 0;
  virtual bool x4_prefix() = 0;

  signal x2_;
  signal x4_;
};

struct ADWTInterpolate : public ADWTSubSystem {
  ADWTInterpolate() {}
  void process_(const signal &input);

private:
  bool x2_prefix();
  bool x4_prefix();
};

struct ADWTUpdate : public ADWTSubSystem {
  ADWTUpdate() {}
  void process_(const signal &input);  

private:
  bool x2_prefix();
  bool x4_prefix();
};


#endif  // ADWT_ADWT_H
