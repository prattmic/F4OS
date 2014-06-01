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

#include <string.h>
#include <kernel/collection.h>
#include <kernel/reentrant_mutex.h>

struct obj *collection_iter(struct collection *c) {
    reentrant_acquire(&c->lock);

    if(list_empty(&c->list)) {
        reentrant_release(&c->lock);
        return NULL;
    }

    c->curr = c->list.next;
    return container_of(c->curr, struct obj, list);
}

void collection_stop(struct collection *c) {
    reentrant_release(&c->lock);
}

struct obj *collection_next(struct collection *c) {
    c->curr = c->curr->next;

    if(c->curr == &c->list) {
        collection_stop(c);
        return NULL;
    }

    return container_of(c->curr, struct obj, list);
}

void collection_add(struct collection *c, struct obj *o) {
    reentrant_acquire(&c->lock);
    list_add(&c->list, &o->list);
    reentrant_release(&c->lock);
}

void collection_del(struct collection *c, struct obj *o) {
    reentrant_acquire(&c->lock);
    list_remove(&o->list);
    reentrant_release(&c->lock);
}

struct obj *get_by_name(char *name, struct collection *c) {
    struct obj *curr;

    reentrant_acquire(&c->lock);
    list_for_each_entry(curr, &c->list, list) {
        if(!strncmp(curr->name, name, 32)) {
            reentrant_release(&c->lock);
            return curr;
        }
    }
    reentrant_release(&c->lock);
    return NULL;
}
