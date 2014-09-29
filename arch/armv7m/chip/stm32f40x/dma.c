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

#include <libfdt.h>
#include <stdlib.h>
#include <arch/chip/dma.h>
#include <arch/chip/rcc.h>
#include <dev/device.h>
#include <dev/fdtparse.h>
#include <dev/raw_mem.h>
#include <kernel/class.h>
#include <kernel/init.h>
#include <kernel/obj.h>
#include <mm/mm.h>

#define STM32F4_DMA_COMPAT "stmicro,stm32f407-dma"

/* Upper byte is stream, lower byte is channel */
static stm32f4_dma_handle_t handle_create(uint8_t stream, uint8_t channel) {
    return ((stream & 0xff) << 8) | (channel & 0xff);
}

static uint8_t handle_stream(stm32f4_dma_handle_t handle) {
    return handle >> 8;
}

static uint32_t stream_transaction_complete(struct stm32f4_dma *dma, uint8_t stream) {
    switch (stream) {
    case 0:
        return raw_mem_read(&dma->regs->LISR) & DMA_LISR_TCIF0;
    case 1:
        return raw_mem_read(&dma->regs->LISR) & DMA_LISR_TCIF1;
    case 2:
        return raw_mem_read(&dma->regs->LISR) & DMA_LISR_TCIF2;
    case 3:
        return raw_mem_read(&dma->regs->LISR) & DMA_LISR_TCIF3;
    case 4:
        return raw_mem_read(&dma->regs->HISR) & DMA_HISR_TCIF4;
    case 5:
        return raw_mem_read(&dma->regs->HISR) & DMA_HISR_TCIF5;
    case 6:
        return raw_mem_read(&dma->regs->HISR) & DMA_HISR_TCIF6;
    case 7:
        return raw_mem_read(&dma->regs->HISR) & DMA_HISR_TCIF7;
    default:
        return 0;
    }
}

static void stream_clear_flags(struct stm32f4_dma *dma, uint8_t stream) {
    switch (stream) {
    case 0:
        raw_mem_set_bits(&dma->regs->LIFCR,
                         DMA_LIFCR_CFEIF0 | DMA_LIFCR_CDMEIF0 |
                         DMA_LIFCR_CTEIF0 | DMA_LIFCR_CHTIF0 |
                         DMA_LIFCR_CTCIF0);
        break;
    case 1:
        raw_mem_set_bits(&dma->regs->LIFCR,
                         DMA_LIFCR_CFEIF1 | DMA_LIFCR_CDMEIF1 |
                         DMA_LIFCR_CTEIF1 | DMA_LIFCR_CHTIF1 |
                         DMA_LIFCR_CTCIF1);
        break;
    case 2:
        raw_mem_set_bits(&dma->regs->LIFCR,
                         DMA_LIFCR_CFEIF2 | DMA_LIFCR_CDMEIF2 |
                         DMA_LIFCR_CTEIF2 | DMA_LIFCR_CHTIF2 |
                         DMA_LIFCR_CTCIF2);
        break;
    case 3:
        raw_mem_set_bits(&dma->regs->LIFCR,
                         DMA_LIFCR_CFEIF3 | DMA_LIFCR_CDMEIF3 |
                         DMA_LIFCR_CTEIF3 | DMA_LIFCR_CHTIF3 |
                         DMA_LIFCR_CTCIF3);
        break;
    case 4:
        raw_mem_set_bits(&dma->regs->HIFCR,
                         DMA_HIFCR_CFEIF4 | DMA_HIFCR_CDMEIF4 |
                         DMA_HIFCR_CTEIF4 | DMA_HIFCR_CHTIF4 |
                         DMA_HIFCR_CTCIF4);
        break;
    case 5:
        raw_mem_set_bits(&dma->regs->HIFCR,
                         DMA_HIFCR_CFEIF5 | DMA_HIFCR_CDMEIF5 |
                         DMA_HIFCR_CTEIF5 | DMA_HIFCR_CHTIF5 |
                         DMA_HIFCR_CTCIF5);
        break;
    case 6:
        raw_mem_set_bits(&dma->regs->HIFCR,
                         DMA_HIFCR_CFEIF6 | DMA_HIFCR_CDMEIF6 |
                         DMA_HIFCR_CTEIF6 | DMA_HIFCR_CHTIF6 |
                         DMA_HIFCR_CTCIF6);
        break;
    case 7:
        raw_mem_set_bits(&dma->regs->HIFCR,
                         DMA_HIFCR_CFEIF7 | DMA_HIFCR_CDMEIF7 |
                         DMA_HIFCR_CTEIF7 | DMA_HIFCR_CHTIF7 |
                         DMA_HIFCR_CTCIF7);
        break;
    }
}

