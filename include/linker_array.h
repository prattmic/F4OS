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

#ifndef LINKER_ARRAY_H_INCLUDED
#define LINKER_ARRAY_H_INCLUDED

/*
 * Build "arrays" across compilation units, at link time.
 *
 * Using the LINKER_ARRAY macros, symbols can be combined into a single
 * "array" at link time.  These symbols are placed in a shared section,
 * whose start and end addresses can be accessed, allowing them to be
 * treated as a contiguous array.
 *
 * This is useful for generating lists of available drivers, programs, etc,
 * where components may or may not be included in a given build configuration.
 * Each component can define its array entry in its compilation unit.  If the
 * compilation unit is not built, the item is not included in the array.
 *
 * Items in the arrays are expected to be of the same type.  There is no
 * enforcement of this, however ordering of the array is undefined, so there
 * may be no way to distinguish different items without a common type across
 * the array.
 *
 * Arrays are placed into read-only memory, thus entries cannot be modified
 * at run-time.
 */

#include <compiler.h>
#include <stdint.h>

/*
 * Declare a new linker array
 *
 * Generates symbols used to reference the start and end of a linker array.
 * This must be called in a single compilation unit to enable references
 * across compilation units.
 *
 * @param _name Name of the linker array
 */
#define LINKER_ARRAY_DECLARE(_name)   \
    int _linker_array_##_name##_start[0] \
        __attribute__((section(".linker_array." STRINGIFY(_name) ".0"))); \
    int _linker_array_##_name##_end[0]  \
        __attribute__((section(".linker_array." STRINGIFY(_name) ".2")));

/*
 * Add entry to linker array
 *
 * Declares a variable as a member of a linker array.  Added as a
 * variable attribute, as below:
 *  struct array_entry entry1 LINKER_ARRAY_ENTRY(array) = { ... };
 *
 * @param _name Name of the linker array entry belongs to
 */
#define LINKER_ARRAY_ENTRY(_name)    \
    __attribute__((section(".linker_array." STRINGIFY(_name) ".1"), unused))

/*
 * Get linker array start address
 *
 * @returns void * address of beginning of linker array
 */
#define LINKER_ARRAY_START(_name)   ({  \
    extern int _linker_array_##_name##_start[0];    \
    (void *)&_linker_array_##_name##_start[0];  \
})

/*
 * Get linker array end address
 *
 * This is the address immediately following the last entry.
 *
 * @returns void * address of end of linker array
 */
#define LINKER_ARRAY_END(_name)   ({  \
    extern int _linker_array_##_name##_end[0];    \
    (void *)&_linker_array_##_name##_end[0];  \
})

/*
 * Loop over all entries in linker array
 *
 * Address of an array entry is assigned to _current on each
 * iteration of the loop.
 *
 * Entries in array are all expected to have the typeof(*_current)
 * for this for loop to behave properly.
 *
 * @param _name Name of linker array to loop over
 * @param _current  Pointer to entry in array in each iteration
 */
#define LINKER_ARRAY_FOR_EACH(_name, _current)  \
    for (_current = LINKER_ARRAY_START(_name);  \
         ((uintptr_t)(_current)) < ((uintptr_t)LINKER_ARRAY_END(_name));    \
         _current += 1)

#endif
