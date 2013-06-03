#ifndef DEV_HW_PERFCOUNTER_H_INCLUDED
#define DEV_HW_PERFCOUNTER_H_INCLUDED

void init_perfcounter(void) __attribute__((section(".kernel")));
uint64_t perfcounter_getcount(void);

#endif
