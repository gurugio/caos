#ifndef __PERCPU_H__
#define __PERCPU_H__




#define DEFINE_PER_CPU(type, name) \
	__attribute__((__section__(".data.percpu"))) __typeof__(type) per_cpu__##name




void setup_per_cpu_areas(void);


#endif
