/*
  Copyright: (c) 2011 Matija Osrecki <matija.osrecki@fer.hr>
 */

#ifndef ADWT_SYSUTIL_H
#define ADWT_SYSUTIL_H

#include <vector>
#include <iostream>

typedef std::vector<double> signal;

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
  void decimate(const signal &input, signal &res_even, signal &res_odd);
};

class PolySystem : public System {
public:  
  PolySystem(const double constant=1.0) : System(constant) {}
  void addMember(const int offset, const double coef);
  virtual void process(const signal &input, signal &output);

private:
  std::vector< std::pair<int, double> > members_;
};

class Multiply2 : public System {
public:
  Multiply2(const double constant=1.0) : System(constant) {}
  void multiply(const signal &s1, const signal &s2, signal &res);
};

class Add2 : public System {
public:
  Add2(const double constant=1.0) : System(constant) {}
  void add(const signal &s1, const signal &s2, const double c2, signal &res);
};

class Sumator {
public:
  Sumator(signal &result) : result_(&result) {}
  void init(signal &result);
  void sumSignal(const signal &signal, const double coef);

private:
  signal *result_;
};


#endif  // ADWT_SYSUTIL_H
