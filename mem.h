/* mem.h: memory operations definitions.
 * Convers mem.S and mem.c */

/* mem.S */
extern void raise_privilege(void);
extern void user_mode(void);

/* mem.c */
uint16_t mpu_size(uint32_t size);
