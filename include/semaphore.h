void spin_acquire(volatile uint8_t *semaphore);
void acquire(volatile uint8_t *semaphore) __attribute__((naked));
void release(volatile uint8_t *semaphore);
