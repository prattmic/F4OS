#include <stdlib.h>
#include <kernel/sched.h>
#include <kernel/semaphore.h>
#include <dev/resource.h>

#include <dev/buf_stream.h>

struct buf_stream {
    char *buf;
};

/* Warning! Buffer streams use the buffer you pass them, they do not copy them! */
rd_t open_buf_stream(char *buf);
char buf_stream_read(void *env);
void buf_stream_write(char c, void *env);
void buf_stream_close(resource *resource);

rd_t open_buf_stream(char *buf) {
    resource *new_r = malloc(sizeof(resource));

    struct buf_stream *env = malloc(sizeof(struct buf_stream)); 
    env->buf = buf;

    new_r->env    = env;
    new_r->writer = &buf_stream_write;
    new_r->reader = &buf_stream_read;
    new_r->closer = &buf_stream_close;
    new_r->sem    = malloc(sizeof(semaphore));
    init_semaphore(new_r->sem);

    add_resource(curr_task->task, new_r);

    return curr_task->task->top_rd - 1;
}

char buf_stream_read(void *env) {
    return *((struct buf_stream *) env)->buf == '\0' ? '\0' : *((struct buf_stream *) env)->buf++;
}

void buf_stream_write(char c, void *env) {
    *((struct buf_stream *) env)->buf++ = c;
    *((struct buf_stream *) env)->buf = '\0';
}

void buf_stream_close(resource *resource) {
    acquire(resource->sem);
    free(resource->env);
    free((void*) resource->sem);
}
