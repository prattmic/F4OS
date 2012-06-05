#define IDLE_TASK_BASE (uint32_t *)0x200

typedef struct k_task_node_struct{
    struct k_task_node_struct*  next_node;
    struct k_task_struct*       task;
}taskNode;

typedef struct k_taskNode_list{
    taskNode*   head;
    taskNode*   tail;
}taskNodeList;
   
typedef struct k_task_struct{
    uint32_t *stack_top;
    uint32_t *stack_base;
    uint8_t priority;
    void(*fptr)(void);
    uint32_t ticks_until_wake;
}taskCtrl;

void idle_task(void);
taskCtrl* create_task(void (*fptr)(void), uint8_t priority, uint32_t ticks_until_wake);
void register_task(taskCtrl *task_ptr);
//inline void k_set_ct_delay(unsigned long new_delay) __attribute__((always_inline));
taskNode* find_last_taskNode(taskNode* init_node);
void append_task_to_queue(taskNode* new_task);

