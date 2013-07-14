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
        : [ret] "=r" (ret), [failed] "=r" (failed)
        : [num] "r" (&v->num), [i] "g" (i)
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
        : [ret] "=r" (ret), [failed] "=r" (failed)
        : [num] "r" (&v->num), [i] "g" (i)
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
        : [failed] "=r" (failed), [ret] "=r" (ret)
        : [ptr] "r" (ptr), [val] "g" (val)
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
        : [failed] "=r" (failed), [ret] "=r" (ret)
        : [ptr] "r" (ptr), [val] "g" (val)
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
