
#include <anydsl_runtime.h>
#include <iostream>

int main(int argc, const char** argv) {

	anydsl_info();

	int32_t dev = ANYDSL_DEVICE(ANYDSL_LEVELZERO, 0);
	//int32_t cpu = ANYDSL_DEVICE(ANYDSL_HOST, 0);

	uint32_t n = 8192;
	uint32_t s = n * sizeof(uint32_t);

	void* p0 = anydsl_alloc_host(dev, s);
	//void* p0 = anydsl_alloc(cpu, s);
	void* p1 = anydsl_alloc(dev, s);
	void* p2 = anydsl_alloc_unified(dev, s);
	void* p3 = anydsl_alloc_host(dev, s);

	uint32_t* pp0 = static_cast<uint32_t*>(p0);
	uint32_t* pp3 = static_cast<uint32_t*>(p3);

	for (uint32_t i = 0; i < n; ++i) {
		pp0[i] = i;
		pp3[i] = 0x23;
	}

	anydsl_copy(dev, p0, 0, dev, p1, 0, s);

	anydsl_copy(dev, p1, 0, dev, p2, 0, s);

	anydsl_copy(dev, p2, 0, dev, p3, 0, s);

	anydsl_synchronize(dev);

	for (uint32_t i = 0; i < 256; ++i) {
		std::cout << i << ": " << pp3[i] << std::endl;
	}

	anydsl_release_host(dev, p3);
	anydsl_release(dev, p2);
	anydsl_release(dev, p1);
	anydsl_release_host(dev, p0);

	return 0;
}
