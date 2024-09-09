
#include <anydsl_runtime.h>
#include <iostream>

int main(int argc, const char* argv) {

	anydsl_info();

	int32_t dev = ANYDSL_DEVICE(ANYDSL_LEVELZERO, 0);

	uint32_t n = 8192;

	void* p0 = anydsl_alloc_host(dev, n);
	void* p1 = anydsl_alloc(dev, n);
	void* p2 = anydsl_alloc_unified(dev, n);

	uint32_t* p = static_cast<uint32_t*>(p0);
	for (uint32_t i = 0; i < n; ++i) {
		p[i] = i;
	}

	anydsl_copy(dev, p0, 0, dev, p1, 0, n);

	anydsl_copy(dev, p1, 0, dev, p0, 0, n);

	for (uint32_t i = 0; i < 256; ++i) {
		std::cout << i << ": " << p[i] << std::endl;
	}

	anydsl_release(dev, p2);
	anydsl_release(dev, p1);
	anydsl_release_host(dev, p0);

	return 0;
}
