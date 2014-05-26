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

#include <compiler.h>
#include <libfdt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arch/chip/adc.h>
#include <arch/chip/gpio.h>
#include <dev/fdtparse.h>
#include <dev/hw/gpio.h>
#include <dev/hw/adc.h>
#include <dev/raw_mem.h>
#include <kernel/collection.h>
#include <kernel/fault.h>
#include <kernel/mutex.h>
#include <kernel/obj.h>
#include <mm/mm.h>

#define STM32F4_ADC_COMPAT  "stmicro,stm32f407-adc"

struct adc_channel {
    int adc;
    int channel;
};

struct adc_individual_state {
    int valid;  /* This ADC exists */
    uint8_t ready;
    int periph_id;
    struct fdt_gpio channel_gpio[ADC_CHANNELS];
    int channel_in_use[ADC_CHANNELS];
};

struct adc_state {
    const void *fdt;
    int offset;
    struct stm32f4_adc_regs *regs;
    struct adc_individual_state adc[NUM_ADC];
    struct mutex lock;
    struct obj obj;
};

struct stm32f4_adc {
    struct adc_state *state;
    int adc_num;
    int channel;
    int sequence_number;
    uint8_t ready;
};

void stm32f4_adc_obj_dtor(struct obj *o);

struct obj_type stm32f4_adc_obj_type  = {
    .offset = offset_of(struct adc_state, obj),
    .dtor = stm32f4_adc_obj_dtor,
};

void stm32f4_adc_obj_dtor(struct obj *o) {
    assert_type(o, &stm32f4_adc_obj_type);

    /* Free allocated name */
    if (o->name) {
        free((char *)o->name);
    }
}

struct collection stm32f4_adcs = INIT_COLLECTION(stm32f4_adcs);

static inline struct adc_state *to_adc_state(struct obj *o) {
    return (struct adc_state *) container_of(o, struct adc_state, obj);
}

static int adc_sequence_length(struct stm32f4_adc_individual_regs *regs) {
    return 1 + ((raw_mem_read(&regs->SQR1) & ADC_SQR1_LEN_MASK) \
                    >> ADC_SQR1_LEN_SHIFT);
}

static int adc_read_sequence_number(struct stm32f4_adc_individual_regs *regs,
                                    int sequence_number) {
    uint32_t reg, val;

    if (sequence_number <= 6) {
        reg = raw_mem_read(&regs->SQR3);

        val = (reg & ADC_SQR3_SQ_MASK(sequence_number))
                >> ADC_SQR3_SQ_SHIFT(sequence_number);
    }
    else if (sequence_number <= 12) {
        reg = raw_mem_read(&regs->SQR2);

        val = (reg & ADC_SQR2_SQ_MASK(sequence_number))
                >> ADC_SQR2_SQ_SHIFT(sequence_number);
    }
    else {
        reg = raw_mem_read(&regs->SQR1);

        val = (reg & ADC_SQR1_SQ_MASK(sequence_number))
                >> ADC_SQR1_SQ_SHIFT(sequence_number);
    }

    return val;
}

static void adc_set_sequence_number(struct stm32f4_adc_individual_regs *regs,
                                    int sequence_number, int value) {
    if (sequence_number <= 6) {
        raw_mem_set_mask(&regs->SQR3, ADC_SQR3_SQ_MASK(sequence_number),
                         ADC_SQR3_SQ(sequence_number, value));
    }
    else if (sequence_number <= 12) {
        raw_mem_set_mask(&regs->SQR2, ADC_SQR2_SQ_MASK(sequence_number),
                         ADC_SQR2_SQ(sequence_number, value));
    }
    else {
        raw_mem_set_mask(&regs->SQR1, ADC_SQR1_SQ_MASK(sequence_number),
                         ADC_SQR1_SQ(sequence_number, value));
    }
}

/*
 * ADC peripheral initialization
 *
 * Initialize the ADC peripheral for use, initializing the registers
 * to a standard state.
 *
 * The ADC state mutex should be held when calling this function.
 *
 * @param adc   ADC peripheral to initialize
 * @returns zero on success, negative on error
 */