static stm32f4_dma_handle_t stm32f4_dma_class_allocate(struct stm32f4_dma *dma,
                                                       uint8_t stream,
                                                       uint8_t channel) {
    stm32f4_dma_handle_t handle = STM32F4_DMA_ERROR;

    if (!dma) {
        return STM32F4_DMA_ERROR;
    }

    /* Stream and channel in range? */
    if (stream > 7 || channel > 7) {
        return STM32F4_DMA_ERROR;
    }

    acquire(&dma->lock);

    /* Stream already taken? */
    if (dma->streams_in_use[stream]) {
        goto out;
    }

    /* Reserve stream */
    dma->streams_in_use[stream] = 1;

    /* Disable stream */
    raw_mem_clear_bits(&dma->regs->stream[stream].CR, DMA_SxCR_EN);

    /* Wait for stream to disable */
    while (raw_mem_read(&dma->regs->stream[stream].CR) & DMA_SxCR_EN) {
        /* TODO: timeout */
    }

    /* Reset stream configuration */
    raw_mem_write(&dma->regs->stream[stream].CR, 0);
    raw_mem_write(&dma->regs->stream[stream].NDTR, 0);
    raw_mem_write(&dma->regs->stream[stream].PAR, 0);
    raw_mem_write(&dma->regs->stream[stream].M0AR, 0);
    raw_mem_write(&dma->regs->stream[stream].M1AR, 0);
    raw_mem_write(&dma->regs->stream[stream].FCR, 0);

    /* Configure channel */
    raw_mem_set_mask(&dma->regs->stream[stream].CR, DMA_SxCR_CHSEL_MASK,
                     DMA_SxCR_CHSEL(channel));

    handle = handle_create(stream, channel);

out:
    release(&dma->lock);
    return handle;
}

static int stm32f4_dma_class_deallocate(struct stm32f4_dma *dma,
                                        stm32f4_dma_handle_t handle) {
    uint8_t stream = handle_stream(handle);

    if (!dma) {
        return -1;
    }

    /* Stream in range? */
    if (stream > 7) {
        return -1;
    }

    acquire(&dma->lock);

    /* Disable stream */
    raw_mem_clear_bits(&dma->regs->stream[stream].CR, DMA_SxCR_EN);

    /* Wait for stream to disable */
    while (raw_mem_read(&dma->regs->stream[stream].CR) & DMA_SxCR_EN) {
        /* TODO: timeout */
    }

    /* Release stream */
    dma->streams_in_use[stream] = 0;

    release(&dma->lock);

    return 0;
}

