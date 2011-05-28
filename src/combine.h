/*
  Copyright: (c) 2011 Matija Osrecki <matija.osrecki@fer.hr>
*/

#ifndef ADWT_COMBINE_H
#define ADWT_COMBINE_H

#include <cmath>
#include <vector>

#include "lpw.h"

typedef std::vector<double> signal;

namespace Denoise {
  class ICIDenoiser;
}

namespace Combine {

  class WindowCombiner {
  public:
    WindowCombiner() {}
    virtual void combine(std::vector<double> &result) = 0;
    virtual void setLpwData(const signal &y, const signal &x) = 0;
  };

  class CUDACombiner : public WindowCombiner {
  public:
    CUDACombiner() {}

    void combine(std::vector<double> &result);

    void setLpwData(const signal &y, const signal &x) {
      x_ = &x;
      y_ = &y;
    }
  
  private:
    const signal *y_;
    const signal *x_;
  };

  class LpwCombiner : public WindowCombiner {
  public:
    LpwCombiner(Lpw &lpw) : lpw_(lpw) {}

    void setLpwData(const signal &y, const signal &x) {
      lpw_.setLpwData(y, x);
    }

  protected:
    Lpw &lpw_;
  };

  class DenoiserWindowCombiner : public LpwCombiner {
  public:
    DenoiserWindowCombiner(Lpw &lpw, Denoise::ICIDenoiser &denoiser) : 
      LpwCombiner(lpw), denoiser_(denoiser) {}

    void combine(std::vector<double> &result);

  private:
    Denoise::ICIDenoiser &denoiser_;
  };

  class ICIWindowCombiner : public LpwCombiner {
  public:
    ICIWindowCombiner(Lpw &lpw, double gama, double rc, double sigma, bool rici): 
      LpwCombiner(lpw), gama_(gama), rc_(rc), sigma_(sigma), rici_(rici) {}

    void combine(std::vector<double> &result);

  private:
    void combine_(std::vector< std::pair<double, int> > &, int);

    const double gama_;
    const double rc_;
    const double sigma_;
  
    const bool rici_;
  };

  class SimpleWindowCombiner : public LpwCombiner {
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
      LpwCombiner(lpw), interval_(interval) {}

    void combine(std::vector<double> &result);

  private:
    ConfidenceInterval &interval_;
  };

}  // namespace

#endif  // ADWT_COMBINE_H
