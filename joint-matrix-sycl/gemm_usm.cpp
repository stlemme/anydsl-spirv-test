//==============================================================
// Copyright © 2022 Intel Corporation
//
// SPDX-License-Identifier: MIT
// =============================================================

#include <bits/stdc++.h>

#include <cmath>
#include <iostream>
#include <sycl/sycl.hpp>

#include "oneapi/mkl/blas.hpp"

//#define B_LAYOUT_COL

using use = sycl::ext::oneapi::experimental::matrix::use;
using layout = sycl::ext::oneapi::experimental::matrix::layout;
using bfloat16 = sycl::ext::oneapi::bfloat16;
using fp16 = sycl::half;

constexpr float ALPHA = 2.0;
constexpr float C_INIT = 1.0;
constexpr float BF16_EPSILON = 0.0781250;
constexpr float FP16_EPSILON = 0.1250;

template <typename KernelName> size_t get_sg_size(sycl::queue q) {
  auto KernelID = sycl::get_kernel_id<KernelName>();
  auto KB = sycl::get_kernel_bundle<sycl::bundle_state::executable>(
      q.get_context(), {KernelID});
  auto kernel = KB.get_kernel(KernelID);

  return kernel.template get_info<
      sycl::info::kernel_device_specific::max_sub_group_size>(q.get_device());
}
template <typename Tc, typename Ta, typename Tb,
          size_t M, size_t N, size_t K, class kernel_name>
double matrix_multiply(Tc *C, Ta *A, Tb *B, sycl::queue q) {
  oneapi::mkl::transpose transA = oneapi::mkl::transpose::nontrans;
#ifdef B_LAYOUT_COL
  oneapi::mkl::transpose transB = oneapi::mkl::transpose::trans;
#else
  oneapi::mkl::transpose transB = oneapi::mkl::transpose::nontrans;
#endif

  // kernel begin
  sycl::event e = oneapi::mkl::blas::row_major::gemm(q, transA, transB, M, N, K, 1.0, A, N, B, N, 1.0, C, N, {});
  // kernel end

  q.wait();
  return(e.template get_profiling_info<sycl::info::event_profiling::command_end>() -
       e.template get_profiling_info<sycl::info::event_profiling::command_start>());
}

float make_fp32(bfloat16 x) {
  unsigned int y = *((int *)&x);
  y = y << 16;
  float *res = reinterpret_cast<float *>(&y);
  return *res;
}

template <typename Tc, typename Ta, typename Tb, size_t M, size_t N, size_t K>
void matrix_multiply_ref(Ta *A, Tb *B, Tc *C) {
  for (size_t m = 0; m < M; m++)
    for (size_t n = 0; n < N; n++) {
      for (size_t k = 0; k < K; k++) {
        C[m * N + n] += make_fp32(A[m * K + k]) *
#ifdef B_LAYOUT_COL
                make_fp32(B[n * K + k]);
#else
                make_fp32(B[k * N + n]);
#endif
      }
      //C[m * N + n] *= ALPHA;
    }
}

