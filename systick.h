
void systick_init(void) __attribute__((section(".kernel")));
void systick_handler(void) __attribute__((section(".kernel"), naked));
