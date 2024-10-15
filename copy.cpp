
#include <anydsl_runtime.h>
#include <iostream>

int main(int argc, const char** argv) {

	anydsl_info();

	int32_t dev = ANYDSL_DEVICE(ANYDSL_LEVELZERO, 0);
	int32_t cpu = ANYDSL_DEVICE(ANYDSL_HOST, 0);

	uint32_t n = 8192;
	uint32_t s = n * sizeof(uint32_t);

	void* p0 = anydsl_alloc_host(dev, s);
	//void* p0 = anydsl_alloc(cpu, s);
	void* p1 = anydsl_alloc(dev, s);
	void* p2 = anydsl_alloc_unified(dev, s);

	uint32_t* p = static_cast<uint32_t*>(p0);
	for (uint32_t i = 0; i < n; ++i) {
		p[i] = i;
	}

	anydsl_copy(cpu, p0, 0, dev, p1, 0, s);

	anydsl_copy(dev, p1, 0, cpu, p0, 0, s);

	for (uint32_t i = 0; i < 256; ++i) {
		std::cout << i << ": " << p[i] << std::endl;
	}

	anydsl_release(dev, p2);
	anydsl_release(dev, p1);
	anydsl_release_host(dev, p0);

	return 0;
}
