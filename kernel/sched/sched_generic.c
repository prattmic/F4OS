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

#include <stdio.h>
#include <kernel/sched.h>
#include <kernel/mutex.h>

/* Functions common to all scheduler implementations */

/*
 * Setup new task IO
 *
 * This means copying stdin/stdout/stderr from the current task.
 */
static void task_io_setup(task_t *task) {
    if (stdin) {
        obj_get(&stdin->obj);
    }
    task->_stdin = stdin;

    if (stdout) {
        obj_get(&stdout->obj);
    }
    task->_stdout = stdout;

    if (stderr) {
        obj_get(&stderr->obj);
    }
    task->_stderr = stderr;
}

/* Do non-scheduler setup for new task */
void generic_task_setup(task_t *task) {
    task_io_setup(task);
    task_mutex_setup(task);
}
