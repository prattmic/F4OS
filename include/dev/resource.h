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

#ifndef DEV_RESOURCE_H_INCLUDED
#define DEV_RESOURCE_H_INCLUDED

#include <stdint.h>

struct task_t;
typedef struct task_t task_t;

struct mutex;

typedef struct resource {
    /* Separate read and write mutexes available.
     * For resources that require one lock for all
     * actions, these should be set equal to one another. */
    volatile struct mutex       *read_mut;
    volatile struct mutex       *write_mut;
    void                        *env;
    int                         (*writer)(char, void*);
    int                         (*swriter)(char*, void*);   /* Optional string writer function used by swrite if available  */
    char                        (*reader)(void*, int*);
    int                         (*closer)(struct resource*);
} resource;

resource *create_new_resource(void) __attribute__((section(".kernel")));
void task_resource_setup(task_t *task) __attribute__((section(".kernel")));

/*
 * Close initialized resource
 *
 * @returns 0 on success, negative on error
 */
int resource_close(struct resource *resource);

/*
 * Create a char_device wrapper for resource
 *
 * Allocate and get a char_dev obj, which acts as an abstract
 * interface to the passed resource.
 *
 * When finished, use obj_put() to free structures.
 *
 * @param resource  Resource to wrap
 * @return Character device wrapper, or NULL on error
 */
struct char_device *resource_to_char_device(struct resource *resource);

/*
 * Check if two char_devices share the same underlying resource
 *
 * @returns true if d1 and d2 have the same underlying resource, else false
 */
int resource_char_device_equal(struct char_device *d1,
                               struct char_device *d2);

/* Determine default stdout/stderr */
extern struct resource usart_resource;
extern struct resource usb_resource;

#define USART_DEV   &usart_resource
#define USB_DEV     &usb_resource

#if defined(CONFIG_STDOUT_USART)
#define STDOUT_DEV        USART_DEV
#elif defined(CONFIG_STDOUT_USB)
#define STDOUT_DEV        USB_DEV
#else
#define STDOUT_DEV        NULL
#endif

#if defined(CONFIG_STDERR_USART)
#define STDERR_DEV        USART_DEV
#elif defined(CONFIG_STDERR_USB)
#define STDERR_DEV        USB_DEV
#else
#define STDERR_DEV        NULL
#endif

#endif
