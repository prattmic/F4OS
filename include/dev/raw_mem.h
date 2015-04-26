/*
 * Copyright (C) 2013 F4OS Authors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef INCLUDE_DEV_RAW_MEM_H_INCLUDED
#define INCLUDE_DEV_RAW_MEM_H_INCLUDED

#include <atomic.h>
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
 * Adding a volatile qualifier ensures that the compiler does not optimize the
 * access away.
 */
#define raw_mem_cast(__address) ((volatile typeof(__address))(__address))

/*
 * Safely read raw memory.
 *
 * Perform a read of raw memory, including a memory barrier.
 *
 * @param __address Pointer to location to read
 * @returns Value at __address
 */
#define raw_mem_read(__address) ({    \
    typeof(*__address) __val = *raw_mem_cast(__address); \
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
    *raw_mem_cast(__address) = __val; \
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
 * Raw memory set bits (atomic)
 *
 * Perform an atomic write of raw memory, setting the specified bits.
 *
 * @param __address Pointer to location to write
 * @param __bits    Bits to set
 */
#define raw_mem_set_bits_atomic(__address, __bits) do {    \
    typeof(*__address) __val;   \
    do {    \
        __val = load_link(raw_mem_cast(__address)); \
        __val |= __bits;    \
    } while (store_conditional(raw_mem_cast(__address), __val));    \
    memory_barrier();   \
} while (0)

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
 * Raw memory clear bits (atomic)
 *
 * Perform an atomic write of raw memory, clearing the specified bits.
 *
 * @param __address Pointer to location to write
 * @param __bits    Bits to set
 */
#define raw_mem_clear_bits_atomic(__address, __bits) do {    \
    typeof(*__address) __val;   \
    do {    \
        __val = load_link(raw_mem_cast(__address)); \
        __val &= ~(__bits);    \
    } while (store_conditional(raw_mem_cast(__address), __val));    \
    memory_barrier();   \
} while (0)

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

/*
 * Raw memory set masked region (atomic)
 *
 * Perform an atomic write of raw memory, clearing a masked region first.
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
#define raw_mem_set_mask_atomic(__address, __mask, __val) do {    \
    typeof(*__address) __tmp;   \
    do {    \
        __tmp = load_link(raw_mem_cast(__address)); \
        __tmp &= ~(__mask); \
        __tmp |= __val;    \
    } while (store_conditional(raw_mem_cast(__address), __tmp));    \
    memory_barrier();   \
} while (0)

#endif
