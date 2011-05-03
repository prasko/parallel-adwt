/*
  Copyright: (c) 2011 Matija Osrecki <matija.osrecki@fer.hr>
*/

#ifndef ADWT_COMBINE_H
#define ADWT_COMBINE_H

#include <cmath>
#include <vector>

#include "lpw.h"

typedef std::vector<double> signal;

class WindowCombiner {
public:
  WindowCombiner(Lpw &lpw) : lpw_(lpw) {}
  virtual void combine(std::vector<double> &result) = 0;

protected:
  Lpw &lpw_;
};

class DenoiserWindowCombiner : public WindowCombiner {
public:
  DenoiserWindowCombiner(Lpw &lpw) : WindowCombiner(lpw) {}
  void combiner(std::vector<double> &result);
};

class ICIWindowCombiner : public WindowCombiner {
public:
  ICIWindowCombiner(Lpw &lpw) : WindowCombiner(lpw) {}
  void combine(std::vector<double> &result);

private:
  void combine_(std::vector< std::pair<double, int> > &, int);
};

class SimpleWindowCombiner : public WindowCombiner {
public:

  class ConfidenceInterval {
  public:
    ConfidenceInterval(double base) : base_(base) {}
    bool inside(int pos, double value) { return fabs(value) < interval(pos); }
    virtual double interval(const int pos) { return base_; }

  protected:
    const double base_;
  };

  class TunnelInterval : public ConfidenceInterval {
  public:
    TunnelInterval(double upper, double lower, double rate) : 
      ConfidenceInterval(upper), lower_(lower), rate_(rate) {}
  
    virtual double interval(int pos) { 
      return std::max(lower_, base_ - pos * rate_ * (base_-lower_) );
    }

  protected:
    const double lower_;
    const double rate_;
  };

  SimpleWindowCombiner(Lpw &lpw, ConfidenceInterval &interval) : 
    WindowCombiner(lpw), interval_(interval) {}

  void combine(std::vector<double> &result);

private:
  ConfidenceInterval &interval_;
};

#endif  // ADWT_COMBINE_H
