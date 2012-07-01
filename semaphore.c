#include "types.h"
#include "registers.h"
#include "context.h"
#include "semaphore.h"

void aquire(volatile uint8_t *semaphore) {
    while (*semaphore) {
        yield();
    }
    *semaphore = 1;
}

void release(volatile uint8_t *semaphore) {
    *semaphore = 0;
}