static int stm32f4_adc_periph_initialize(struct adc *adc) {
    struct stm32f4_adc *stm32f4_adc = adc->priv;
    struct adc_individual_state *periph;
    struct stm32f4_adc_individual_regs *regs;

    periph = &stm32f4_adc->state->adc[stm32f4_adc->adc_num];

    if (rcc_set_clock_enable(periph->periph_id, 1)) {
        return -1;
    }

    regs = &stm32f4_adc->state->regs->adc[stm32f4_adc->adc_num];

    /* Reset sample times and scan sequences */
    raw_mem_write(&regs->SMPR1, 0);
    raw_mem_write(&regs->SMPR2, 0);
    raw_mem_write(&regs->SQR1, 0);
    raw_mem_write(&regs->SQR2, 0);
    raw_mem_write(&regs->SQR3, 0);

    /* Scan mode */
    raw_mem_write(&regs->CR1, ADC_CR1_SCAN);

    /* TODO: ADC in continuous mode */

    /*
     * Don't actually enable the ADC yet, to indicate to the
     * channel initialization that none of the channels scan
     * sequences are in use.
     */

    /* TODO: DMA? */

    periph->ready = 1;

    return 0;
}

/*
 * ADC channel initialization
 *
 * Initialize the ADC channel for reading, initializing the entire
 * peripheral if necessary.
 *
 * The ADC state mutex should be held when calling this function.
 *
 * @param adc   ADC channel to initialize
 * @returns zero on success, negative on error
 */
static int stm32f4_adc_channel_initialize(struct adc *adc) {
    struct stm32f4_adc *stm32f4_adc = adc->priv;
    struct adc_individual_state *periph;
    struct stm32f4_adc_individual_regs *regs;
    int enabled;

    periph = &stm32f4_adc->state->adc[stm32f4_adc->adc_num];

    if (!periph->ready) {
        if (stm32f4_adc_periph_initialize(adc)) {
            goto err;
        }
    }

    regs = &stm32f4_adc->state->regs->adc[stm32f4_adc->adc_num];

    enabled = raw_mem_read(&regs->CR2) & ADC_CR2_ADON;

    /* If the ADC is not enabled, this is the first channel */
    if (!enabled) {
        raw_mem_set_mask(&regs->SQR3, ADC_SQR3_SQ_MASK(1),
                         ADC_SQR3_SQ(1, stm32f4_adc->channel));

        raw_mem_set_mask(&regs->SQR1, ADC_SQR1_LEN_MASK, ADC_SQR1_LEN(1));

        stm32f4_adc->sequence_number = 1;

        /* Enable ADC and TODO: start continuous conversion */
        raw_mem_set_bits(&regs->CR2, ADC_CR2_ADON);
    }
    else {
        /* Add this channel to the sequence as number LEN+1 */
        int new_seq_num;
        int length = adc_sequence_length(regs);
        WARN_ON(length >= 16);

        new_seq_num = length + 1;

        adc_set_sequence_number(regs, new_seq_num, stm32f4_adc->channel);

        stm32f4_adc->sequence_number = new_seq_num;

        /* Set new scan length */
        raw_mem_set_mask(&regs->SQR1, ADC_SQR1_LEN_MASK,
                         ADC_SQR1_LEN(new_seq_num));
    }

    stm32f4_adc->ready = 1;

    return 0;

err:
    return -1;
}

static int stm32f4_adc_init_nolock(struct adc *adc) {
    struct stm32f4_adc *stm32f4_adc;
    struct adc_individual_state *periph;
    int ret = 0;

    if (!adc) {
        return -1;
    }

    stm32f4_adc = adc->priv;
    periph = &stm32f4_adc->state->adc[stm32f4_adc->adc_num];

    if (!periph->ready) {
        ret = stm32f4_adc_periph_initialize(adc);
        if (ret) {
            goto out;
        }
    }

    if (!stm32f4_adc->ready) {
        ret = stm32f4_adc_channel_initialize(adc);
        if (ret) {
            goto out;
        }
    }

out:
    return ret;
}

static int stm32f4_adc_init(struct adc *adc) {
    struct stm32f4_adc *stm32f4_adc;
    int ret;

    if (!adc) {
        return -1;
    }

    stm32f4_adc = adc->priv;

    acquire(&stm32f4_adc->state->lock);
    ret = stm32f4_adc_init_nolock(adc);
    release(&stm32f4_adc->state->lock);

    return ret;
}

