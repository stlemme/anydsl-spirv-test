source /opt/intel/oneapi/setvars.sh

sycl-ls

sycl-ls --verbose | grep "ext_intel_matrix"

export NEO_CACHE_PERSISTENT=0
export IGC_ShaderDumpEnable=1
export IGC_ShaderDumpEnableAll=0
export IGC_DumpToCustomDir=$(pwd)/shader_dump

icpx -fsycl -fsycl-targets=spir64_gen -o joint-matrix -Xsycl-target-backend "-device dg2 -internal_options -ze-opt-large-register-file" joint-matrix.cpp

ONEAPI_DEVICE_SELECTOR=level_zero:1 ./joint-matrix

