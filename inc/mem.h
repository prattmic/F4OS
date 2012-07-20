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

void memset32(void *p, int32_t value, uint32_t size) __attribute__((section(".kernel")));
void create_context(task_ctrl *task, void (*lptr)(void)) __attribute__((section(".kernel"), optimize(0)));

struct memlist {
    struct memlist *next;
};
