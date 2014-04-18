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

#ifndef DEV_CLOCKS_H_INCLUDED
#define DEV_CLOCKS_H_INCLUDED

#include <linker_array.h>

struct clock_driver {
    /* FDT compatible string this driver is compatible with */
    const char *compat;
    /*
     * Enable clocks
     *
     * Enable clocks described by the passed clocks property.
     *
     * @param fdt   pointer to FDT blob
     * @param offset    Node offset containing clocks property
     * @param prop  String name of clocks property
     * @returns 0 on success, negative on error
     */
    int (*enable)(const void *fdt, int offset, const char *prop);
    /*
     * Disable clocks
     *
     * Disable clocks described by the passed clocks property.
     *
     * @param fdt   pointer to FDT blob
     * @param offset    Node offset containing clocks property
     * @param prop  String name of clocks property
     * @returns 0 on success, negative on error
     */
    int (*disable)(const void *fdt, int offset, const char *prop);
    /*
     * Select clock parameter
     *
     * Set a clock parameter for the clock described by the passed
     * clock-select property.  This is defined by the clock, but is
     * general either a clock source or speed.
     *
     * TODO: Support more than just the first entry.
     *
     * @param fdt   pointer to FDT blob
     * @param offset    Node offset containing clocks property
     * @param prop  String name of clock-select property
     * @param param Parameter value to set
     * @returns 0 on success, negative on error
     */
    int (*set_param)(const void *fdt, int offset, const char *name,
                     unsigned int param);
};

/*
 * Declare an available clock driver
 *
 * Sets up a struct definition for a clock driver, which should
 * be assigned to.
 *
 * Example:
 * DECLATE_CLOCK_DRIVER(example) = {
 *     .compat = "mycorp,example",
 *     .enable = &example_enable,
 *     .disable = &example_enable,
 *     .set_param = &example_set_param,
 * };
 */
#define DECLARE_CLOCK_DRIVER(name) \
    struct clock_driver _clock_driver_##name LINKER_ARRAY_ENTRY(clocks)

/*
 * Enable clocks
 *
 * Enable clocks described by the passed clocks property.
 *
 * TODO: Enable more than just the first entry.
 *
 * @param fdt   pointer to FDT blob
 * @param offset    Node offset containing clocks property
 * @param prop  String name of clocks property
 * @returns 0 on success, negative on error
 */
int clocks_enable(const void *fdt, int offset, const char *name);

/*
 * Disable clocks
 *
 * Disable clocks described by the passed clocks property.
 *
 * TODO: Enable more than just the first entry.
 *
 * @param fdt   pointer to FDT blob
 * @param offset    Node offset containing clocks property
 * @param prop  String name of clocks property
 * @returns 0 on success, negative on error
 */
int clocks_disable(const void *fdt, int offset, const char *name);

/*
 * Select clock parameter
 *
 * Set a clock parameter for the clock described by the passed
 * clock property.  This is defined by the clock, but is
 * generally either a clock source or speed.
 *
 * TODO: Support more than just the first entry.
 *
 * @param fdt   pointer to FDT blob
 * @param offset    Node offset containing clocks property
 * @param prop  String name of clock-select property
 * @param param Parameter value to set
 * @returns 0 on success, negative on error
 */
int clocks_set_param(const void *fdt, int offset, const char *name,
                     unsigned int param);

#endif