/**
 * Remove a sequence number from scan sequence
 *
 * The specified sequence number is removed from the existing sequence.
 * All channels later in the sequence are shifted one place earlier in
 * the sequence.
 *
 * @param regs  Individual ADC registers with sequence to modify
 * @param num   Sequence index to remove
 * @returns 0 on success, negative on error
 */
static int remove_from_adc_sequence(struct stm32f4_adc_individual_regs *regs,
                                    int num) {
    int length = adc_sequence_length(regs);

    if (num > length) {
        return -1;
    }

    /* Removing only channel, disable ADC */
    if (length == 1) {
        raw_mem_clear_bits(&regs->CR2, ADC_CR2_ADON);
        return 0;
    }

    /* Stop conversion while sequences are rearranged */
    raw_mem_clear_bits(&regs->CR2, ADC_CR2_ADON);

    /* Shift higher channels in sequence to the right */
    for (int i = num + 1; i <= length; i++) {
        int channel = adc_read_sequence_number(regs, i);
        adc_set_sequence_number(regs, i-1, channel);
    }

    /* Set new scan length */
    raw_mem_set_mask(&regs->SQR1, ADC_SQR1_LEN_MASK,
                     ADC_SQR1_LEN(length-1));

    /* TODO: start continuous conversion */
    raw_mem_set_bits(&regs->CR2, ADC_CR2_ADON);

    return 0;
}

static int stm32f4_adc_dtor(struct adc *adc) {
    struct stm32f4_adc *stm32f4_adc;
    struct adc_individual_state *periph;
    struct stm32f4_adc_individual_regs *regs;
    int ret = 0;

    if (!adc) {
        return -1;
    }

    stm32f4_adc = adc->priv;
    periph = &stm32f4_adc->state->adc[stm32f4_adc->adc_num];
    regs = &stm32f4_adc->state->regs->adc[stm32f4_adc->adc_num];

    acquire(&stm32f4_adc->state->lock);

    /* ADC has been initialized, remove it from the sequence */
    if (stm32f4_adc->ready) {
        WARN_ON(!stm32f4_adc->sequence_number);

        remove_from_adc_sequence(regs, stm32f4_adc->sequence_number);
        stm32f4_adc->sequence_number = 0;
        stm32f4_adc->ready = 0;
    }

    periph->channel_in_use[stm32f4_adc->channel] = 0;

    release(&stm32f4_adc->state->lock);

    /* Done with the state obj */
    obj_put(&stm32f4_adc->state->obj);

    kfree(stm32f4_adc);

    return ret;
}

static uint32_t stm32f4_adc_read_raw(struct adc *adc) {
    struct stm32f4_adc *stm32f4_adc = adc->priv;
    struct stm32f4_adc_individual_regs *regs;
    uint32_t val = 0;
    uint32_t saved_sq1, saved_length;

    regs = &stm32f4_adc->state->regs->adc[stm32f4_adc->adc_num];

    /*
     * Set our channel as the only channel in the sequence, and perform
     * a one-shot conversion.
     *
     * TODO: use the DMA to read the multichannel sequence set up by the
     * rest of the driver.
     */

    acquire(&stm32f4_adc->state->lock);

    if (!stm32f4_adc->ready) {
        val = stm32f4_adc_init_nolock(adc);
        if (val) {
            goto out;
        }
    }

    saved_length = raw_mem_read(&regs->SQR1) & ADC_SQR1_LEN_MASK;
    saved_sq1 = raw_mem_read(&regs->SQR3) & ADC_SQR3_SQ_MASK(1);

    /* Clear end of conversion */
    raw_mem_clear_bits(&regs->SR, ADC_SR_EOC);

    /* Set this channel as the only in the sequence */
    raw_mem_set_mask(&regs->SQR1, ADC_SQR1_LEN_MASK, ADC_SQR1_LEN(1));
    raw_mem_set_mask(&regs->SQR3, ADC_SQR3_SQ_MASK(1),
                     ADC_SQR3_SQ(1, stm32f4_adc->channel));

    /* Initiate conversion */
    raw_mem_set_bits(&regs->CR2, ADC_CR2_SWSTART);

    /*
     * Wait for conversion to complete
     * TODO: Timeout
     */
    while (!(raw_mem_read(&regs->SR) & ADC_SR_EOC));

    val = raw_mem_read(&regs->DR);

    /* Restore original values */
    raw_mem_set_mask(&regs->SQR1, ADC_SQR1_LEN_MASK, saved_length);
    raw_mem_set_mask(&regs->SQR3, ADC_SQR3_SQ_MASK(1), saved_sq1);

out:
    release(&stm32f4_adc->state->lock);
    return val;
}

