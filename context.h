#define stuff 0

void user_prefix(void) __attribute__((section(".kernel")));
void svc_handler(uint32_t*) __attribute__((section(".kernel")));


