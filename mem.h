/* mem.h: memory operations definitions.
 * Convers mem.S and mem.c */

/* mem.S */
extern void raise_privilege(void);
extern void user_mode(void);
extern void user_mode_branch(void (*fptr)(void));
extern void enable_psp(uint32_t *);
extern void disable_psp();

/* mem.c */
#define NULL                    (void *) 0x00000000
#define STKSIZE                 64                      /* This is in words */

/* For MPU */
#define FLASH_REGION            0
#define USER_MEM_REGION         1
#define KERNEL_CODE_REGION      2       /* This MUST be 2, because WTF?! */ /* Actually, this effectively disabled this MPU section */
#define KERNEL_STACK_REGION     6
#define PERIPH_REGION           5

void memset32(uint32_t *p, int32_t value, uint32_t size) __attribute__((section(".kernel")));
void mpu_stack_set(uint32_t *stack_base);
uint16_t mpu_size(uint32_t size) __attribute__((section(".kernel")));

struct memlist {
    struct memlist *next;
};
