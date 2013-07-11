#ifndef ATOMIC_H_INCLUDED
#define ATOMIC_H_INCLUDED

/*
 * Atomic Operations
 *
 * Provide a standard API for atomic operations, available across architectures.
 *
 * These are largely based on/derived from the Linux kernel API.
 *
 * Most implementations are architecture dependent, so they may have vastly different
 * performance, depending on the architecture.
 */

#include <stdint.h>

/*
 * By placing the atomic int inside of a struct, it is not castable to a non-atomic
 * int, which provides nice type safety.
 */
typedef struct {
    volatile int num __attribute__((aligned(4)));
} atomic_t;

/* Statically initialize atomic int */
#define ATOMIC_INIT(n)                  {(n)}

/**
 * Read atomic int
 *
 * @param v     Atomic number to read
 * @returns Value of atomic number
 */
static int atomic_read(atomic_t *v);

/**
 * Set atomic int
 *
 * @param v     Atomic number to set
 * @param i     Value to set
 */
static void atomic_set(atomic_t *v, int i);

/**
 * Atomic addition
 *
 * @param v     Atomic number to add to
 * @param i     Value to add
 * @returns Final value of atomic int
 */
static int atomic_add(atomic_t *v, int i);

/**
 * Atomic subtraction
 *
 * @param v     Atomic number to subtract from
 * @param i     Value to subtract
 * @returns Final value of atomic int
 */
static int atomic_sub(atomic_t *v, int i);

/**
 * Atomic increment
 *
 * Equivalent to atomic_add(v, 1).
 *
 * @param v     Atomic number to increment
 * @returns Final value of atomic int
 */
static int atomic_inc(atomic_t *v);

/**
 * Atomic decrement
 *
 * Equivalent to atomic_sub(v, 1).
 *
 * @param v     Atomic number to decrement
 * @returns Final value of atomic int
 */
static int atomic_dec(atomic_t *v);

/**
 * Atomic decrement and test
 *
 * Equivalent to atomic_sub(v, 1) == 0.
 *
 * @param v     Atomic number to decrement
 * @returns 1 if number == 0, 0 otherwise
 */
static int atomic_dec_and_test(atomic_t *v);

/**
 * Atomic spin swap
 *
 * Swap the value at a given memory address with a new value,
 * returning the old value.  May need to spin until successful.
 *
 * @param ptr       Memory location to update
 * @param update    Value to write to location
 * @returns Value at ptr before swap
 */
static uint32_t atomic_spin_swap(uint32_t *ptr, uint32_t update);

/**
 * Atomic OR
 *
 * Load the value pointed to by ptr, OR in val, and store the value back
 *
 * @param ptr   Pointer to memory location to update
 * @param val   Value to OR into memory location
 * @returns Result of OR operation
 */
static uint32_t atomic_or(uint32_t *ptr, uint32_t val);

/**
 * Atomic AND
 *
 * Load the value pointed to by ptr, AND in val, and store the value back
 *
 * @param ptr   Pointer to memory location to update
 * @param val   Value to AND into memory location
 * @returns Result of AND operation
 */
static uint32_t atomic_and(uint32_t *ptr, uint32_t val);

/* Include actual arch implementation */
#include <arch/atomic.h>

#endif
