/*
  Copyright: (c) 2011 Matija Osrecki <matija.osrecki@fer.hr>
*/

#include <vector>

#include "combine.h"

// Floating point type to be used on the device.
typedef float dftype; 

void CUDACombiner::combine(std::vector<double> &res) {
  const int n = (int)x_.size();
  const int typesize = sizeof(dftype);

  // calculate Xt * X and Xt * Y
  dftype *hxx = new dftype[n+1];
  dftype *hxy = new dftype[n+1];

  hxx[0] = hxy[0] = 0.0;

  for(int i = 0; i < n; ++i) {
    hxx[i+1] = hxx[i] + x_[i] * x_[i];
    hxy[i+1] = hxy[i] + x_[i] * y_[i];
  }

  // allocate device memory and copy data

  dftype *dxx, *dxy, *dres;

  cudaMalloc(&dxx, (n+1) * typesize);
  cudaMalloc(&dxy, (n+1) * typesize);
  cudaMalloc(&dres, n * typesize);

  cudaMemcpy(dxx, hxx, (n+1) * typesize, cudaMemcpyHostToDevice);
  cudaMemcpy(dxy, hxy, (n+1) * typesize, cudaMemcpyHostToDevice);

  // TODO(matija): init kernel

  // TODO(matija): start kernel

  // retrieve results
  dftype *hres = new dftype[n];
  cudaMemcpy(hres, dres, n * typesize, cudaMemcpyDeviceToHost);

  res.resize(n);
  copy(hres, hres+n, res, res+n);

  // free memory
  delete[] hxx;
  delete[] hxy;
  delete[] hres;

  cudaFree(dxx);
  cudaFree(dxy);
  cudaFree(dres);
}