struct adc_ops stm32f4_adc_ops = {
    .init = stm32f4_adc_init,
    .dtor = stm32f4_adc_dtor,
    .read_raw = stm32f4_adc_read_raw,
};

/*
 * Create ADC state structure
 *
 * Build a new state structure, setting up all state, and adding
 * it to the ADC list.
 *
 * @param fdt   FDT blob
 * @param offset    ADC node offset
 * @returns ADC state struct, or NULL on error
 */
static struct adc_state *create_adc_state(const void *fdt, int offset) {
    struct adc_state *state;
    char *path;
    int adc_found = 0;
    const char *adc_subnodes[NUM_ADC] = {"adc1", "adc2", "adc3"};

    state = malloc(sizeof(*state));
    if (!state) {
        return NULL;
    }

    memset(state, 0, sizeof(*state));

    /* Allocated name, freed in dtor */
    path = fdtparse_get_path(fdt, offset);
    if (!path) {
        goto err;
    }

    obj_init(&state->obj, &stm32f4_adc_obj_type, path);

    state->fdt = fdt;
    state->offset = offset;
    init_mutex(&state->lock);

    state->regs = fdtparse_get_addr32(fdt, offset, "reg");
    if (!state->regs) {
        goto err_free_path;
    }

    /* Some of ADC1-3 may be missing, so only add those that exist */
    for (int i = 0; i < NUM_ADC; i++) {
        int channels;
        int adc_offset;

        adc_offset = fdt_subnode_offset(fdt, offset, adc_subnodes[i]);
        if (adc_offset < 0) {
            continue;
        }

        adc_found = 1;
        state->adc[i].valid = 1;

        channels = fdtparse_get_gpios(fdt, adc_offset, "stmicro,adc-channels",
                                      state->adc[i].channel_gpio,
                                      ADC_CHANNELS);

        /* FDT must have exactly ADC_CHANNELS GPIOs */
        if (channels != ADC_CHANNELS) {
            goto err_free_path;
        }

        if (fdtparse_get_int(fdt, adc_offset, "stmicro,periph-id",
                             &state->adc[i].periph_id)) {
            goto err_free_path;
        }
    }

    /* Make sure we found *something* */
    if (!adc_found) {
        goto err_free_path;
    }

    collection_add(&stm32f4_adcs, &state->obj);

    return state;

err_free_path:
    free(path);
err:
    free(state);
    return NULL;
}

/*
 * Lookup or create ADC state structure
 *
 * In the ADC list, find the appropriate ADC state structure.
 * If none exists, create one and add it to the ADC list.
 *
 * @param fdt   FDT blob
 * @param offset    ADC node offset
 * @returns Pointer to ADC state structure, NULL on error
 */
static struct adc_state *lookup_adc_state(const void *fdt, int offset) {
    struct obj *state_obj;
    struct adc_state *state = NULL;

    collection_lock(&stm32f4_adcs);

    for (state_obj = collection_iter(&stm32f4_adcs); state_obj;
         state_obj = collection_next(&stm32f4_adcs)) {
        struct adc_state *tmp_state = to_adc_state(state_obj);
        if (tmp_state->fdt == fdt && tmp_state->offset == offset) {
            /* Found it! */
            collection_stop(&stm32f4_adcs);
            state = tmp_state;
        }
    }

    /* Couldn't find one, so make one */
    if (!state) {
        state = create_adc_state(fdt, offset);
    }

    collection_unlock(&stm32f4_adcs);

    return state;
}

/**
 * Find and reserve an ADC channel for a GPIO
 *
 * Search through the ADC state to find an unused ADC channel compatible
 * with the provided GPIO, and reserve it as used.
 *
 * @param state ADC state to search
 * @param gpio_obj  GPIO to match with an ADC channel
 * @param adc_channel   Struct to store result in, on success
 * @returns 0 on success, negative on error
 */
