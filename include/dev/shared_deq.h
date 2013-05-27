/* Needs list.h */

typedef struct shared_deq {
    struct list list;
    struct semaphore sem;
} shared_deq_t;


#define DEFINE_SHARED_DEQ(name)         \
    struct shared_deq name = {          \
        .list = INIT_LIST(name.list),   \
        .sem.lock = 0,                  \
        .sem.waiting = NULL,            \
        .sem.held_by = NULL             \
    };


#define sdeq_empty(sdeq) (list_empty(&(sdeq)->list))

static inline void sdeq_add(shared_deq_t *sdeq, list_t *l) {
    acquire(&sdeq->sem);
    list_add(l, &sdeq->list);
    release(&sdeq->sem);
}

static inline list_t *sdeq_pop(shared_deq_t *sdeq) {
    list_t *ret;
    acquire(&sdeq->sem);
    ret = list_pop(&sdeq->list);
    release(&sdeq->sem);
    return ret;
}

/* NOT INHERENTLY THREADSAFE */
#define for_each_in_sdeq(curr, sdeq, member) for_each(curr, &sdeq.list, member)

