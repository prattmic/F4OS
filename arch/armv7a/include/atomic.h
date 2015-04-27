/*
 * Copyright (C) 2014 F4OS Authors
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

/*
 * XXX: These may not be SMP safe, as they have not been vetted for
 * safety within ARM's weakly consistent memory model.
 */

#ifndef ARCH_ATOMIC_H_INCLUDED
#define ARCH_ATOMIC_H_INCLUDED

#include <compiler.h>

/* word aligned reads/writes of words are atomic */
static __always_inline int atomic_read(atomic_t *v) {
    int ret = 0;

    __atomic_load(&v->num, &ret, __ATOMIC_SEQ_CST);

    return ret;
}

static __always_inline void atomic_set(atomic_t *v, int i) {
    __atomic_store(&v->num, &i, __ATOMIC_SEQ_CST);
}

static inline int atomic_add(atomic_t *v, int i) {
    return __atomic_add_fetch(&v->num, i, __ATOMIC_SEQ_CST);
}

static inline int atomic_sub(atomic_t *v, int i) {
    return __atomic_sub_fetch(&v->num, i, __ATOMIC_SEQ_CST);
}

static __always_inline int atomic_inc(atomic_t *v) {
    return atomic_add(v, 1);
}

static __always_inline int atomic_dec(atomic_t *v) {
    return atomic_sub(v, 1);
}

static __always_inline int atomic_dec_and_test(atomic_t *v) {
    return atomic_dec(v) == 0;
}

static inline uint32_t atomic_spin_swap(uint32_t *ptr, uint32_t update) {
    unsigned int ret = 0;

    __atomic_exchange(ptr, &update, &ret, __ATOMIC_SEQ_CST);

    return ret;
}

static inline uint32_t atomic_or(uint32_t *ptr, uint32_t val) {
    return __atomic_or_fetch(ptr, val, __ATOMIC_SEQ_CST);
}

static inline uint32_t atomic_and(uint32_t *ptr, uint32_t val) {
    return __atomic_and_fetch(ptr, val, __ATOMIC_SEQ_CST);
}

static __always_inline uint32_t load_link32(volatile uint32_t *address) {
    uint32_t val;

    asm volatile (
        "dmb\n\r"
        "ldrex  %[val], [%[ptr]]\n\t"
        : [val] "=r" (val)
        : [ptr] "r" (address)
    );

    return val;
}

static __always_inline uint8_t store_conditional32(volatile uint32_t *address,
                                                   uint32_t value) {
    uint32_t failed;

    asm volatile (
        "strex  %[failed], %[val], [%[ptr]]\n\t"
        "dmb\n\r"
        : [failed] "=&r" (failed)
        : [ptr] "r" (address), [val] "r" (value)
        : "memory"
    );

    return failed;
}

static __always_inline uint16_t load_link16(volatile uint16_t *address) {
    uint16_t val;

    asm volatile (
        "dmb\n\r"
        "ldrexh %[val], [%[ptr]]\n\t"
        : [val] "=r" (val)
        : [ptr] "r" (address)
    );

    return val;
}

static __always_inline uint8_t store_conditional16(volatile uint16_t *address,
                                                   uint16_t value) {
    uint32_t failed;

    asm volatile (
        "strexh %[failed], %[val], [%[ptr]]\n\t"
        "dmb\n\r"
        : [failed] "=&r" (failed)
        : [ptr] "r" (address), [val] "r" (value)
        : "memory"
    );

    return failed;
}

static __always_inline uint8_t load_link8(volatile uint8_t *address) {
    uint8_t val;

    asm volatile (
        "dmb\n\r"
        "ldrexb %[val], [%[ptr]]\n\t"
        : [val] "=r" (val)
        : [ptr] "r" (address)
    );

    return val;
}

static __always_inline uint8_t store_conditional8(volatile uint8_t *address,
                                                  uint8_t value) {
    uint32_t failed;

    asm volatile (
        "strexb %[failed], %[val], [%[ptr]]\n\t"
        "dmb\n\r"
        : [failed] "=&r" (failed)
        : [ptr] "r" (address), [val] "r" (value)
        : "memory"
    );

    return failed;
}

#endif /* ARCH_ATOMIC_H_INCLUDED */
