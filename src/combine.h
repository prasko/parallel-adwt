#ifndef ADWT_COMBINE_H
#define ADWT_COMBINE_H

#include "lpw.h"

#include <vector>

class WindowCombiner {
public:
  WindowCombiner(Lpw &lpw) : lpw_(lpw) {}
  virtual void combine(std::vector<double> &result) = 0;

protected:
  Lpw &lpw_;
};

class SimpleWindowCombiner : public WindowCombiner {
public:
  SimpleWindowCombiner(Lpw &lpw) : WindowCombiner(lpw) {}
  void combine(std::vector<double> &result);
};

#endif  // ADWT_COMBINE_H
