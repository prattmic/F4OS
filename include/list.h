#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

/* Simplified linux list implementation */

#include <stddef.h>

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

static inline void list_init(struct list *list) {
    list->next = list;
    list->prev = list;
}

#define DEFINE_LIST(name) \
    struct list name = INIT_LIST(name);


#define LIST_ELEMENT struct list _list

#define list_empty(l) ((l) == (l)->next)

#define list_entry(ptr, type, member)   container_of(ptr, type, member)

static inline struct list *list_tail(struct list *head) {
    return head->prev;
}

/* Insert new list element between two known elements */
static inline void list_insert(struct list *new, struct list *before,
        struct list *after) {
    before->next = new;
    new->prev = before;
    new->next = after;
    after->prev = new;
}

/* Insert item before insert_point */
static inline void list_insert_before(struct list *item, struct list *insert_point) {
    list_insert(item, insert_point->prev, insert_point);
}

/* Insert item after insert_point */
static inline void list_insert_after(struct list *item, struct list *insert_point) {
    list_insert(item, insert_point, insert_point->next);
}

/* Adds new to front of list head */
static inline void list_add_head(struct list *new, struct list *head) {
    list_insert(new, head, head->next);
}

/* Adds new to end of list head */
static inline void list_add_tail(struct list *new, struct list *head) {
    list_insert(new, head->prev, head);
}

#define list_add    list_add_head

/* Remove element from list, by connecting elements before and after */
static inline void list_remove(struct list *element) {
    struct list *before = element->prev;
    struct list *after = element->next;

    before->next = after;
    after->prev = before;
}

static inline struct list *list_pop_tail(struct list *head) {
    struct list *tail = list_tail(head);
    list_remove(tail);
    return tail;
}

/* head is pointer to list head, which has the first element
 * in the list as next */
static inline struct list *list_pop_head(struct list *head) {
    struct list *element = head->next;
    list_remove(element);
    return element;
}

/* Pop defaults to tail */
#define list_pop(h) list_pop_tail(h)

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

#endif
