/*
  Copyright: (c) 2011 Matija Osrecki <matija.osrecki@fer.hr>
*/

#include <cassert>
#include <algorithm>
#include <vector>

#include "denoise.h"

typedef float Decimal;

namespace Denoise {

  Denoiser::Result* CUDAICIDenoiser::denoise(const Signal &sig) {
    std::vector<Signal> multiple_sig;
    std::vector<Denoiser::Result*> multiple_res;

    multiple_sig.push_back(sig);
    denoiseMultiple(multiple_sig, multiple_res);
    return multiple.res[0];
  }

  void CUDAICIDenoiser::denoiseMultiple(const std::vector<Signal> &sig,
      std::vector<Denoiser::Result*> &res) {

    if(sig.empty()) return;

    // 1. init number of signals, signal width, and width in bytes

    const int n = (int)sig.size();
    int m = (int)sig[0].size();
    bool same_width = true;

    for(int i = 1; i < n; ++i)
      if((int)sig[i].size() > m) {
        same_width = false;
        m = sig[i];
      }
  
    const int width_bytes = m*sizeof(Decimal);
    const int size_bytes = 2*n*width_bytes;

    // 2. init input data on host and device

    Decimal *host_signals = new Decimal[2*n*m];

    // set all to 0 if they're not the same length
    if(!same_width) {
      memset(host_signals, 0, sizeof host_signals);
      // XXX maybe use forloop instead!!
      // for(int i = 0; i < 2*n*m; ++i) 
      //   host_signals[i] = 0.0;
    }

    for(int i = 0; i < n; ++i) {
      std::copy(sig[i].begin(), sig[i].end(), 
                host_signals+2*i*width);

      std::reverse_copy(sig[i].begin(), sig[i].end, 
                        host_signals+(2*i+1)*width+(m-(int)sig[i].size()));
    }

    Decimal *device_signals;
    cudaMalloc(&device_signals, size_bytes);
    cudaMemcpy(device_signals, host_signals, size_bytes, 
               cudaMemcpyHostToDevice);

    // 2. init result data

    Decimal *device_param;
    int *device_wsize;

    cudaMalloc(&device_param, size_bytes);
    cudaMalloc(&device_param, 2*n*sizeof(int));

    // 3. init kernel

    // 4. start kernel

    // 5. retrieve and post-calculate results

    Decimal *host_param = new Decimal[2*n*m];
    int *host_wsize = new int[2*n*m];

    cudaMemcpy(host_param, device_param, size_bytes, 
               cudaMemcpyDeviceToHost);

    cudaMemcpy(host_wsize, device_wsize, 2*n*sizeof(int), 
               cudaMemcpyDeviceToHost);

    res.resize(n);
    for(int i = 0; i < n; ++i) {
      Decimal pvleft, pvright;  // param value
      int wsleft, wsright;  // window size

      for(int pos = 0; pos < (int)sig[i].size(); ++pos) {
        pvright = host_param[2*i*m + pos];
        pvleft = host_param[(2*i+2)*m - (pos+1)]; // (2*i+1)*m + (m-pos-1)
        wsright = host_wsize[2*i*m + pos];
        wsleft = host_wsize[(2*i+2)*m - (pos+1)];
        
        res.back()->addLeft(wsleft);
        res.back()->addRight(wsright);
        res.back()->addDenoised((pvright * wsright + pvleft * wsleft) /
                                (wsleft + wsright));
      }
    }

    // 6. clean up

    delete[] host_signals;
    delete[] host_param;
    delete[] host_wsize;

    cudaFree(device_signals);
    cudaFree(device_param);
    cudaFree(device_wsize);
  }
                                      
}  // namespace
