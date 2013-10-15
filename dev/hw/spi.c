/*
 * Copyright (C) 2013 F4OS Authors
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

#include <dev/hw/spi.h>
#include <kernel/class.h>
#include <mm/mm.h>

static void spi_dtor(struct obj *o);

struct obj_type spi_type_s  = {
    .offset = offset_of(struct spi, obj),
    .dtor = &spi_dtor,
};

struct class spi_class = INIT_CLASS(spi_class, "spi", &spi_type_s);

static void spi_dtor(struct obj *o) {
    struct spi *spi;
    struct spi_ops *ops;

    assert_type(o, &spi_type_s);
    spi = to_spi(o);
    ops = (struct spi_ops *)o->ops;
    ops->deinit(spi);

    if (spi->priv) {
        kfree(spi->priv);
    }
}
