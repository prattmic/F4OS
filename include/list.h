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

/* Insert new list element between two known elements */
static inline void list_insert(struct list *new, struct list *before,
        struct list *after) {
    before->next = new;
    new->prev = before;
    new->next = after;
    after->prev = new;
}

/* Adds n to front of list head */
static inline void list_add(struct list *new, struct list *head) {
    list_insert(new, head, head->next);
}

/* Removes from tail of list */
static inline list_t *list_pop(struct list *l) {
    list_t *ret;
    ret = l->prev;
    l->prev = ret->prev;
    ret->prev->next = l;
    return ret;
}

/* Iterate over each element in list
 * curr - struct list * that is updated with each iteration
 * head - struct list * to begin iteration at */
#define list_for_each(curr, head) \
    for (curr = (head)->next; curr != (head); curr = curr->next)

/* Iterate over each entry in list
 * curr - entry type * that is updated with each iteration
 * head - struct list * to begin iteration at
 * member - name of struct list member in entry */
#define list_for_each_entry(curr, head, member)                            \
    for (curr = container_of((head)->next, typeof(*curr), member);  \
        &curr->member != (head);                                   \
        curr = container_of(curr->member.next, typeof(*curr), member))

