#define BASE_REGION             0
#define USER_MEM_REGION         1
#define VECTFLASH_REGION        2
#define RAM_REGION              4
#define PRIV_PERIPH_REGION      5
#define KERNEL_CODE_REGION      6
#define KERNEL_MEM_REGION       7

void mpu_setup(void) __attribute__((section(".kernel")));
void mpu_stack_set(uint32_t *stack_base) __attribute__((section(".kernel")));
uint16_t mpu_size(uint32_t size) __attribute__((section(".kernel")));
