#include <string.h>
#include <kernel/collection.h>
#include <stdio.h>

struct obj *collection_iter(struct collection *c) {
    acquire(&c->lock);

    if(list_empty(&c->list)) {
        release(&c->lock);
        return NULL;
    }

    c->curr = c->list.next;
    return container_of(c->curr, struct obj, list);
}

void collection_stop(struct collection *c) {
    release(&c->lock);
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
    acquire(&c->lock);
    list_add(&c->list, &o->list);
    release(&c->lock);
}

void collection_del(struct collection *c, struct obj *o) {
    acquire(&c->lock);
    list_remove(&o->list);
    release(&c->lock);
}

struct obj *get_by_name(char *name, struct collection *c) {
    struct obj *curr;

    acquire(&c->lock);
    list_for_each_entry(curr, &c->list, list) {
        if(!strncmp(curr->name, name, 32)) {
            release(&c->lock);
            return curr;
        }
    }
    release(&c->lock);
    return NULL;
}
