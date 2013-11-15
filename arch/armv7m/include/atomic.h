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

#ifndef ARCH_ATOMIC_H_INCLUDED
#define ARCH_ATOMIC_H_INCLUDED

/* TODO: Move to a more general header */
#define __always_inline    inline __attribute__((always_inline))

/* word aligned reads/writes of words are atomic */
static __always_inline int atomic_read(atomic_t *v) {
    return v->num;
}

static __always_inline void atomic_set(atomic_t *v, int i) {
    v->num = i;
}

static inline int atomic_add(atomic_t *v, int i) {
    int ret;
    uint8_t failed;

    asm volatile(
        "0: ldrex  %[ret], [%[num]]            \n\t"
        "   add    %[ret], %[ret], %[i]        \n\t"
        "   strex  %[failed], %[ret], [%[num]] \n\t"
        "   cmp    %[failed], #0               \n\t"
        "   bne    0b                          \n\t"
        : [ret] "=&r" (ret), [failed] "=r" (failed)
        : [num] "r" (&v->num), [i] "ri" (i)
        : "memory"
    );

    return ret;
}

static inline int atomic_sub(atomic_t *v, int i) {
    int ret;
    uint8_t failed;

    asm volatile(
        "0: ldrex  %[ret], [%[num]]            \n\t"
        "   sub    %[ret], %[ret], %[i]        \n\t"
        "   strex  %[failed], %[ret], [%[num]] \n\t"
        "   cmp    %[failed], #0               \n\t"
        "   bne    0b                          \n\t"
        : [ret] "=&r" (ret), [failed] "=r" (failed)
        : [num] "r" (&v->num), [i] "ri" (i)
        : "memory"
    );

    return ret;
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
    unsigned int ret;
    uint8_t failed;

    asm volatile(
        "0: ldrex  %[ret], [%[ptr]]             \n\t"
        "   strex  %[failed], %[new], [%[ptr]]  \n\t"
        "   cmp    %[failed], #0                \n\t"
        "   bne    0b                           \n\t"
        : [ret] "=r" (ret), [ptr] "=r" (ptr), [failed] "=r" (failed)
        : [new] "r" (update)
        : "memory"
    );

    return ret;
}

static inline uint32_t atomic_or(uint32_t *ptr, uint32_t val) {
    uint32_t ret;
    uint32_t failed;

    asm volatile (
        "0: ldrex %[ret], [%[ptr]]              \n\t"
        "   orr %[ret], %[ret], %[val]          \n\t"
        "   strex %[failed], %[ret], [%[ptr]]   \n\t"
        "   cmp %[failed], #0                   \n\t"
        "   bne 0b                              \n\t"
        : [failed] "=r" (failed), [ret] "=&r" (ret)
        : [ptr] "r" (ptr), [val] "ri" (val)
        : "memory"
    );

    return ret;
}

static inline uint32_t atomic_and(uint32_t *ptr, uint32_t val) {
    uint32_t ret;
    uint8_t failed;

    asm volatile (
        "0: ldrex %[ret], [%[ptr]]              \n\t"
        "   and %[ret], %[ret], %[val]          \n\t"
        "   strex %[failed], %[ret], [%[ptr]]   \n\t"
        "   cmp %[failed], #0                   \n\t"
        "   bne 0b                              \n\t"
        : [failed] "=r" (failed), [ret] "=&r" (ret)
        : [ptr] "r" (ptr), [val] "ri" (val)
        : "memory"
    );

    return ret;
}

static __always_inline uint32_t load_link(volatile uint32_t *address) {
    uint32_t val;

    asm volatile (
        "ldrex  %[val], [%[ptr]]\n\t"
        : [val] "=r" (val)
        : [ptr] "r" (address)
    );

    return val;
}

static __always_inline uint8_t store_conditional(volatile uint32_t *address, uint32_t value) {
    uint32_t failed;

    asm volatile (
        "strex  %[failed], %[val], [%[ptr]]\n\t"
        : [failed] "=r" (failed)
        : [ptr] "r" (address), [val] "r" (value)
        : "memory"
    );

    return failed;
}

#endif /* ARCH_ATOMIC_H_INCLUDED */
