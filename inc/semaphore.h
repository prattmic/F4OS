struct semaphore {
    uint8_t lock;
    task_ctrl *held_by;
};

void spin_acquire(volatile struct semaphore *semaphore);
void acquire(volatile struct semaphore *semaphore) __attribute__((naked));
void release(volatile struct semaphore *semaphore);
void init_semaphore(volatile struct semaphore *semaphore);
