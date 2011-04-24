/*
  Copyright: (c) 2011 Matija Osrecki <matija.osrecki@fer.hr>
 */

#include <cassert>
#include <cstdio>
#include <vector>

#include "lpw.h"

using std::vector;

/*
  Copies vector to regular array.
*/
float* copy_vector(const vecd &v) {
  int n = (int)v.size();
  float *va = new float[n];

  for(int i = 0; i < n; ++i) 
    va[i] = (float)v[i];

  return va;
}

/*
  Takes two arrays of sizes N, and calculates for every i = [1,n]:
    result[i] = result[i-1] + a1[i-1] * a2[a-1], result[0] = 0

  The resulting array is accumulated sum of multiplied array a1 and a2.
*/
float* multiply(const float *a1, const float *a2, const int size) {
  float *res = new float[size+1];

  res[0] = 0.0f;
  for(int i = 0; i < size; ++i) 
    res[i+1] = res[i] + a1[i] * a2[i];

  return res;
}

/*
  Allocates memory of size 4 * n bytes on device and copies input array to device.
*/
float* alloc_cpy(const float *src, const int n, const cudaMemcpyKind type) {
  float *dest;

  cudaMalloc(&dest, n * sizeof(float));
  cudaMemcpy(dest, src, n * sizeof(float), type);

  return dest;
}


/*
  Function for device excecution. 
  Calculates B and E matrix members.
 */
__global__ void lsw_kernel(const float *dx, const float *dy,
			   const float *dxx, const float *dxy,
			   float *db, float *derr,
			   const int n, const int tspread) {

  int wsize = blockIdx.y * blockDim.y + threadIdx.y;
  int wpos = blockIdx.x * blockDim.x * tspread + threadIdx.x;

  if(wsize >= n-1) return;

  for(int i = 0; i < tspread; ++i) {
    if(wpos + wsize + 2 >= n+1) return;
    
    float b = (dxy[wpos + wsize + 2] - dxy[wpos]) / (dxx[wpos + wsize + 2] - dxx[wpos]);
    float err = 0.0f, delta;

    for(int j = wpos; j < wpos + wsize + 2; ++j) {
      delta = dy[j] - b * dx[j];
      err += delta * delta;
    }

    db[wsize * (n-1) + wpos] = b;
    derr[wsize * (n-1) + wpos] = err;

    wpos += blockDim.x;
  }
}

void lsw(const vecd &y, const vecd &x, lpw_res &result) {
  assert(x.size() == y.size());
  
  int n = (int)x.size();

  // copy data from vectors
  float *hy = copy_vector(y);
  float *hx = copy_vector(x);

  // calculate Xt * X and Xt * Y
  float *hxx = multiply(hx, hx, n);
  float *hxy = multiply(hx, hy, n);

  // allocate device memory and copy data
  float *dy = alloc_cpy(hy, n, cudaMemcpyHostToDevice);
  float *dx = alloc_cpy(hx, n, cudaMemcpyHostToDevice);
  float *dxx = alloc_cpy(hxx, n+1, cudaMemcpyHostToDevice);
  float *dxy = alloc_cpy(hxy, n+1, cudaMemcpyHostToDevice);

  // allocate device memory for resulting B matrix
  float *db, *derr;
  
  int width = (n-1) * sizeof(float);

  cudaMalloc(&db, width * (n-1));
  cudaMalloc(&derr, width *(n-1));

  // init kernel
  const int block_size = 16;
  const int thread_spread = 64;

  const int grid_width = (n-2) / (block_size*thread_spread) + 1;
  const int grid_height = (n-2) / block_size + 1;

  dim3 dim_block(block_size, block_size);
  dim3 dim_grid(grid_width, grid_height); 

  // start kernel that computes B matrix
  lsw_kernel<<<dim_grid, dim_block>>>(dx, dy, dxx, dxy, db, derr, n, thread_spread);

  // retrieve results
  float *hb = new float[(n-1) * (n-1)];
  cudaMemcpy(hb, db, width * (n-1), cudaMemcpyDeviceToHost);

  float *herr = new float[(n-1) * (n-1)];
  cudaMemcpy(herr, derr, width * (n-1), cudaMemcpyDeviceToHost);

  // store results to vector
  result.resize(n-1, std::vector<pdd> (n-1));
  for(int wsize = 0; wsize < n-1; ++wsize)
    for(int wpos = 0; wpos < n-1; ++wpos) {
      result[wsize][wpos].first = hb[wsize * (n-1) + wpos];
      result[wsize][wpos].second = herr[wsize * (n-1) + wpos];
    }

  // free memory
  delete[] hx;
  delete[] hy;
  delete[] hxx;
  delete[] hxy;
  delete[] hb;
  delete[] herr;

  cudaFree(dx);
  cudaFree(dy);
  cudaFree(dxx);
  cudaFree(dxy);
  cudaFree(db);
  cudaFree(derr);
}

