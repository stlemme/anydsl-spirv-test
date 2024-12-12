
//#define CL_TARGET_OPENCL_VERSION 120
//#include <CL/opencl.h>
//#include <stdint.h>

typedef uint uint32_t;


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

__kernel void copy_krnl_lsm(__global const uint32_t* src, __global uint32_t* dst, __local uint32_t* lsm)
{
    size_t i = get_global_id(0);
    size_t j = get_local_id(0);

    //__local uint32_t lsm[32];

    lsm[j] = 0xDEADBEEF;

    barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);

    lsm[j] = src[i];

    barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);

    dst[i] = lsm[j];
}

