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

/*
 * COLLECTION - Abstraction for a group of objs - Michael Yenik
 *
 * Collections allow objs to be grouped together and have nice common API for
 * simple things like "add an obj to this group" and "iterate through all
 * objs in this collection".
 *
 * Collections are kind of like an iterator for your data structure, baked in
 * with add/delete functions.
 */

#ifndef KERNEL_COLLECTION_H_INCLUDED
#define KERNEL_COLLECTION_H_INCLUDED

#include <list.h>
#include <kernel/obj.h>
#include <kernel/reentrant_mutex.h>

struct collection {
    struct reentrant_mutex lock;
    struct list list;
    struct list *curr;
};

/**
 * Statically initialize collection
 *
 * At compile-time, statically initialize a struct collection
 * for use.
 *
 * struct collection c = INIT_COLLECTION(c);
 *
 * @param c symbol name of collection
 */
#define INIT_COLLECTION(c) { .lock = INIT_REENTRANT_MUTEX, \
                             .list = INIT_LIST((c).list), \
                             .curr = NULL, \
                           }

/**
 * Dynamically initialize collection
 *
 * At runtime, initialize a struct collection for use.
 *
 * @param c collection to initialize
 */
static inline void init_collection(struct collection *c) {
    init_reentrant_mutex(&c->lock);
    list_init(&c->list);
    c->curr = NULL;
}

/**
 * Lock collection access
 *
 * Lock access to this collection to the current task.  This prevents
 * other tasks from accessing the collection until collection_unlock()
 * is called.
 *
 * collection_unlock() must be called when finished with the collection.
 * collection_lock()/collection_unlock() may be called reentrantly.
 *
 * @param c collection to lock
 */
void collection_lock(struct collection *c);

/**
 * Unlock collection access
 *
 * Unlock access to this collection.
 *
 * This must be called when finished with the collection, only after calling
 * collection_lock().
 * collection_lock()/collection_unlock() may be called reentrantly.
 *
 * @param c collection to unlock
 */
void collection_unlock(struct collection *c);

/**
 * Begin iteration over items in collection
 *
 * Prepare for and begin iteration over all items in the collection.
 * The first item in the collection is returned.  Call collection_next()
 * to step through the remaining items, until it returns NULL, indicating
 * the end of the items.
 *
 * If iterating through the entire collection (until collection_next()
 * returns NULL), no cleanup action is necessary.  If ending iteration
 * early, collection_stop() must be called.
 *
 * @param c collection to iterate over
 * @returns first item in collection, or NULL if collection is empty
 */
struct obj *collection_iter(struct collection *c);

/**
 * End iteration early
 *
 * Must called to stop collection iteration before collection_next() returns
 * NULL.  It must not be called once collection_next() returns NULL.
 *
 * @param c collection to end iteration of
 */
void collection_stop(struct collection *c);

/**
 * Next item in collection iteration
 *
 * Return the next item in collection.
 * collection_iter() must be called first to begin iteration.
 *
 * Automatically perform iteration cleanup when reaching the end of the
 * items in the collection (and returning NULL).  If exiting iteration
 * early, collection_stop() must be called.
 *
 * @param c collection iterating over
 * @returns next item in collection, or NULL at end of collection
 */
struct obj *collection_next(struct collection *c);

/**
 * Add obj to collection
 *
 * @param c collection to add to
 * @param o obj to add
 * @returns 0 on success, negative on error
 */
int collection_add(struct collection *c, struct obj *o);

/**
 * Remove obj from collection
 *
 * obj to be removed must be a member of the collection!
 *
 * @param c collection to remove from
 * @param o obj to remove
 * @returns 0 on success, negative on error
 */
int collection_del(struct collection *c, struct obj *o);

/**
 * Find member by name
 *
 * Find a collection member by its obj name.  The first obj matching the
 * provided name will be returned.
 *
 * @param c collection to search
 * @param name  name to find
 * @returns first obj in collection with matching name
 */
struct obj *collection_get_by_name(struct collection *c, const char *name);

#endif
