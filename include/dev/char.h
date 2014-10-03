/*
 * Copyright (C) 2014 F4OS Authors
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

#ifndef DEV_CHAR_H_INCLUDED
#define DEV_CHAR_H_INCLUDED

/*
 * A character device is an abstract interface to devices that read and write
 * raw bytes.  Character devices are wrappers around a concrete base obj, such
 * as a UART or USB CDC device, and are thus not part of the standard "device"
 * subsystem.
 *
 * Upon creation, the base character device should already be initialized and
 * ready to use.  If unnecessary, the base obj may be NULL.
 */

#include <linker_array.h>
#include <stdint.h>
#include <kernel/obj.h>

struct char_device {
    struct obj  obj;
    struct obj  *base;
    void        *priv;
};

#define to_char_device(__obj) container_of((__obj), struct char_device, obj)

struct char_ops {
    /**
     * Read from device
     *
     * Read up to num bytes from the device into buf.  read() does not block.
     * It will return immediately if blocking would be required.  As such,
     * multiple calls may be necessary to complete reading.
     *
     * @param char_device Character device to read from
     * @param buf   Buffer to read into.  Must hold at least num bytes
     * @param num   Number of bytes to read
     *
     * @returns number of bytes read, or negative on error
     */
    int     (*read)(struct char_device *, char *, size_t);
    /**
     * Write to device
     *
     * Write up to num bytes to the device from buf.  write() does not block.
     * It will return immediately if blocking would be required.  As such,
     * multiple calls may be necessary to complete reading.
     *
     * @param char_device Character device to write to
     * @param buf   Buffer to write from.  Must have at least num bytes
     * @param num   Number of bytes to write
     *
     * @returns number of bytes written, or negative on error
     */
    int     (*write)(struct char_device *, char *, size_t);
    /**
     * Cleanup internal structures
     *
     * Frees internal data structures and prepares the obj to be destroyed.
     * Does not put the base obj.  This is done by the char_device destructor.
     *
     * Should only be called from the char_device destructor.
     *
     * @param char_device Character device object to cleanup
     *
     * @returns zero on success, negative on error
     */
    int     (*_cleanup)(struct char_device *);
};

/**
 * Create a new char_device structure
 *
 * Reserves base obj by performing an obj_get().  If base obj is no longer
 * needed on its own once char_device is created, perform an obj_put().
 *
 * Destroyed by char_device destructor.
 *
 * @param base  Base concrete character device obj.  May be NULL.
 * @param ops   Character device ops
 */
struct char_device *char_device_create(struct obj *base,
                                       struct char_ops *ops);

/**
 * "Cast" obj to a char_device
 *
 * For obj-types providing a conversion function, provide a char_device
 * wrapper for the base obj, allowing its use as a char_device.
 *
 * The conversion function will perform a get on the base obj, perform an
 * obj_put() if the base obj is no longer directly needed.
 *
 * @param base  Base obj to convert to char_device
 * @returns char_device wrapper for base obj, NULL if it cannot be cast
 */
struct char_device *char_device_cast(struct obj *base);

struct char_conversion {
    /* Type casting function casts to char_device */
    struct obj_type *type;
    /*
     * "Cast" obj to char_device
     *
     * Provide a char_device wrapper for obj of type type.  The base obj
     * should be reserved using obj_get() if it is required in the
     * char_device.
     *
     * @param base  Obj of type type to cast
     * @returns char_device for base obj, NULL on error
     */
    struct char_device *(*cast)(struct obj *);
};

/*
 * Declare an available char conversion function
 *
 * Sets up a struct definition for a char_device conversion function,
 * which should be assigned to.
 *
 * Example:
 * DECLARE_CHAR_CONVERSION(example) = {
 *     .type = &example_type_s,
 *     .cast = example_char_cast,
 * };
 */
#define DECLARE_CHAR_CONVERSION(name) \
    struct char_conversion _char_conversion_##name \
        LINKER_ARRAY_ENTRY(char_conversions)

/**
 * Get instance of a char device
 *
 * Identical to device_get(), but casts the result to a char_device.
 *
 * @param name  Name of device to get
 * @return  Reference to object, or NULL on error
 */
struct char_device *char_device_get(const char *name);

/**
 * Put an instance of a char device
 *
 * When finished with a device, return it to the system,
 * possibly freeing its resources and destroying its object completely.
 *
 * The device must not be accessed after calling this function
 *
 * @param device    Device to put
 */
static inline void char_device_put(struct char_device *dev) {
    obj_put(&dev->obj);
}

/**
 * Determine if two char_devices are backed by the same base device
 *
 * @returns positive if the two devices share a base device, 0 otherwise.
 */
int char_device_base_equal(const struct char_device *d1,
                           const struct char_device *d2);

/**
 * Initialize global default stdin/stdout/stderr
 *
 * Called before task switching begins to setup the global stdin/stdout/stderr
 * based on the provided config.  If any cannot be acquired, the system will
 * panic.
 */
void init_io(void);

#endif
