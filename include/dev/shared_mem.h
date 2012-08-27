#ifndef DEV_SHARED_MEM_H_INCLUDED
#define DEV_SHARED_MEM_H_INCLUDED

rd_t open_shared_mem(void) __attribute__((section(".kernel")));

#endif
