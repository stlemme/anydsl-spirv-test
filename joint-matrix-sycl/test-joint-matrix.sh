#source /opt/intel/oneapi/setvars.sh

#sycl-ls

#sycl-ls --verbose | grep "ext_intel_matrix"

#export NEO_CACHE_PERSISTENT=0
#export IGC_ShaderDumpEnable=1
#export IGC_ShaderDumpEnableAll=0
#export IGC_DumpToCustomDir=$(pwd)/shader_dump

#for N in {64,128,256,512,768,1024,1280,1536,1792,2048,2560,3072,3584,4096,4608,5120,5632,6144,6656,7168,7680,8192}; do
#for N in {128,256,512,1024,2048,4096,8192}; do
for N in 4096; do
    icpx -fsycl -fsycl-targets=spir64_gen -DTARGET_INTEL_DG2 -o joint-matrix-${N} -Xsycl-target-backend "-device dg2 -internal_options -ze-opt-large-register-file" joint-matrix.cpp -DN_PARAM=${N}

    ONEAPI_DEVICE_SELECTOR=level_zero:1 ./joint-matrix-${N}
    ONEAPI_DEVICE_SELECTOR=level_zero:1 ./joint-matrix-${N}
    ONEAPI_DEVICE_SELECTOR=level_zero:1 ./joint-matrix-${N}
    ONEAPI_DEVICE_SELECTOR=level_zero:1 ./joint-matrix-${N}
    ONEAPI_DEVICE_SELECTOR=level_zero:1 ./joint-matrix-${N}
    ONEAPI_DEVICE_SELECTOR=level_zero:1 ./joint-matrix-${N}
    ONEAPI_DEVICE_SELECTOR=level_zero:1 ./joint-matrix-${N}
    ONEAPI_DEVICE_SELECTOR=level_zero:1 ./joint-matrix-${N}
    ONEAPI_DEVICE_SELECTOR=level_zero:1 ./joint-matrix-${N}
    ONEAPI_DEVICE_SELECTOR=level_zero:1 ./joint-matrix-${N}
    ONEAPI_DEVICE_SELECTOR=level_zero:1 ./joint-matrix-${N}
    ONEAPI_DEVICE_SELECTOR=level_zero:1 ./joint-matrix-${N}
    ONEAPI_DEVICE_SELECTOR=level_zero:1 ./joint-matrix-${N}
    ONEAPI_DEVICE_SELECTOR=level_zero:1 ./joint-matrix-${N}
    ONEAPI_DEVICE_SELECTOR=level_zero:1 ./joint-matrix-${N}
    ONEAPI_DEVICE_SELECTOR=level_zero:1 ./joint-matrix-${N}
    ONEAPI_DEVICE_SELECTOR=level_zero:1 ./joint-matrix-${N}
    ONEAPI_DEVICE_SELECTOR=level_zero:1 ./joint-matrix-${N}
    ONEAPI_DEVICE_SELECTOR=level_zero:1 ./joint-matrix-${N}
    ONEAPI_DEVICE_SELECTOR=level_zero:1 ./joint-matrix-${N}
    ONEAPI_DEVICE_SELECTOR=level_zero:1 ./joint-matrix-${N}
    ONEAPI_DEVICE_SELECTOR=level_zero:1 ./joint-matrix-${N}
    ONEAPI_DEVICE_SELECTOR=level_zero:1 ./joint-matrix-${N}
    ONEAPI_DEVICE_SELECTOR=level_zero:1 ./joint-matrix-${N}
    ONEAPI_DEVICE_SELECTOR=level_zero:1 ./joint-matrix-${N}
done
