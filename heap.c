#include "types.h"
#include "mem.h"
#include "heap.h"


typedef struct heap_node_struct{
    struct heap_node_struct* next_node;
    uint8_t mem[HEAP_MEM_SIZE];
}heapNode;

#define HEAP_BLOCK_SIZE (HEAP_MEM_SIZE + sizeof(heapNode*))

typedef struct heap_list_struct{
    heapNode* head;
    heapNode* tail;
}heapList;

static heapList k_heapList = {  NULL ,
                                NULL };

void* malloc(int size){
    int needed_blocks = size/(HEAP_BLOCK_SIZE)+ 1;
    heapNode* break_node = k_heapList.head;         /* Node at which we jump to after the break */
    heapNode* ret_node = NULL;
    /* Find which node will be new head, then return old head, which is first mem location in newly alloc'd mem */
    for(int i = 0; i < needed_blocks; i++){
        break_node = break_node->next_node;
    }
    ret_node = k_heapList.head;
    k_heapList.head = break_node;
    return ret_node;
}
