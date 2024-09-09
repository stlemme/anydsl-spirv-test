cmake_minimum_required(VERSION 3.13.4 FATAL_ERROR)

project(AnyDSL-spirv-test)

set(PACKAGE_VERSION "0.3.9")
#set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "limited config" FORCE)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(AnyDSL_runtime CONFIG REQUIRED)

add_executable(spirv-copy copy.cpp)
target_include_directories(spirv-copy PRIVATE ${AnyDSL_runtime_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(spirv-copy PRIVATE runtime)


find_program(Clang_BIN NAMES clang PATHS ${LLVM_BINARY_DIR} PATH_SUFFIXES ${CMAKE_CONFIGURATION_TYPES})
find_program(LLVM_SPIRV_BIN NAMES llvm-spirv PATHS ${LLVM_BINARY_DIR} PATH_SUFFIXES ${CMAKE_CONFIGURATION_TYPES})

# add_custom_command(OUTPUT spv_kernels.spv
	# COMMAND ${Clang_BIN} --target=spirv64 -o spv_kernels.spv ${CMAKE_CURRENT_SOURCE_DIR}/spv_kernels.cl
    # MAIN_DEPENDENCY spv_kernels.spv)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/spv_kernels.spv
	COMMAND ${Clang_BIN} -cc1 -triple spirv64 -emit-llvm-bc -emit-llvm-uselists -main-file-name spv_kernels.cl -v -finclude-default-header -fdeclare-opencl-builtins -fgnuc-version=4.2.1 -fno-threadsafe-statics -fcolor-diagnostics -o ${CMAKE_CURRENT_BINARY_DIR}/spv_kernels.bc -x cl ${CMAKE_CURRENT_SOURCE_DIR}/spv_kernels.cl
	COMMAND ${LLVM_SPIRV_BIN} ${CMAKE_CURRENT_BINARY_DIR}/spv_kernels.bc -o ${CMAKE_CURRENT_BINARY_DIR}/spv_kernels.spv
	MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/spv_kernels.cl)

add_executable(spirv-copy-kernel copy-kernel.cpp ${CMAKE_CURRENT_BINARY_DIR}/spv_kernels.spv)
target_compile_definitions(spirv-copy-kernel PRIVATE KERNEL_PATH="${CMAKE_CURRENT_BINARY_DIR}")
target_include_directories(spirv-copy-kernel PRIVATE ${AnyDSL_runtime_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(spirv-copy-kernel PRIVATE runtime)