static int reserve_adc_channel(struct adc_state *state, struct obj *gpio_obj,
                               struct adc_channel *adc_channel) {
    struct gpio *gpio = to_gpio(gpio_obj);
    int adc_num, channel, found = 0;
    int ret = -1;

    if (!state || !gpio_obj || !adc_channel) {
        return -1;
    }

    acquire(&state->lock);

    /* Look for a compatible and free channel */
    for (adc_num = 0; adc_num < NUM_ADC; adc_num++) {
        if (!state->adc[adc_num].valid) {
            continue;
        }

        for (channel = 0; channel < ADC_CHANNELS; channel++) {
            int channel_gpio_num, channel_in_use;

            channel_gpio_num = state->adc[adc_num].channel_gpio[channel].gpio;
            channel_in_use = state->adc[adc_num].channel_in_use[channel];

            if ((channel_gpio_num == gpio->num) && !channel_in_use) {
                found = 1;
                goto done;
            }
        }
    }

done:
    if (!found) {
        goto out;
    }

    state->adc[adc_num].channel_in_use[channel] = 1;

    adc_channel->adc = adc_num;
    adc_channel->channel = channel;
    ret = 0;

out:
    release(&state->lock);
    return ret;
}

/**
 * Free reserved ADC channel
 *
 * Makes the ADC channel available to other users
 *
 * @param state ADC state to free ADC channel in
 * @param adc_channel ADC channel to free
 */
static void free_adc_channel(struct adc_state *state,
                             struct adc_channel *adc_channel) {
    if (!state || !adc_channel) {
        return;
    }

    if (adc_channel->adc >= NUM_ADC) {
        return;
    }

    if (adc_channel->channel >= ADC_CHANNELS) {
        return;
    }

    acquire(&state->lock);
    state->adc[adc_channel->adc].channel_in_use[adc_channel->channel] = 0;
    release(&state->lock);
}

static struct obj *build_adc_obj(struct adc_state *state, struct obj *gpio_obj,
                                 struct adc_channel *adc_channel) {
    struct gpio *gpio;
    struct gpio_ops *gpio_ops;
    struct obj *adc_obj;
    struct adc *adc;
    struct stm32f4_adc *stm32f4_adc;
    char name[32];
    int err;

    if (!state || !gpio_obj || !adc_channel) {
        return NULL;
    }

    gpio = to_gpio(gpio_obj);
    gpio_ops = gpio_obj->ops;

    /* Set to analog input */
    err = gpio_ops->direction(gpio, GPIO_INPUT|STM32F4_GPIO_DIRECTION_ANALOG);
    if (err) {
        goto err;
    }

    /* Construct ADC name */
    scnprintf(name, 32, "adc_%s", gpio_obj->name);

    /* Instantiate ADC object */
    adc_obj = instantiate(name, &adc_class, &stm32f4_adc_ops, struct adc);
    if (!adc_obj) {
        goto err;
    }

    adc = to_adc(adc_obj);
    adc->gpio = gpio_obj;
    adc->priv = kmalloc(sizeof(struct stm32f4_adc));
    if (!adc->priv) {
        goto err_free_adc_obj;
    }

    /* Will be holding onto the state */
    obj_get(&state->obj);

    stm32f4_adc = adc->priv;
    stm32f4_adc->state = state;
    stm32f4_adc->adc_num = adc_channel->adc;
    stm32f4_adc->channel = adc_channel->channel;
    stm32f4_adc->ready = 0;

    /* Export to the OS */
    class_export_member(adc_obj);

    return adc_obj;

err_free_adc_obj:
    class_deinstantiate(adc_obj);
err:
    obj_put(gpio_obj);
    return NULL;
}

struct obj *stm32f4_adc_get(const void *fdt, int offset, struct obj *gpio) {
    struct obj *adc;
    struct adc_state *state;
    struct adc_channel adc_channel;

    if (fdt_node_check_compatible(fdt, offset, STM32F4_ADC_COMPAT)) {
        goto err;
    }

    state = lookup_adc_state(fdt, offset);
    if (!state) {
        goto err;
    }

    if (reserve_adc_channel(state, gpio, &adc_channel)) {
        goto err;
    }

    adc = build_adc_obj(state, gpio, &adc_channel);
    if (!adc) {
        goto err_free_adc_channel;
    }

    return adc;

err_free_adc_channel:
    free_adc_channel(state, &adc_channel);
err:
    return NULL;
}

DECLARE_ADC_DRIVER(stm32f4) = {
    .compat = STM32F4_ADC_COMPAT,
    .adc_get = stm32f4_adc_get,
};
