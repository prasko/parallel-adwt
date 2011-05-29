/*
  Copyright: (c) 2011 Matija Osrecki <matija.osrecki@fer.hr>
*/

#include <cassert>
#include <algorithm>
#include <vector>

#include "denoise.h"

typedef float Decimal;

using std::vector;

__global__ void denoise_kernel(const Decimal *signals, 
                               Decimal *param, 
                               Decimal *wsize,
                               const int n,
                               const int m,
                               const Denoise::CUDAICIDenoiser denoiser) {

  // init starting thread responsibility
  int signal_pos = blockIdx.x * blockDim.x * thread_jobs + threadIdx.x;
  const int signal_index = blockIdx.y * blockDim.y + threadIdx.y;

  if(signal_index >= n) return;

  // init variables
  Decimal sum, avg, tavg, curr_sigma, rk;
  Decimal maxlb, minub;

  // window size
  int wsize;

  for(int job = 0; job < thread_jobs; ++job) {
    if(signal_pos >= m) return;
    
    // init max upper and min lower bounds
    maxlb = -1e7;
    minub = +1e7;

    // increment wsize - window size
    for(wsize = 2; signal_pos+wsize-1 < m; ++wsize) {
      // calculate current sum, temp. average and sigma
      sum += signals[signal_index*m+signal_pos+wsize-1];
      tavg = sum / wsize;
      curr_sigma = denoiser.sigma / sqrt(wsize);

      // recalculate max upper and min lower bounds
      minub = std::min(minub, tavg + denoiser.gama * curr_sigma);
      maxlb = std::max(maxlb, tavg - denoiser.gama * curr_sigma);

      // calculate new RICI parameter Rk
      rk = (minub - maxlb) / (2 * gama * curr_sigma);

      // break if ICI or RICI conditions achieved
      if(minub < maxlb || rk < denoiser.rc) break;

      avg = tavg;
    }

    // add average and window size to solution
    param[signal_index*m+signal_pos] = avg;
    wsize[signal_index*m+signal_pos] = wsize-1;

    signal_pos += blockDim.x;
  }
}

namespace Denoise {

  Denoiser::Result* CUDAICIDenoiser::denoise(const Signal &sig) {
    std::vector<Signal> multiple_sig;
    std::vector<Denoiser::Result*> multiple_res;

    multiple_sig.push_back(sig);
    denoiseMultiple(multiple_sig, multiple_res);
    return multiple.res[0];
  }

  void CUDAICIDenoiser::denoiseMultiple(const vector<Signal> &sig,
                                        vector<Denoiser::Result*> &res) {

    assert(res.empty());

    if(sig.empty()) {
      return;
    }

    // 1. init number of signals, signal width, and width in bytes

    const int n = (int)sig.size();
    int m = (int)sig[0].size();
    bool same_width = true;

    for(int i = 1; i < n; ++i) {
      if((int)sig[i].size() > m) {
        same_width = false;
        m = sig[i];
      }
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

      std::reverse_copy(sig[i].begin(), sig[i].end(), 
                        host_signals+(2*i+2)*m-(int)sig[i].size());
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

    const int block_size = 16;
    const int thread_spread = 32;

    const int grid_width = (n-1) / (block_size_ * thread_jobs_) + 1;
    const int grid_height = (n-1) / block_size_ + 1;

    dim3 dim_block(block_size, block_size);
    dim3 dim_grid(grid_width, grid_height);

    // 4. start kernel

    denoise_kernel<<<dim_grid, dim_block>>>(device_signals, 
                                            device_param,
                                            device_wsize, 
                                            n,
                                            m,
                                            *this);

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
