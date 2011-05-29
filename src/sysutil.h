/*
  Copyright: (c) 2011 Matija Osrecki <matija.osrecki@fer.hr>
 */

#ifndef ADWT_SYSUTIL_H
#define ADWT_SYSUTIL_H

#include <vector>
#include <iostream>

typedef std::vector<double> Signal;

class System {
public:
  System(const double constant=1.0) : constant_(constant) {}
  void setConstant(const double constant) { constant_ = constant; }

protected:
  double constant_;
};

class Decimator : public System {
public:  
  Decimator(const double constant=1.0) : System(constant) {}
  void decimate(const Signal &input, Signal &res_even, Signal &res_odd);
};

class PolySystem : public System {
public:  
  PolySystem(const double constant=1.0) : System(constant) {}
  void addMember(const int offset, const double coef);
  virtual void process(const Signal &input, Signal &output);

private:
  std::vector< std::pair<int, double> > members_;
};

class Multiply2 : public System {
public:
  Multiply2(const double constant) : System(constant) {}
  Multiply2(const bool constant=true) : System(constant ? 1.0 : -1.0) {}

  void multiply(const Signal &s1, const Signal &s2, Signal &res);
};

class Add2 : public System {
public:
  Add2(const double constant=1.0) : System(constant) {}
  void add(const Signal &s1, const Signal &s2, const double c2, Signal &res);
};

class Sumator {
public:
  Sumator(Signal &result) : result_(&result) {}
  void init(Signal &result);
  void sumSignal(const Signal &signal, const double coef);
  void sumSignal(const Signal &signal, const bool prefix);
  void sumMultiply(const Signal &s1, const Signal &s2, const bool prefix);

private:
  Signal *result_;
};


#endif  // ADWT_SYSUTIL_H
