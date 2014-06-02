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
#include <kernel/fault.h>
#include <kernel/reentrant_mutex.h>

/**
 * Determine if iteration is in progress
 *
 * @param c collection to check
 * @returns 1 if iteration is in progress, 0 otherwise
 */
static int collection_iterating(struct collection *c) {
    int iterating = 0;

    collection_lock(c);
    if (c->curr) {
        iterating = 1;
    }
    collection_unlock(c);

    return iterating;
}

void collection_lock(struct collection *c) {
    WARN_ON(!c);

    if (c) {
        reentrant_acquire(&c->lock);
    }
}

void collection_unlock(struct collection *c) {
    WARN_ON(!c);

    if (c) {
        reentrant_release(&c->lock);
    }
}

struct obj *collection_iter(struct collection *c) {
    if (!c) {
        return NULL;
    }

    collection_lock(c);

    if(list_empty(&c->list)) {
        collection_unlock(c);
        return NULL;
    }

    c->curr = c->list.next;
    return container_of(c->curr, struct obj, list);
}

void collection_stop(struct collection *c) {
    WARN_ON(!c);

    if (c) {
        WARN_ON(!collection_iterating(c));

        c->curr = NULL;
        collection_unlock(c);
    }
}

struct obj *collection_next(struct collection *c) {
    if (!c) {
        return NULL;
    }

    if (!collection_iterating(c)) {
        return NULL;
    }

    c->curr = c->curr->next;

    if(c->curr == &c->list) {
        collection_stop(c);
        return NULL;
    }

    return container_of(c->curr, struct obj, list);
}

int collection_add(struct collection *c, struct obj *o) {
    if (!c || !o) {
        return -1;
    }

    collection_lock(c);
    list_add(&o->list, &c->list);
    collection_unlock(c);

    return 0;
}

int collection_del(struct collection *c, struct obj *o) {
    if (!c || !o) {
        return -1;
    }

    /* TODO: verify obj is actually in *this* collection's list */

    collection_lock(c);
    list_remove(&o->list);
    collection_unlock(c);

    return 0;
}

struct obj *collection_get_by_name(struct collection *c, const char *name) {
    struct obj *ret = NULL;
    struct obj *curr;

    if (!name || !c) {
        return NULL;
    }

    collection_lock(c);
    list_for_each_entry(curr, &c->list, list) {
        if(!strcmp(curr->name, name)) {
            ret = curr;
            goto out;
        }
    }

out:
    collection_unlock(c);
    return ret;
}
