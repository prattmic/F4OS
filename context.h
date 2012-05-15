void user_prefix(void) __attribute__((section(".kernel")));
void svc_handler(uint32_t*) __attribute__((section(".kernel")));

#define _svc(x)     asm volatile ("svc  %0  \n" :: "i" (x))
