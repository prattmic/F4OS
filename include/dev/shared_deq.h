/*
 * Copyright (C) 2013, 2014 F4OS Authors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef DEV_SHARED_DEQ_H_INCLUDED
#define DEV_SHARED_DEQ_H_INCLUDED

/* Needs list.h */

typedef struct shared_deq {
    LIST_ELEMENT;
    struct mutex mut;
} shared_deq_t;


#define DEFINE_SHARED_DEQ(name)         \
    struct shared_deq name = {          \
        ._list = INIT_LIST(name._list), \
        .mut = INIT_MUTEX,          \
    };


#define sdeq_empty(sdeq) (list_empty(&(sdeq->_list)))

#define sdeq_add(sdeq, s) __sdeq_add(sdeq, &(s->_list))


#define sdeq_entry(sdeq, type) container_of(sdeq, type, _list)

static inline void __sdeq_add(shared_deq_t *sdeq, list_t *l) {
    acquire(&sdeq->mut);
    list_add(l, &sdeq->_list);
    release(&sdeq->mut);
}

/*
#define sdeq_pop(sdeq, type) ({                     \
    struct list* __mptr = __sdeq_pop(sdeq);         \
    type* __ret;                                    \
    if (__mptr) {                                   \
        __ret = container_of(__mptr, type, _list);  \
    }                                               \
    else                                            \
        __ret = NULL;                               \
    __ret;})
*/

static inline list_t *__sdeq_pop(shared_deq_t *sdeq) {
    list_t *ret;
    acquire(&sdeq->mut);
    if(sdeq_empty(sdeq)) {
        release(&sdeq->mut);
        return NULL;
    }
    ret = list_pop(&sdeq->_list);
    release(&sdeq->mut);
    return ret;
}

/* NOT INHERENTLY THREADSAFE */
#define for_each_in_sdeq(curr, sdeq, member) for_each(curr, &sdeq._list, member)

#endif