static int stm32f4_dma_class_configure(struct stm32f4_dma *dma,
                                       stm32f4_dma_handle_t handle,
                                       struct stm32f4_dma_config *config) {
    uint8_t stream = handle_stream(handle);

    if (!dma || !config) {
        return -1;
    }

    /* Stream in range? */
    if (stream > 7) {
        return -1;
    }

    switch (config->direction) {
    case STM32F4_DMA_DIR_PERIPH_TO_MEM:
        raw_mem_set_mask(&dma->regs->stream[stream].CR,
                         DMA_SxCR_DIR_MASK,
                         DMA_SxCR_DIR_PM);
        break;
    case STM32F4_DMA_DIR_MEM_TO_PERIPH:
        raw_mem_set_mask(&dma->regs->stream[stream].CR,
                         DMA_SxCR_DIR_MASK,
                         DMA_SxCR_DIR_MP);
        break;
    case STM32F4_DMA_DIR_MEM_TO_MEM:
        raw_mem_set_mask(&dma->regs->stream[stream].CR,
                         DMA_SxCR_DIR_MASK,
                         DMA_SxCR_DIR_MM);
        break;
    default:
        return -1;
    }

    switch (config->memory_size) {
    case 1:
        raw_mem_set_mask(&dma->regs->stream[stream].CR,
                         DMA_SxCR_MSIZE_MASK,
                         DMA_SxCR_MSIZE_BYTE);
        break;
    case 2:
        raw_mem_set_mask(&dma->regs->stream[stream].CR,
                         DMA_SxCR_MSIZE_MASK,
                         DMA_SxCR_MSIZE_HW);
        break;
    case 4:
        raw_mem_set_mask(&dma->regs->stream[stream].CR,
                         DMA_SxCR_MSIZE_MASK,
                         DMA_SxCR_MSIZE_WORD);
        break;
    default:
        return -1;
    }

    switch (config->peripheral_size) {
    case 1:
        raw_mem_set_mask(&dma->regs->stream[stream].CR,
                         DMA_SxCR_PSIZE_MASK,
                         DMA_SxCR_PSIZE_BYTE);
        break;
    case 2:
        raw_mem_set_mask(&dma->regs->stream[stream].CR,
                         DMA_SxCR_PSIZE_MASK,
                         DMA_SxCR_PSIZE_HW);
        break;
    case 4:
        raw_mem_set_mask(&dma->regs->stream[stream].CR,
                         DMA_SxCR_PSIZE_MASK,
                         DMA_SxCR_PSIZE_WORD);
        break;
    default:
        return -1;
    }

    if (config->memory_increment) {
        raw_mem_set_bits(&dma->regs->stream[stream].CR,
                         DMA_SxCR_MINC);
    }
    else {
        raw_mem_clear_bits(&dma->regs->stream[stream].CR,
                           DMA_SxCR_MINC);
    }

    if (config->peripheral_increment) {
        raw_mem_set_bits(&dma->regs->stream[stream].CR,
                         DMA_SxCR_PINC);
    }
    else {
        raw_mem_clear_bits(&dma->regs->stream[stream].CR,
                           DMA_SxCR_PINC);
    }

    if (config->circular) {
        raw_mem_set_bits(&dma->regs->stream[stream].CR,
                         DMA_SxCR_CIRC);
    }
    else {
        raw_mem_clear_bits(&dma->regs->stream[stream].CR,
                           DMA_SxCR_CIRC);
    }

    if (config->double_buffer) {
        raw_mem_set_bits(&dma->regs->stream[stream].CR,
                         DMA_SxCR_DBM);
    }
    else {
        raw_mem_clear_bits(&dma->regs->stream[stream].CR,
                           DMA_SxCR_DBM);
    }

    raw_mem_write(&dma->regs->stream[stream].PAR, config->peripheral_addr);
    raw_mem_write(&dma->regs->stream[stream].M0AR, config->mem0_addr);
    raw_mem_write(&dma->regs->stream[stream].M1AR, config->mem1_addr);

    return 0;
}

static int stm32f4_dma_class_begin_transaction(struct stm32f4_dma *dma,
                                               stm32f4_dma_handle_t handle,
                                               uint16_t num) {
    uint8_t stream = handle_stream(handle);

    if (!dma) {
        return -1;
    }

    /* Stream in range? */
    if (stream > 7) {
        return -1;
    }

    /* Cancel any ongoing transactions */
    raw_mem_clear_bits(&dma->regs->stream[stream].CR, DMA_SxCR_EN);

    /* Wait for stream to disable */
    while (raw_mem_read(&dma->regs->stream[stream].CR) & DMA_SxCR_EN) {
        /* TODO: timeout */
    }

    /* Clear stream events */
    stream_clear_flags(dma, stream);

    raw_mem_write(&dma->regs->stream[stream].NDTR, num);

    /* Start transaction */
    raw_mem_set_bits(&dma->regs->stream[stream].CR, DMA_SxCR_EN);

    return 0;
}

