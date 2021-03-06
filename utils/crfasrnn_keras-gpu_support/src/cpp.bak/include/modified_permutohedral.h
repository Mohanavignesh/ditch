/*
   This file contains a modified version of the "permutohedral.h" code
   available at http://graphics.stanford.edu/projects/drf/. Copyright notice of
   the original file is included below:
    Copyright (c) 2013, Philipp Krähenbühl
    All rights reserved.
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
        * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
        * Neither the name of the Stanford University nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.
    THIS SOFTWARE IS PROVIDED BY Philipp Krähenbühl ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL Philipp Krähenbühl BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MODIFIED_PERMUTOHEDRAL_HPP_
#define MODIFIED_PERMUTOHEDRAL_HPP_

#include <cstdlib>
#include <vector>
#include <cstring>
#include <cassert>
#include <cstdio>
#include <cmath>

#include "tensorflow/core/framework/tensor.h"
#if __CUDACC__
#include "hash_table.h" //this should not be compiled by nvcc
#endif
using namespace tensorflow;

/************************************************/
/***          ModifiedPermutohedral Lattice   ***/
/************************************************/

typedef struct MatrixEntry {
  int index;
  float weight;
} MatrixEntry;


class ModifiedPermutohedral {
protected:
  struct Neighbors {
    int n1, n2;

    Neighbors(int n1 = 0, int n2 = 0) : n1(n1), n2(n2) {
    }
  };

  std::vector<int> offset_, rank_;
  std::vector<float> barycentric_;
  std::vector<Neighbors> blur_neighbors_;
  #if __CUDACC__
  bool is_init;
  MatrixEntry * matrix;
  HashTable table;
  #endif
  // Number of elements, size of sparse discretized space, dimension of features
  int N_, M_, d_, w_, h_;

  void sseCompute(Tensor &out, const Tensor &in, int value_size,
                  bool reverse = false, bool add = false) const;

  void seqCompute(Tensor &out, const Tensor &in, int value_size,
                  bool reverse = false, bool add = false) const;


public:
  ModifiedPermutohedral() : N_( 0 ), M_( 0 ), d_( 0 ) {}
  ~ModifiedPermutohedral() {}

  #if __CUDACC__
  void freeMatrix(){
    if (is_init) {
      CUDA_CHECK(cudaFree(matrix));
    }
  }
  #endif

  void init_cpu(const float *features, int num_dimensions, int num_points);
  void init_gpu(const float* features, int num_dimensions, int w, int h);

  void compute_cpu(Tensor &out, const Tensor &in, int value_size,
               bool reverse = false, bool add = false) const;
  void compute_gpu(float* out, const float* in, int value_size,
               bool reverse = false, bool add = false) const;
};

#endif //_MODIFIED_PERMUTOHEDRAL_HPP_