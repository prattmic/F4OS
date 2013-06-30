#include <stddef.h>
#include <stdlib.h>
#include <mm/mm.h>
#include <kernel/sched.h>
#include <kernel/semaphore.h>
#include <kernel/fault.h>
#include <dev/resource.h>

#include <dev/buf_stream.h>

struct buf_stream {
    char *buf;
    uint32_t i;
    uint32_t len;
};

static char buf_stream_read(void *env, int *error) {
    if (error != NULL) {
        *error = 0;
    }

    struct buf_stream *stream = (struct buf_stream *) env;

    if ((stream->i >= stream->len) || (stream->buf[stream->i] == '\0')) {
        return '\0';
    }

    return stream->buf[stream->i++];
}

static int buf_stream_write(char c, void *env) {
    struct buf_stream *stream = (struct buf_stream *) env;

    /* Always leave room for NULL byte at end of buffer */
    if (stream->i >= stream->len - 1) {
        return 0;
    }

    stream->buf[stream->i++] = c;
    stream->buf[stream->i] = '\0';
    return 1;
}

static int buf_stream_close(resource *resource) {
    acquire_for_free(resource->read_sem);
    free(resource->env);
    free((void*) resource->read_sem);
    return 0;
}

rd_t open_buf_stream(char *buf, uint32_t len) {
    rd_t ret;

    struct buf_stream *env = malloc(sizeof(struct buf_stream));
    if (!env) {
        ret = -1;
        goto err;
    }

    resource *new_r = create_new_resource();
    if (!new_r) {
        ret = -1;
        goto err_free_env;
    }

    env->buf = buf;
    env->i = 0;
    env->len = len;

    new_r->env    = env;
    new_r->writer = &buf_stream_write;
    new_r->swriter = NULL;
    new_r->reader = &buf_stream_read;
    new_r->closer = &buf_stream_close;
    new_r->read_sem = malloc(sizeof(semaphore));
    if (new_r->read_sem) {
        init_semaphore(new_r->read_sem);
    }
    else {
        ret = -1;
        goto err_free_new_r;
    }
    new_r->write_sem = new_r->read_sem;

    ret = add_resource(curr_task, new_r);
    if (ret < 0) {
        goto err_free_new_r;
    }

    return ret;

err_free_new_r:
    kfree(new_r);
err_free_env:
    kfree(env);
err:
    printk("OOPS: Unable to open buffer stream.\r\n");
    return ret;
}