static int stm32f4_dma_class_transaction_complete(struct stm32f4_dma *dma,
                                                  stm32f4_dma_handle_t handle) {
    uint8_t stream = handle_stream(handle);
    uint32_t complete;

    if (!dma) {
        return -1;
    }

    /* Stream in range? */
    if (stream > 7) {
        return -1;
    }

    complete = stream_transaction_complete(dma, stream);

    if (complete) {
        /* Clear stream events */
        stream_clear_flags(dma, stream);
    }

    return !!complete;
}

static int stm32f4_dma_class_items_remaining(struct stm32f4_dma *dma,
                                             stm32f4_dma_handle_t handle) {
    uint8_t stream = handle_stream(handle);
    uint32_t items;

    if (!dma) {
        return -1;
    }

    /* Stream in range? */
    if (stream > 7) {
        return -1;
    }

    items = raw_mem_read(&dma->regs->stream[stream].NDTR) & 0xffff;

    return items;
}

static struct stm32f4_dma_ops stm32f4_dma_ops = {
    .allocate = stm32f4_dma_class_allocate,
    .deallocate = stm32f4_dma_class_deallocate,
    .configure = stm32f4_dma_class_configure,
    .begin_transaction = stm32f4_dma_class_begin_transaction,
    .transaction_complete = stm32f4_dma_class_transaction_complete,
    .items_remaining = stm32f4_dma_class_items_remaining,
};

static void stm32f4_dma_dtor(struct obj *o);

static struct obj_type stm32f4_dma_type = {
    .offset = offset_of(struct stm32f4_dma, obj),
    .dtor = &stm32f4_dma_dtor,
};

static struct class stm32f4_dma_class = INIT_CLASS(stm32f4_dma_class,
                                                   "stm32f4_dma",
                                                   &stm32f4_dma_type);

static int stm32f4_dma_probe(const char *name) {
    const void *blob = fdtparse_get_blob();
    int offset;

    /* Lookup peripheral node */
    offset = fdt_path_offset(blob, name);
    if (offset < 0) {
        return 0;
    }

    /* Check that peripheral is compatible with driver */
    return fdt_node_check_compatible(blob, offset, STM32F4_DMA_COMPAT) == 0;
}

static struct obj *stm32f4_dma_ctor(const char *name) {
    const void *blob = fdtparse_get_blob();
    int offset, err, periph_id, dma_cells;
    struct obj *obj;
    struct stm32f4_dma *stm32f4_dma;
    struct stm32f4_dma_regs *regs;

    offset = fdt_path_offset(blob, name);
    if (offset < 0) {
        return NULL;
    }

    if (fdt_node_check_compatible(blob, offset, STM32F4_DMA_COMPAT)) {
        return NULL;
    }

    regs = fdtparse_get_addr32(blob, offset, "reg");
    if (!regs) {
        return NULL;
    }

    err = fdtparse_get_int(blob, offset, "stmicro,periph-id", &periph_id);
    if (err) {
        return NULL;
    }

    err = fdtparse_get_int(blob, offset, "#dma-cells", &dma_cells);
    if (err) {
        return NULL;
    }

    /* DMA cells must be 2, stream and channel number */
    if (dma_cells != 2) {
        return NULL;
    }

    obj = instantiate(name, &stm32f4_dma_class, &stm32f4_dma_ops, struct stm32f4_dma);
    if (!obj) {
        return NULL;
    }

    stm32f4_dma = to_stm32f4_dma(obj);

    stm32f4_dma->periph_id = periph_id;
    stm32f4_dma->regs = regs;
    init_mutex(&stm32f4_dma->lock);
    memset(stm32f4_dma->streams_in_use, 0,
           sizeof(stm32f4_dma->streams_in_use));

    /* Enable clock */
    err = rcc_set_clock_enable(periph_id, 1);
    if (err) {
        goto err_free_obj;
    }

    /* Export to the OS */
    class_export_member(obj);

    return obj;

err_free_obj:
    class_deinstantiate(obj);

    return NULL;
}

