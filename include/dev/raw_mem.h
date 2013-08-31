#ifndef INCLUDE_DEV_RAW_MEM_H_INCLUDED
#define INCLUDE_DEV_RAW_MEM_H_INCLUDED

#include <stdint.h>

/* Prevent the compiler from reordering memory access */
#define compiler_memory_barrier()  do { \
    asm volatile ("" :::"memory");      \
} while (0)

/* In the future, it may make sense to include a CPU barrier */
#define memory_barrier()    do {\
    compiler_memory_barrier();  \
} while (0)

/*
 * Safely read raw memory.
 *
 * Perform a read of raw memory, including a memory barrier.
 *
 * @param __address Pointer to location to read
 * @returns Value at __address
 */
#define raw_mem_read(__address) ({    \
    typeof(*__address) __val = *(volatile typeof(__address))__address; \
    memory_barrier();   \
    __val;              \
})

/*
 * Safely write raw memory.
 *
 * Perform a write of raw memory, including a memory barrier.
 *
 * @param __address Pointer to location to write
 * @param __val     Value to write
 */
#define raw_mem_write(__address, __val) ({    \
    *((volatile typeof(__address))(__address)) = __val; \
    memory_barrier();   \
})

/*
 * Raw memory set bits
 *
 * Perform a write of raw memory, including a memory barrier,
 * setting the specified bits.
 *
 * @param __address Pointer to location to write
 * @param __bits    Bits to set
 */
#define raw_mem_set_bits(__address, __bits) ({    \
    typeof(*__address) __val = raw_mem_read(__address); \
    __val |= __bits;    \
    raw_mem_write(__address, __val);    \
})

/*
 * Raw memory clear bits
 *
 * Perform a write of raw memory, including a memory barrier,
 * clearing the specified bits.
 *
 * @param __address Pointer to location to write
 * @param __bits    Bits to clear
 */
#define raw_mem_clear_bits(__address, __bits) ({    \
    typeof(*__address) __val = raw_mem_read(__address); \
    __val &= ~(__bits);    \
    raw_mem_write(__address, __val);    \
})

/*
 * Raw memory set masked region
 *
 * Perform a write of raw memory, including a memory barrier,
 * clearing a masked region first.
 *
 * This is equivalent to the following pseudocode:
 *
 * tmp = *reg;
 * tmp &= ~mask;
 * tmp |= val;
 * *reg = tmp;
 *
 * @param __address Pointer to location to write
 * @param __mask    Mask to clear
 * @param __value   Value to write
 */
#define raw_mem_set_mask(__address, __mask, __val) ({    \
    typeof(*__address) __tmp = raw_mem_read(__address); \
    __tmp &= ~(__mask); \
    __tmp |= __val;    \
    raw_mem_write(__address, __tmp);    \
})

#endif