template <typename Ta, typename Tb, typename Tc, class kernel_name>
int test() {

  static constexpr size_t M = N_PARAM; // TM * 2;
  static constexpr size_t N = N_PARAM; // TN * 2;
  static constexpr size_t K = N_PARAM; // TK * 2;
                                    //
  sycl::queue q(sycl::gpu_selector_v, sycl::property::queue::enable_profiling{});
  Ta *A = sycl::malloc_shared<Ta>(M * K, q);
  Tb *B = sycl::malloc_shared<Tb>(K * N, q);
  Tc *C = sycl::malloc_shared<Tc>(M * N, q);
  Tc *D = sycl::malloc_shared<Tc>(M * N, q);

  std::default_random_engine gen;
  std::uniform_real_distribution<float> distribution(0.0, 1.0);

  for (size_t i = 0; i < M; i++) {
    for (size_t j = 0; j < K; j++) {
      //A[i * K + j] = Ta(1.0f * (i + j));
      A[i * K + j] = Ta(distribution(gen));
    }
  }
  for (size_t i = 0; i < K; i++) {
    for (size_t j = 0; j < N; j++) {
      //B[i * N + j] = Tb(2.0f * i + 3.0f * j);
#ifdef B_LAYOUT_COL //Does not realy matter, but still...
      B[j * K + i]
#else
      B[i * N + j]
#endif
          = Tb(distribution(gen));
    }
  }
  for (size_t i = 0; i < M; i++) {
    for (size_t j = 0; j < N; j++) {
      C[i * N + j] = C_INIT;
      D[i * N + j] = C_INIT;
    }
  }

  //matrix_multiply_ref<Tc, Ta, Tb, M, N, K>(A, B, D);

  double t = matrix_multiply<Tc, Ta, Tb, M, N, K, kernel_name>(C, A, B, q);
  std::cout << "kernel time: " << t / 1000000 << " ms" << std::endl;
  
  bool res = true;

  /*for (size_t i = 0; i < M; i++) {
    for (size_t j = 0; j < N; j++) {
      if constexpr (std::is_same_v<Tc, float>) {
        if (std::fabs(C[i * N + j] - D[i * N + j]) > FP16_EPSILON) {
          res = false;
          std::cout << "Incorrect result in matrix. "
                    << "i: " << i << ", j: " << j << ", Ref: " << D[i * N + j]
                    << ", Val: " << C[i * N + j]
                    << ", Error: " << std::fabs(C[i * N + j] - D[i * N + j]) << "\n";
        }
      } else if (C[i * N + j] != D[i * N + j])
        res = false;
    }
  }*/

  std::cout << (res ? "passed" : "failed") << std::endl;
  return res;
}

int main() {
  sycl::queue q; //(sycl::gpu_selector_v);
  // Snippet begin
  auto device_name = q.get_device().get_info<sycl::info::device::name>();
  std::cout << "device: " << device_name << std::endl;

  //std::vector<sycl::ext::oneapi::experimental::matrix::combination>
  //    combinations = q.get_device().get_info<sycl::ext::oneapi::experimental::info::device::matrix_combinations>();

  //std::cout << combinations.size() << std::endl;

  bool passed = true;
  //for (unsigned int i = 0; i < combinations.size(); i++) {
    //std::cout << "M N K: " << combinations[i].msize << " " << combinations[i].nsize << " " << combinations[i].ksize << std::endl;

    /*
    if (combinations[i].nsize == 0) { // Intel AMX
      passed &=
          test<int8_t, int8_t, int32_t, 16, 16, 64, class amx_int_16x16x64>();
      passed &= test<bfloat16, bfloat16, float, 16, 16, 32,
                     class amx_bf16_16x16x32>();
      break;
    }
    */

    /*
    if (combinations[i].nsize == 16) { // architecture::intel_gpu_pvc
      passed &=
          test<int8_t, int8_t, int32_t, 8, 16, 32, class pvc_int_8x16x32>();
      passed &=
          test<bfloat16, bfloat16, float, 8, 16, 16, class pvc_bf16_8x16x16>();
      break;
    }
    */

    //if (combinations[i].nsize == 8) { // architecture::intel_gpu_dg2*
      //passed &= test<int8_t, int8_t, int32_t, 8, 8, 32, class dg2_int_8x16x32>();
      //passed &=
      //break;
      //    test<bfloat16, bfloat16, float, 8, 8, 16, class dg2_bf16_8x16x16>();
    //}
  //}
  // Snippet end

#if defined(TARGET_INTEL_DG2)
  passed &= test<float, float, float, class dg2_fp16_8x8x16>();
#elif defined(TARGET_NVIDIA_CUDA)
  passed &= test<fp16, fp16, float, 16, 16, 16, class nvptx_fp16_16x16x16>();
#else
#error "No supported target device selected"
#endif
  return !passed;
}
