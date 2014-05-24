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

#ifndef DEV_ADC_H_INCLUDED
#define DEV_ADC_H_INCLUDED

#include <stdint.h>
#include <linker_array.h>
#include <kernel/obj.h>

struct adc {
    struct obj  *gpio;
    struct obj  obj;
    void        *priv;
};

/* Takes obj and returns containing struct adc */
static inline struct adc *to_adc(struct obj *o) {
    return (struct adc *) container_of(o, struct adc, obj);
}

struct adc_ops {
    /**
     * Initialize ADC
     *
     * Perform any initialization necessary to prepare the ADC for reading.
     * This method will automatically be called one the first call to other
     * methods, if not called explicitly.
     *
     * @param adc       ADC output to initialize
     * @return 0 on success, negative on error
     */
    int         (*init)(struct adc *);
    /**
     * Read raw value from ADC
     *
     * Reads an input value from the ADC.  The units of this value
     * are implementation dependent.
     *
     * @param adc       ADC output to read
     * @return latest value read from ADC
     */
    uint32_t    (*read_raw)(struct adc *);
    /**
     * Implementation specific destructor
     *
     * Performs any implementation specific destruction necessary when freeing
     * the ADC object.
     *
     * @param adc  ADC object being destroyed
     * @returns zero on success, negative on error
     */
    int     (*dtor)(struct adc *);
};

extern struct class adc_class;

/**
 * Get ADC object
 *
 * Get ADC object for passed in GPIO obj.  A ADC object is allocated
 * from a suitable driver, and will read inputs from the passed GPIO.
 *
 * The ADC driver to use can be specified with the driver argument.
 * If this argument is NULL, then any suitable driver will be used.
 *
 * The reference count on the GPIO will be incremented when the object is
 * instantiated, and decremented when the object is destroyed.
 *
 * Once allocated, the output must be enabled with the enable method.  Once
 * finish, the object should be freed with adc_put().
 *
 * @param gpio      GPIO to get ADC object for
 * @param driver    Driver name to use as ADC driver
 * @returns ADC object on success, NULL on error
 */
struct obj *adc_get(struct obj *gpio, const char *driver);

/**
 * Return ADC object
 *
 * Release a ADC object received by adc_get.  This must be called when
 * you are finished using the ADC.
 *
 * @param obj   ADC obj received from adc_get to release
 */
static inline void adc_put(struct obj *o) {
    obj_put(o);
}

struct adc_driver {
    /* FDT compatible string this driver is compatible with */
    const char *compat;
    /*
     * Get ADC object
     *
     * Use the ADC described in the FDT at offset to create an ADC
     * object for the provided GPIO.
     *
     * @param fdt       pointer to FDT blob
     * @param offset    Node offset of ADC device
     * @param gpio      GPIO to get ADC object for
     * @returns ADC object on success, NULL on error
     */
    struct obj *(*adc_get)(const void *fdt, int offset, struct obj *gpio);
};

/*
 * Declare an available ADC driver
 *
 * Sets up a struct definition for an ADC driver, which should
 * be assigned to.
 *
 * Example:
 * DECLARE_ADC_DRIVER(example) = {
 *     .compat = "mycorp,example",
 *     .adc_get = &example_adc_get,
 * };
 */
#define DECLARE_ADC_DRIVER(name) \
    struct adc_driver _adc_driver_##name LINKER_ARRAY_ENTRY(adc_drivers)

#endif
