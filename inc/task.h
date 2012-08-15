#define RESOURCE_TABLE_SIZE         16

typedef uint8_t rd_t;

typedef struct task_node_struct {
    struct task_node_struct *prev;
    struct task_node_struct *next;
    struct task_struct      *task;
} task_node;

typedef struct task_node_list {
    task_node   *head;
    task_node   *tail;
} task_node_list;


struct semaphore {
        uint8_t     lock;
        task_node   *held_by;
};

typedef struct semaphore semaphore;

typedef struct resource {
    semaphore   *sem;
    void        *env;
    void        (*writer)(char, void*);
    char        (*reader)(void*);
} resource;
   
typedef struct task_struct {
    uint32_t    *stack_top;
    uint32_t    *stack_base;
    void        (*fptr)(void);
    uint32_t    period;
    uint32_t    ticks_until_wake;
    uint8_t     priority;
    uint8_t     running;
    uint32_t    pid;
    task_node   *task_list_node;
    task_node   *periodic_node;
    resource    *resources[RESOURCE_TABLE_SIZE];
    rd_t        top_rd;
} task_ctrl;

task_node_list task_list;
task_node_list periodic_task_list;

task_node *curr_task;
extern task_node * volatile task_to_free;
extern uint8_t task_switching;

void kernel_task(void) __attribute__((section(".kernel")));
void start_task_switching(void) __attribute__((section(".kernel")));
void switch_task(void) __attribute__((section(".kernel")));
task_ctrl *create_task(void (*fptr)(void), uint8_t priority, uint32_t period) __attribute__((section(".kernel")));
task_node *register_task(task_node_list *list, task_ctrl *task_ptr) __attribute__((section(".kernel")));
void new_task(void (*fptr)(void), uint8_t priority, uint32_t period) __attribute__((section(".kernel")));
void append_task(task_node_list *list, task_node *task) __attribute__((section(".kernel")));
void remove_task(task_node_list *list, task_node *tasknode) __attribute__((section(".kernel")));
void free_task(task_node *tasknode) __attribute__((section(".kernel")));
void end_task(void) __attribute__((section(".kernel"),naked));;
void end_periodic_task(void) __attribute__((section(".kernel"),naked));;
