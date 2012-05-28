/* mem.h: memory operations definitions.
 * Convers mem.S and mem.c */

/* mem.S */
extern void raise_privilege(void);
extern void user_mode(void);
extern void enable_psp(uint32_t *);
extern void disable_psp();

/* mem.c */
#define NULL                    (void *) 0x00000000
#define STKSIZE                 32                      /* This is in words */

/* For MPU */
#define FLASH_REGION            0
#define USER_MEM_REGION         1
#define KERNEL_CODE_REGION      7
#define KERNEL_STACK_REGION     6
#define PERIPH_REGION           3

void memset32(uint32_t *p, int32_t value, uint32_t size) __attribute__((section(".kernel")));
uint16_t mpu_size(uint32_t size) __attribute__((section(".kernel")));

struct memlist {
    struct memlist *next;
};
