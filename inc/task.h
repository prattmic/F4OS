#define IDLE_TASK_BASE (uint32_t *)0x200

typedef struct k_task_node_struct {
    struct k_task_node_struct*  prev;
    struct k_task_node_struct*  next;
    struct k_task_struct*       task;
} task_node;

typedef struct k_task_node_list {
    task_node*   head;
    task_node*   tail;
} task_node_list;
   
typedef struct k_task_struct {
    uint32_t *stack_top;
    uint32_t *stack_base;
    void(*fptr)(void);
    uint32_t ticks_until_wake;
    uint8_t priority;
    uint8_t running;
} task_ctrl;

task_node *k_curr_task;
extern task_node * volatile task_to_free;
extern uint8_t task_switching;

void idle_task(void);
void end_task(void) __attribute__((section(".kernel"),naked));;
void kernel_task(void) __attribute__((section(".kernel")));
void start_task_switching(void) __attribute__((section(".kernel")));
void switch_task(void) __attribute__((section(".kernel")));
task_ctrl *create_task(void (*fptr)(void), uint8_t priority, uint32_t ticks_until_wake) __attribute__((section(".kernel")));
task_node *register_task(task_ctrl *task_ptr) __attribute__((section(".kernel")));
void remove_task(task_node *tasknode) __attribute__((section(".kernel")));
void free_task(task_node *tasknode) __attribute__((section(".kernel")));
//inline void k_set_ct_delay(unsigned long new_delay) __attribute__((always_inline));
task_node* find_last_task_node(task_node* init_node) __attribute__((section(".kernel")));
void append_task_to_queue(task_node* new_task) __attribute__((section(".kernel")));
