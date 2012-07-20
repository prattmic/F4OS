/* SVC case names */
#define     SVC_RAISE_PRIV      0x0
#define     SVC_YIELD           0x1
#define     SVC_END_TASK        0x2

void user_prefix(void) __attribute__((section(".kernel")));
void pendsv_handler(void) __attribute__((section(".kernel"), naked));
void svc_handler(uint32_t*) __attribute__((section(".kernel")));
extern uint32_t *save_context(void) __attribute__((section(".kernel"), naked));
extern uint32_t *restore_context() __attribute__((section(".kernel"), naked));
extern uint32_t *restore_full_context() __attribute__((section(".kernel"), naked));
