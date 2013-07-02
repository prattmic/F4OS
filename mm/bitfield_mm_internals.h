#ifndef MM_BITFIELD_MM_INTERNALS_H_INCLUDED
#define MM_BITFIELD_MM_INTERNALS_H_INCLUDED

extern uint32_t _suserheap;
extern uint32_t _euserheap;
extern uint32_t _skernelheap;
extern uint32_t _ekernelheap;

#define MM_GRAINS_PER_BLOCK 32          /* Due to bits in uint32_t */
#define MM_GRAIN_SIZE (1 << CONFIG_MM_GRAIN_SHIFT)
#define MM_MAGIC 0xABCD
#define MM_BLOCK_SIZE (MM_GRAINS_PER_BLOCK*MM_GRAIN_SIZE)
#define MM_USER_NUM_BLOCKS ((CONFIG_EUSERHEAP - CONFIG_SUSERHEAP)/MM_BLOCK_SIZE)
#define MM_KERNEL_NUM_BLOCKS ((CONFIG_EKERNELHEAP - CONFIG_SKERNELHEAP)/MM_BLOCK_SIZE)
#define MM_MAX_USER_SIZE (CONFIG_EUSERHEAP - CONFIG_SUSERHEAP)
#define MM_MAX_KERNEL_SIZE (CONFIG_EKERNELHEAP - CONFIG_SKERNELHEAP)

/* Gives back mask with n bits set to 1 */
#define MASK(n) ((unsigned int)((1UL << (n)) - 1))

typedef struct alloc_header {
    uint16_t    magic;
    uint16_t    grains;
} alloc_header_t;

typedef struct mm_block {
    uint8_t     free_grains;
    uint32_t    free_mask;
} mm_block_t;

extern mm_block_t userheap[];
extern mm_block_t kernelheap[];
extern struct semaphore userheap_mutex;
extern struct semaphore kernelheap_mutex;

/* Returns value to left shift mask by to look at freemask */
static inline uint32_t addr_to_grain_offset(void *addr, void *base) {
    return (((uintptr_t)addr - (uintptr_t)base)%MM_BLOCK_SIZE)/MM_GRAIN_SIZE;
}

/* Returns whether or not space for mask is free in a freemask */
static inline int is_free(uint32_t field, uint32_t mask) {
    return ((field & mask) == 0);
}

/* Returns block index given address */
static inline uint32_t addr_to_block(void *addr, void *base) {
    return ((uintptr_t)addr - (uintptr_t)base)/MM_BLOCK_SIZE;
}

/* Returns address of allocation to return */
static inline void *to_addr(uint32_t idx, uint32_t grain_offset, void *base) {
    return (void *)((uintptr_t)base + idx*MM_BLOCK_SIZE + grain_offset*MM_GRAIN_SIZE);
}

#endif
