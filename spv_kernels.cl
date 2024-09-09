
//#define CL_TARGET_OPENCL_VERSION 120
//#include <CL/opencl.h>
#include <stdint.h>


__kernel void copy_krnl(__global const uint32_t* src, __global uint32_t* dst)
{
    size_t i = get_global_id(0);
    dst[i] = src[i];
}

__kernel void square_krnl(__global uint32_t* a, __global uint32_t* b)
{
    size_t i = get_global_id(0);
    a[i] = a[i]*a[i];
}
