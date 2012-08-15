
void spin_acquire(volatile struct semaphore *semaphore);
void acquire(volatile struct semaphore *semaphore);
void release(volatile struct semaphore *semaphore);
void init_semaphore(volatile struct semaphore *semaphore);