static void stm32f4_dma_dtor(struct obj *o) {
    struct stm32f4_dma *stm32f4_dma;

    assert_type(o, &stm32f4_dma_type);
    stm32f4_dma = to_stm32f4_dma(o);

    /* Disable clock */
    rcc_set_clock_enable(stm32f4_dma->periph_id, 0);

    kfree(stm32f4_dma);
}

static struct mutex stm32f4_dma_driver_mut = INIT_MUTEX;

static struct device_driver stm32f4_dma_compat_driver = {
    .name = STM32F4_DMA_COMPAT,
    .probe = stm32f4_dma_probe,
    .ctor = stm32f4_dma_ctor,
    .class = &stm32f4_dma_class,
    .mut = &stm32f4_dma_driver_mut,
};

static int stm32f4_dma_register(void) {
    device_compat_driver_register(&stm32f4_dma_compat_driver);
    return 0;
}
CORE_INITIALIZER(stm32f4_dma_register)

static int try_dma_allocate(const void *fdt, int phandle, int stream,
                            int channel, struct stm32f4_dma **dma,
                            stm32f4_dma_handle_t *handle) {
    int offset, ret = -1;
    char *path;
    struct obj *obj;
    struct stm32f4_dma_ops *ops;

    offset = fdt_node_offset_by_phandle(fdt, phandle);
    if (offset < 0) {
        return offset;
    }

    path = fdtparse_get_path(fdt, offset);
    if (!path) {
        return -1;
    }

    obj = device_get(path);
    if (!obj) {
        goto out;
    }

    *dma = to_stm32f4_dma(obj);
    ops = obj->ops;

    *handle = ops->allocate(*dma, stream, channel);
    if (*handle == STM32F4_DMA_ERROR) {
        obj_put(obj);
        goto out;
    }

    ret = 0;

out:
    free(path);

    return ret;
}

int stm32f4_dma_allocate(const void *fdt, int offset, const char *name,
                         struct stm32f4_dma **dma,
                         stm32f4_dma_handle_t *handle) {
    const struct fdt_property *dmas;
    const struct fdt_property *dma_names;
    int dmas_len, dma_names_len;
    const char *curr_name, *name_start;
    int i;
    int success = 0;

    if (!fdt || !name || !dma || !handle) {
        return -1;
    }

    dmas = fdt_get_property(fdt, offset, "dmas", &dmas_len);
    if (dmas_len < 0) {
        return dmas_len;
    }

    dma_names = fdt_get_property(fdt, offset, "dma-names", &dma_names_len);
    if (dma_names_len < 0) {
        return dma_names_len;
    }

    for (i = 0, name_start = curr_name = dma_names->data;
         curr_name;
         i++, curr_name = fdtparse_stringlist_next(name_start, curr_name,
                                                   dma_names_len)) {
        int offset = (uintptr_t)curr_name - (uintptr_t)name_start;
        int max_len = dma_names_len - offset;
        int index;
        fdt32_t *dma_cell;
        uint32_t phandle, stream, channel;

        if (strncmp(curr_name, name, max_len)) {
            continue;
        }
        /* Found a match */

        index = 3 * i * sizeof(fdt32_t);  /* phandle + 2 #dma-cells */

        /* dmas must have room for this entry */
        if (index + 3 * sizeof(fdt32_t) > dmas_len) {
            return -1;
        }

        dma_cell = (fdt32_t *)&dmas->data[index];
        phandle = fdt32_to_cpu(dma_cell[0]);
        stream = fdt32_to_cpu(dma_cell[1]);
        channel = fdt32_to_cpu(dma_cell[2]);

        if (!try_dma_allocate(fdt, phandle, stream, channel, dma, handle)) {
            /* We got it! */
            success = 1;
            break;
        }
    }

    if (!success) {
        return -1;
    }

    return 0;
}

int stm32f4_dma_deallocate(struct stm32f4_dma *dma,
                           stm32f4_dma_handle_t handle) {
    struct stm32f4_dma_ops *ops;
    int ret;

    if (!dma) {
        return -1;
    }

    ops = dma->obj.ops;

    ret = ops->deallocate(dma, handle);
    if (ret) {
        return ret;
    }

    /* Done with this reference to the DMA */
    obj_put(&dma->obj);

    return 0;
}
