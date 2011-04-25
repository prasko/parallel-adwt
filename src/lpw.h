/*
  Copyright: (c) 2011 Matija Osrecki <matija.osrecki@fer.hr>
 */

#ifndef ADWT_LPW_H
#define ADWT_LPW_H

#include <cassert>
#include <vector>

using std::vector;

/*
  Lpw is an interface for calculating parameter B such that Y is best
  approximated by B * X, for every Yi and Xi.

  Calculation is performed on different window positions and sizes,
  meaning that for windows position WP and size WS, the calculation
  will be performed on members of arrays X and Y that are around index
  WP, with a total of WS members included, and others ignored.
 */
class Lpw {
public:
  Lpw(const vector<double> &y, const vector<double> &x) : y_(y), x_(x) {
    assert(x.size() == y.size());
    n_ = x.size();
  }

  virtual double getCoef(const int wsize, const int wpos) = 0;
  int length() const { return n_; }
  
protected:
  int n_;
  const vector<double> &y_;
  const vector<double> &x_;
};

/*
  Calculates the parameter B such that Y is approximated by B * X, by
  minimizing the L2 distance of signals Y and B * X.

  Calculation of B for any window size or position is a O(1)
  operation, and is performed on each call of getCoef(wsize, wpos).
 */
class Lsw : public Lpw {
public:
  Lsw(const vector<double> &y, const vector<double> &x) : Lpw(y, x) { init(); }
  double getCoef(const int wsize, const int wpos);

protected:
  vector<double> xxacc_;
  vector<double> xyacc_;  

private:
  void init();
};


#endif  // ADWT_LPW_H
