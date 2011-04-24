/*
  Copyright: (c) 2011 Matija Osrecki <matija.osrecki@fer.hr>
 */

#ifndef ADWT_LPW_H
#define ADWT_LPW_H

#include <cassert>
#include <vector>

using std::vector;

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

/*
  Takes two equaly sized vectors Y and X, and 
  calculates coefficients for all windows sizes and positions
  using least squared approximation.

  If N is the size of vector X, resulting matrix will have
  N-1 rows (sizes) and N columns (positions).

  mat[i][j] represents coef. for windows size 2*i + 3 centered at position j.

  TODO: change double* with a decent class Matrix
*/
double* lswc(const vector<double> &y, const vector<double> &x);

#endif  // ADWT_LPW_H
