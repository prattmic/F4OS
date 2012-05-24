#define IDLE_TASK_BASE (uint8_t *)0x200

typedef struct k_task_node_struct{
    struct k_task_node_struct*  next_node;
    struct k_task_struct*       task;
}taskNode;

typedef struct k_taskNode_list{
    taskNode*   head;
    taskNode*   tail;
}taskNodeList;
   
typedef struct k_task_struct{
    uint8_t *stack_top;
    uint8_t *stack_base;
    uint8_t priority;
    void(*fptr)(void);
    unsigned long ticks_until_wake;
}taskCtl;

void idle_task(void);
void register_task(void(*task_ptr)(void));
inline void k_set_ct_delay(unsigned long new_delay) __attribute__((always_inline));
taskNode* find_last_taskNode(taskNode* init_node);
void append_task_to_queue(taskNode* new_task);

