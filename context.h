void user_prefix(void) __attribute__((section(".kernel")));
void systick_handler(void) __attribute__((section(".kernel")));
void svc_handler(uint32_t*) __attribute__((section(".kernel")));

extern uint32_t *save_context(void) __attribute__((section(".kernel"),naked));
extern void restore_context(uint32_t *) __attribute__((section(".kernel"),naked));
