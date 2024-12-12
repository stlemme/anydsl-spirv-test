
#include <anydsl_runtime.h>
#include <iostream>

int main(int argc, const char** argv) {

	anydsl_info();

	int32_t dev = ANYDSL_DEVICE(ANYDSL_LEVELZERO, 0);

	uint32_t n = 8192;
	uint32_t s = n * sizeof(uint32_t);

	void* p0 = anydsl_alloc_host(dev, s);
 	void* p1 = anydsl_alloc(dev, s);
	//void* p2 = anydsl_alloc_unified(dev, s);
	void* p3 = anydsl_alloc(dev, s);
	void* p4 = anydsl_alloc_host(dev, s);

	uint32_t* pp0 = static_cast<uint32_t*>(p0);
	uint32_t* pp4 = static_cast<uint32_t*>(p4);

	for (uint32_t i = 0; i < n; ++i) {
		pp0[i] = i;
		pp4[i] = 0x23;
	}

	anydsl_copy(dev, p0, 0, dev, p1, 0, s);

        //for (uint32_t i = 0; i < n; ++i) {
        //        p[i] = 0x0;
        //}

	const char filename[] = KERNEL_PATH "/spv_kernels.spv";
	const char kernelname[] = "copy_krnl_lsm";

	std::cout << "Load kernels from " << filename << std::endl;

	uint32_t bx = 32;
	uint32_t grid[3] = { n, 1, 1 };
	uint32_t block[3] = { bx, 1, 1 };

	const uint32_t num_args = 3;
	void* arg_data[num_args] = { &p1, &p3, NULL };
	const uint32_t lsm_size = bx*sizeof(uint32_t);
	const uint32_t arg_sizes[num_args] = { sizeof(uint32_t*), sizeof(uint32_t*), lsm_size };
	const uint32_t arg_aligns[num_args] = { 0, 0, 0 };
	const uint32_t arg_alloc_sizes[num_args] = { sizeof(uint32_t*), sizeof(uint32_t*), lsm_size };
	const uint8_t arg_types[num_args] = { 0, 0, 0 }; // KernelArgType::Val;

	anydsl_launch_kernel(dev, filename, kernelname, grid, block, arg_data, arg_sizes, arg_aligns, arg_alloc_sizes, arg_types, num_args);

	anydsl_copy(dev, p3, 0, dev, p4, 0, s);

	anydsl_synchronize(dev);

	for (uint32_t i = 0; i < 256; ++i) {
		std::cout << i << ": " << pp4[i] << std::endl;
	}

	//anydsl_synchronize(dev);

	anydsl_release(dev, p3);
	//anydsl_release(dev, p2);
	anydsl_release(dev, p1);
	anydsl_release_host(dev, p4);
	anydsl_release_host(dev, p0);

	return 0;
}
