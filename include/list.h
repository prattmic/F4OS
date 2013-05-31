/* Simplified linux list implementation */

#define container_of(ptr, type, member) ({ \
    const typeof( ((type *)0)->member ) *__mptr = (ptr); \
    (type *)( (char *)__mptr - __builtin_offsetof(type,member) );})


typedef struct list {
    struct list *next;
    struct list *prev;
} list_t;

#define INIT_LIST(list) {   \
    .next = &(list),        \
    .prev = &(list)         \
}

#define DEFINE_LIST(name) \
    struct list name = INIT_LIST(name);


#define LIST_ELEMENT struct list _list

#define list_empty(l) ((l) == (l)->next)

/* Adds n to front of list l */
static inline void list_add(struct list *n, struct list *l) {
    l->next->prev = n;
    n->next = l->next;
    n->prev = l;
    l->next = n;
}

/* Removes from tail of list */
static inline list_t *list_pop(struct list *l) {
    list_t *ret;
    ret = l->prev;
    l->prev = ret->prev;
    ret->prev->next = l;
    return ret;
}

#define list_for_each(curr, start) for( curr=(head)->next; curr != (head); curr = curr->next)

#define for_each(curr, head, member)                            \
for (curr = container_of((head)->next, typeof(*curr), member);  \
     &curr->member != (head);                                   \
     curr = container_of(curr->member.next, typeof(*curr), member))

