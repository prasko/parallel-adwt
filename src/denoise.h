/*
  Denoise module provides classes for denoising signals.
  
  Denoise::Denoiser class is the common interface for all denoiser classes.  It
  provides basic result class Denoise::Denoiser::Result which rembembers final
  denoised signal.

  Denoiser classes provide a way to denoise a single signal via method
  Denoiser::denoise(..) or multiple signals via method
  Denoiser::denoiseMultiple(..).

  Currently implemented is ICI denoising method in class Denoise::ICIDenosier,
  suitable for flat, rectangle-like signals.  

  Copyright: (c) 2011 Matija Osrecki <matija.osrecki@fer.hr>
*/

#ifndef ADWT_DENOISE_H
#define ADWT_DENOISE_H

#include <cassert>
#include <vector>
#include <utility>

typedef std::vector<double> Signal;

namespace Denoise {

  /* Denoising interface. */
  struct Denoiser {

    /* Denoising result superclass. Contains denoised signal. */
    struct Result {
      virtual ~Result() {}

      /* Appends denoised member to denoised signal. */
      void addDenoised(double member);

      /* Returns indexed denoised member. */
      double getDenoised(int index);

    protected:
      /* Denoised signal */
      Signal dns_signal; 
    };

    /* Performs denoising on a single signal. */
    virtual Result* denoise(const Signal &sig) = 0;

    /* Performs denoising on multiple signals. */
    virtual void denoiseMultiple(
      const std::vector<Signal> &input, std::vector<Result*> &res);
  };

  /* Denoiser for suitable for flat, rectangle-like signals, uses ICI method.*/
  struct ICIDenoiser : public Denoiser {
    
    /* Creates new ICI Denoiser based on ICI parameters. */
    ICIDenoiser(double gama=4.4, double rc=0.85, double sigma=0.2) : 
      Denoiser(), gama_(gama), rc_(rc), sigma_(sigma) {}

    /* 
       ICI result class, extends basic denoiser result class.  
       
       Remembers left and right window sizes, looking from current position.
    */
    struct Result : public Denoiser::Result {

      /* Add or return left or right window size values. */
      void addLeft(int interval);    
      int getLeft(int index); 
      void addRight(int interval);
      int getRight(int index);

      friend class ICIDenoiser;

    protected:
      /* Left and right window sizes. */
      std::vector<int> left_interval;
      std::vector<int> right_interval;
    };

    /* Denoising method. */
    virtual Denoiser::Result* denoise(const Signal &sig);

  protected:
    /* ICI parameters */
    const double gama_;
    const double rc_;
    const double sigma_;

  private:
    void denoise_(const Signal &sig, Signal &res, std::vector<int> &interval);
  };

  /* ICI denoising method implemented on CUDA arhitecture. */
  struct CUDAICIDenoiser : public ICIDenoiser {

    /* Creates new CUDA ICI Denoiser based on same ICI parameters. */
    CUDAICIDenoiser(double gama=4.4, double rc=0.85, double sigma=0.2) :
      ICIDenoiser(gama, rc, sigma) {}

    /* Denoising methods. */
    virtual Denoiser::Result* denoise(const Signal &sig);
    virtual void denoiseMultiple(
      const std::vector<Signal> &input, std::vector<Denoiser::Result*> &res);
  };

}  // namespace

#endif  // ADWT_DENOISE_H
