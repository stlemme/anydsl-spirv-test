source /opt/intel/oneapi/setvars.sh

sycl-ls

sycl-ls --verbose | grep "ext_intel_matrix"

export NEO_CACHE_PERSISTENT=0
export IGC_ShaderDumpEnable=1
export IGC_ShaderDumpEnableAll=0
export IGC_DumpToCustomDir=$(pwd)/shader_dump

export ROCM_PATH=/opt/rocm


# build benchmarks

# Level Zero
icpx -fsycl -fsycl-targets=spir64_gen -DTARGET_INTEL_DG2 -o joint-matrix-intel -Xsycl-target-backend "-device dg2 -internal_options -ze-opt-large-register-file" joint-matrix.cpp
# CUDA
icpx -fsycl -fsycl-targets=nvidia_gpu_sm_80 -DTARGET_NVIDIA_CUDA -o joint-matrix-nvidia joint-matrix.cpp
# HIP + rocm
#icpx -fsycl -fsycl-targets=amd_gpu_gfx1100 -DTARGET_AMD_RDNA3 --rocm-path=$ROCM_PATH -o joint-matrix-amd joint-matrix.cpp


# run benchmark

ONEAPI_DEVICE_SELECTOR=level_zero:1 ./joint-matrix-intel

