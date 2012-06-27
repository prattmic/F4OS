#include "types.h"
#include "mem.h"
#include "heap.h"

/* kheap and kmalloc are the kernel versions */

extern uint32_t _suserheap;
extern uint32_t _euserheap;
extern uint32_t _skernelheap;
extern uint32_t _ekernelheap;

/*-------------------------- Simple heap, by mgyenik -----------------------------*/

/* This struct can be made larger to enlarge the minimum block used for allocation. Decreases overhead for defragmentation,
   but increases how much memory silly little things use. */
typedef struct heap_node_struct {
    struct heap_node_struct *next;
} heap_node;

#define HEAP_BLOCK_SIZE sizeof(heap_node*) /* Total size (in bytes) of one heap_node for use in allocing mem. */

typedef struct heaplist_struct {
    heap_node *head;
    heap_node *tail;
} heaplist;

static heaplist k_heaplist = {NULL ,
                                NULL };

static heaplist u_heaplist = {NULL ,
                                NULL };

void init_uheap(void) {
    u_heaplist.head = (heap_node *) &_suserheap;               /* Set heap to first address in heap area. */
    u_heaplist.tail = (heap_node *) &_euserheap;               /* This is the end of the heap. We don't frack with mem after this address. */
    heap_node *curr_node = u_heaplist.head;                    /* Pointer to heap block we are setting up. */

    while(curr_node < u_heaplist.tail){
        curr_node->next = (heap_node*) ((uint32_t) curr_node + HEAP_BLOCK_SIZE);  /* Hack to ensure that HEAP_BLOCK_SIZE is added as bytes instead of words */
        curr_node = curr_node->next;
    }
    u_heaplist.tail->next = NULL;                              /* Clear next node of tail */
}

void init_kheap(void){
    k_heaplist.head = (heap_node *) &_skernelheap;             /* Set heap to first address in heap area. */
    k_heaplist.tail = (heap_node *) &_ekernelheap;             /* This is the end of the heap. We don't frack with mem after this address. */
    heap_node *curr_node = k_heaplist.head;                    /* Pointer to heap block we are setting up. */

    while(curr_node < k_heaplist.tail){
        curr_node->next = (heap_node*) ((uint32_t) curr_node + HEAP_BLOCK_SIZE);  /* Hack to ensure that HEAP_BLOCK_SIZE is added as bytes instead of words */
        curr_node = curr_node->next;
    }
    k_heaplist.tail->next = NULL;                              /* Clear next node of tail */
}

void* malloc(uint32_t size, uint16_t aligned) {
    /* TODO: Ensure memory block returned is contiguous */
    /* TODO: Return errors; Corollary: Add error checking to everything using malloc */
    int needed_blocks = size/(HEAP_BLOCK_SIZE) + 1;
    heap_node *prev_node = NULL;
    heap_node *break_node = u_heaplist.head;         /* Node at which we jump to after the break */

    if (aligned) {
        /* Start at an aligned memory location */
        while ((uint32_t) break_node % size) {
            prev_node  = break_node;
            break_node = break_node->next;
        }
    }

    heap_node *ret_node = break_node;

    /* Find which node will be new head, then return old head, which is first mem location in newly alloc'd mem */
    for(int i = 0; i < needed_blocks; i++){
        break_node = break_node->next;
    }

    if (prev_node != NULL) {
        /* Connect list, skipping returned region */
        prev_node->next = break_node;
    }
    else {
        /* Move head */
        u_heaplist.head = break_node;
    }
    return ret_node;
}

void free(void *mem, uint32_t size) {
    heap_node *new_node = mem;
    heap_node *curr_node = mem;
    uint32_t blocks = size/(HEAP_BLOCK_SIZE);

    for (int i = 0; i < blocks; i++) {
        curr_node->next = (heap_node *) ((uint32_t) curr_node + HEAP_BLOCK_SIZE);
        curr_node = curr_node->next;
    }
    curr_node->next = NULL;

    u_heaplist.tail->next = new_node;
    u_heaplist.tail = curr_node;
}

void* kmalloc(uint32_t size){
    /* TODO: Ensure memory block returned is contiguous */
    /* TODO: Return errors; Corollary: Add error checking to everything using malloc */
    int needed_blocks = size/(HEAP_BLOCK_SIZE) + 1;
    heap_node *break_node = k_heaplist.head;         /* Node at which we jump to after the break */
    heap_node *ret_node = NULL;

    /* Find which node will be new head, then return old head, which is first mem location in newly alloc'd mem */
    for(int i = 0; i < needed_blocks; i++){
        break_node = break_node->next;
    }

    ret_node = k_heaplist.head;                     /* Head now points to alloc'd mem. Get ready to return that mem. */
    k_heaplist.head = break_node;                   /* Move head forward to break node. Heap has grown upwards. */
    return ret_node;
}

void kfree(void *mem, uint32_t size) {
    heap_node *new_node = mem;
    heap_node *curr_node = mem;
    uint32_t blocks = size/(HEAP_BLOCK_SIZE);

    for (int i = 0; i < blocks; i++) {
        curr_node->next = (heap_node *) ((uint32_t) curr_node + HEAP_BLOCK_SIZE);
        curr_node = curr_node->next;
    }
    curr_node->next = NULL;

    k_heaplist.tail->next = new_node;
    k_heaplist.tail = curr_node;
}
