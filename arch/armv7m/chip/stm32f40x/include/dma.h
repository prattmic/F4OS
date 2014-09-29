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

#ifndef ARCH_CHIP_DMA_H_INCLUDED
#define ARCH_CHIP_DMA_H_INCLUDED

#include <stdint.h>
#include <kernel/mutex.h>
#include <kernel/obj.h>

struct stm32f4_dma_stream_regs {
    uint32_t CR;    /* DMA stream control register */
    uint32_t NDTR;  /* DMA stream number of data register */
    uint32_t PAR;   /* DMA stream peripheral register */
    uint32_t M0AR;  /* DMA stream memory 0 address register */
    uint32_t M1AR;  /* DMA stream memory 1 address register */
    uint32_t FCR;   /* DMA stream FIFO control register */
};

struct stm32f4_dma_regs {
    uint32_t LISR;  /* DMA Low interrupt status register */
    uint32_t HISR;  /* DMA High interrupt status register */
    uint32_t LIFCR; /* DMA Low interrupt flag clear register */
    uint32_t HIFCR; /* DMA High interrupt flag clear register */
    struct stm32f4_dma_stream_regs stream[8];
};

struct stm32f4_dma {
    int periph_id;
    struct stm32f4_dma_regs *regs;
    uint8_t streams_in_use[8];
    /* Required for shared structures and registers, not stream-specific regs */
    struct mutex lock;
    struct obj obj;
};

static inline struct stm32f4_dma *to_stm32f4_dma(struct obj *o) {
    return (struct stm32f4_dma *) container_of(o, struct stm32f4_dma, obj);
}

typedef uint16_t stm32f4_dma_handle_t;
#define STM32F4_DMA_ERROR   ((stm32f4_dma_handle_t)-1)

enum stm32f4_dma_direction {
    STM32F4_DMA_DIR_PERIPH_TO_MEM,
    STM32F4_DMA_DIR_MEM_TO_PERIPH,
    STM32F4_DMA_DIR_MEM_TO_MEM,
};

struct stm32f4_dma_config {
    /* Transfer direction */
    enum stm32f4_dma_direction direction;
    /* Memory data size (1, 2, 4 bytes) */
    uint8_t memory_size;
    /* Peripheral data size (1, 2, 4 bytes) */
    uint8_t peripheral_size;
    /* Increment memory address automatically */
    uint8_t memory_increment;
    /* Increment peripheral address automatically */
    uint8_t peripheral_increment;
    /* Transactions automatically continue */
    uint8_t circular;
    /* Use two memory buffers */
    uint8_t double_buffer;
    /* Initial peripheral address */
    uintptr_t peripheral_addr;
    /* Initial memory 0 (primary) address */
    uintptr_t mem0_addr;
    /* Initial memory 1 address */
    uintptr_t mem1_addr;
};

struct stm32f4_dma_ops {
    /*
     * Allocate DMA stream and channel
     *
     * Reserves stream, configured for channel, for use by the caller.
     * The returned handle is used to configure DMA transactions
     *
     * When finished, free stream for other users with deallocate().
     *
     * @param stm32f4_dma   DMA controller to reserve on
     * @param stream        Stream number to reserve
     * @param channel       Channel to configure stream to
     * @returns handle on success, STM32F4_DMA_ERROR on error
     */
    stm32f4_dma_handle_t (*allocate)(struct stm32f4_dma *, uint8_t, uint8_t);
    /*
     * Deallocate DMA stream and channel
     *
     * Frees stream, allocated with allocate(), for use by other peripherals.
     *
     * @param stm32f4_dma   DMA controller for handle
     * @param handle        Stream/channel handle to free
     * @returns 0 on success. negative on error
     */
    int (*deallocate)(struct stm32f4_dma *, stm32f4_dma_handle_t);
    /*
     * Configure DMA handle
     *
     * Configure DMA handle with options specified in stm32f4_dma_config
     * struct.  Should be called before any transactions begin.
     *
     * @param stm32f4_dma   DMA controller for handle
     * @param handle        Stream/channel handle to configure
     * @param config        Configuration options
     * @returns zero on success, negative on error
     */
    int (*configure)(struct stm32f4_dma *, stm32f4_dma_handle_t,
                     struct stm32f4_dma_config *);
    /*
     * Begin DMA transaction
     *
     * Begin DMA transaction of num items.  If a transaction is currently in
     * progress, it will be cancelled in order to begin a new transaction.
     *
     * @param stm32f4_dma   DMA controller for handle
     * @param handle        Stream/channel handle for transaction
     * @param num           Number of items to transfer
     * @returns zero on success, negative on error
     */
    int (*begin_transaction)(struct stm32f4_dma *, stm32f4_dma_handle_t,
                             uint16_t);
    /*
     * Transaction complete
     *
     * @param stm32f4_dma   DMA controller for handle
     * @param handle        Stream/channel handle to check
     * @returns positive if transaction completed since last call, zero if
     *          not, negative on error
     */
    int (*transaction_complete)(struct stm32f4_dma *, stm32f4_dma_handle_t);
    /*
     * Number of data items remaining in transfer
     *
     * @param stm32f4_dma   DMA controller for handle
     * @param handle        Stream/channel handle to check
     * @returns number of data items remaining to transfer in active transaction,
     *          negative on error.
     */
    int (*items_remaining)(struct stm32f4_dma *, stm32f4_dma_handle_t);
};

/*
 * Allocate DMA stream and channel for FDT device
 *
 * Allocate and setup a DMA stream and channel for the named DMA
 * for an FDT object, using the dmas and dma-names properties.
 *
 * Any dma with a matching dma-names entry may be used.  If one
 * is unavailable, others will be checked.
 *
 * When finished, deallocate with stm32f4_dma_deallocate().
 *
 * @param fdt       FDT blob
 * @param offset    Device node containing DMA to allocate
 * @param name      Name of DMA to allocate (one of dma-names)
 * @param dma       Output STM32F4 DMA obj to use for DMA transactions
 * @param handle    STM32F4 DMA handle to use for DMA transactions
 * @returns 0 on success, negative on error
 */
int stm32f4_dma_allocate(const void *fdt, int offset, const char *name,
                         struct stm32f4_dma **dma,
                         stm32f4_dma_handle_t *handle);

/*
 * Deallocate DMA stream and channel
 *
 * Frees the stream for other drivers to use.  The dma pointer must not be
 * used after calling this function.
 *
 * @param dma       STM32F4 DMA controller
 * @param handle    Handle of stream to deallocate
 * @returns 0 on success, negative on error
 */
int stm32f4_dma_deallocate(struct stm32f4_dma *dma,
                           stm32f4_dma_handle_t handle);

#define DMA_LISR_FEIF0                  ((uint32_t) (1 << 0))   /* DMA stream 0 FIFO error flag */
#define DMA_LISR_DMEIF0                 ((uint32_t) (1 << 2))   /* DMA stream 0 direct mode error flag */
#define DMA_LISR_TEIF0                  ((uint32_t) (1 << 3))   /* DMA stream 0 transfer error flag */
#define DMA_LISR_HTIF0                  ((uint32_t) (1 << 4))   /* DMA stream 0 halt transfer flag */
#define DMA_LISR_TCIF0                  ((uint32_t) (1 << 5))   /* DMA stream 0 transfer complete flag */

#define DMA_LISR_FEIF1                  ((uint32_t) (1 << 6))   /* DMA stream 1 FIFO error flag */
#define DMA_LISR_DMEIF1                 ((uint32_t) (1 << 8))   /* DMA stream 1 direct mode error flag */
#define DMA_LISR_TEIF1                  ((uint32_t) (1 << 9))   /* DMA stream 1 transfer error flag */
#define DMA_LISR_HTIF1                  ((uint32_t) (1 << 10))  /* DMA stream 1 halt transfer flag */
#define DMA_LISR_TCIF1                  ((uint32_t) (1 << 11))  /* DMA stream 1 transfer complete flag */

#define DMA_LISR_FEIF2                  ((uint32_t) (1 << 16))  /* DMA stream 2 FIFO error flag */
#define DMA_LISR_DMEIF2                 ((uint32_t) (1 << 18))  /* DMA stream 2 direct mode error flag */
#define DMA_LISR_TEIF2                  ((uint32_t) (1 << 19))  /* DMA stream 2 transfer error flag */
#define DMA_LISR_HTIF2                  ((uint32_t) (1 << 20))  /* DMA stream 2 halt transfer flag */
#define DMA_LISR_TCIF2                  ((uint32_t) (1 << 21))  /* DMA stream 2 transfer complete flag */

#define DMA_LISR_FEIF3                  ((uint32_t) (1 << 22))  /* DMA stream 3 FIFO error flag */
#define DMA_LISR_DMEIF3                 ((uint32_t) (1 << 24))  /* DMA stream 3 direct mode error flag */
#define DMA_LISR_TEIF3                  ((uint32_t) (1 << 25))  /* DMA stream 3 transfer error flag */
#define DMA_LISR_HTIF3                  ((uint32_t) (1 << 26))  /* DMA stream 3 halt transfer flag */
#define DMA_LISR_TCIF3                  ((uint32_t) (1 << 27))  /* DMA stream 3 transfer complete flag */

#define DMA_HISR_FEIF4                  ((uint32_t) (1 << 0))   /* DMA stream 4 FIFO error flag */
#define DMA_HISR_DMEIF4                 ((uint32_t) (1 << 2))   /* DMA stream 4 direct mode error flag */
#define DMA_HISR_TEIF4                  ((uint32_t) (1 << 3))   /* DMA stream 4 transfer error flag */
#define DMA_HISR_HTIF4                  ((uint32_t) (1 << 4))   /* DMA stream 4 halt transfer flag */
#define DMA_HISR_TCIF4                  ((uint32_t) (1 << 5))   /* DMA stream 4 transfer complete flag */

#define DMA_HISR_FEIF5                  ((uint32_t) (1 << 6))   /* DMA stream 5 FIFO error flag */
#define DMA_HISR_DMEIF5                 ((uint32_t) (1 << 8))   /* DMA stream 5 direct mode error flag */
#define DMA_HISR_TEIF5                  ((uint32_t) (1 << 9))   /* DMA stream 5 transfer error flag */
#define DMA_HISR_HTIF5                  ((uint32_t) (1 << 10))  /* DMA stream 5 halt transfer flag */
#define DMA_HISR_TCIF5                  ((uint32_t) (1 << 11))  /* DMA stream 5 transfer complete flag */

#define DMA_HISR_FEIF6                  ((uint32_t) (1 << 16))  /* DMA stream 6 FIFO error flag */
#define DMA_HISR_DMEIF6                 ((uint32_t) (1 << 18))  /* DMA stream 6 direct mode error flag */
#define DMA_HISR_TEIF6                  ((uint32_t) (1 << 19))  /* DMA stream 6 transfer error flag */
#define DMA_HISR_HTIF6                  ((uint32_t) (1 << 20))  /* DMA stream 6 halt transfer flag */
#define DMA_HISR_TCIF6                  ((uint32_t) (1 << 21))  /* DMA stream 6 transfer complete flag */

#define DMA_HISR_FEIF7                  ((uint32_t) (1 << 22))  /* DMA stream 7 FIFO error flag */
#define DMA_HISR_DMEIF7                 ((uint32_t) (1 << 24))  /* DMA stream 7 direct mode error flag */
#define DMA_HISR_TEIF7                  ((uint32_t) (1 << 25))  /* DMA stream 7 transfer error flag */
#define DMA_HISR_HTIF7                  ((uint32_t) (1 << 26))  /* DMA stream 7 halt transfer flag */
#define DMA_HISR_TCIF7                  ((uint32_t) (1 << 27))  /* DMA stream 7 transfer complete flag */

#define DMA_LIFCR_CFEIF0                ((uint32_t) (1 << 0))   /* DMA stream 0 clear FIFO error flag */
#define DMA_LIFCR_CDMEIF0               ((uint32_t) (1 << 2))   /* DMA stream 0 clear direct mode error flag */
#define DMA_LIFCR_CTEIF0                ((uint32_t) (1 << 3))   /* DMA stream 0 clear transfer error flag */
#define DMA_LIFCR_CHTIF0                ((uint32_t) (1 << 4))   /* DMA stream 0 clear halt transfer flag */
#define DMA_LIFCR_CTCIF0                ((uint32_t) (1 << 5))   /* DMA stream 0 clear transfer complete flag */

#define DMA_LIFCR_CFEIF1                ((uint32_t) (1 << 6))   /* DMA stream 1 clear FIFO error flag */
#define DMA_LIFCR_CDMEIF1               ((uint32_t) (1 << 8))   /* DMA stream 1 clear direct mode error flag */
#define DMA_LIFCR_CTEIF1                ((uint32_t) (1 << 9))   /* DMA stream 1 clear transfer error flag */
#define DMA_LIFCR_CHTIF1                ((uint32_t) (1 << 10))  /* DMA stream 1 clear halt transfer flag */
#define DMA_LIFCR_CTCIF1                ((uint32_t) (1 << 11))  /* DMA stream 1 clear transfer complete flag */

#define DMA_LIFCR_CFEIF2                ((uint32_t) (1 << 16))  /* DMA stream 2 clear FIFO error flag */
#define DMA_LIFCR_CDMEIF2               ((uint32_t) (1 << 18))  /* DMA stream 2 clear direct mode error flag */
#define DMA_LIFCR_CTEIF2                ((uint32_t) (1 << 19))  /* DMA stream 2 clear transfer error flag */
#define DMA_LIFCR_CHTIF2                ((uint32_t) (1 << 20))  /* DMA stream 2 clear halt transfer flag */
#define DMA_LIFCR_CTCIF2                ((uint32_t) (1 << 21))  /* DMA stream 2 clear transfer complete flag */

#define DMA_LIFCR_CFEIF3                ((uint32_t) (1 << 22))  /* DMA stream 3 clear FIFO error flag */
#define DMA_LIFCR_CDMEIF3               ((uint32_t) (1 << 24))  /* DMA stream 3 clear direct mode error flag */
#define DMA_LIFCR_CTEIF3                ((uint32_t) (1 << 25))  /* DMA stream 3 clear transfer error flag */
#define DMA_LIFCR_CHTIF3                ((uint32_t) (1 << 26))  /* DMA stream 3 clear halt transfer flag */
#define DMA_LIFCR_CTCIF3                ((uint32_t) (1 << 27))  /* DMA stream 3 clear transfer complete flag */

#define DMA_HIFCR_CFEIF4                ((uint32_t) (1 << 0))   /* DMA stream 4 clear FIFO error flag */
#define DMA_HIFCR_CDMEIF4               ((uint32_t) (1 << 2))   /* DMA stream 4 clear direct mode error flag */
#define DMA_HIFCR_CTEIF4                ((uint32_t) (1 << 3))   /* DMA stream 4 clear transfer error flag */
#define DMA_HIFCR_CHTIF4                ((uint32_t) (1 << 4))   /* DMA stream 4 clear halt transfer flag */
#define DMA_HIFCR_CTCIF4                ((uint32_t) (1 << 5))   /* DMA stream 4 clear transfer complete flag */

#define DMA_HIFCR_CFEIF5                ((uint32_t) (1 << 6))   /* DMA stream 5 clear FIFO error flag */
#define DMA_HIFCR_CDMEIF5               ((uint32_t) (1 << 8))   /* DMA stream 5 clear direct mode error flag */
#define DMA_HIFCR_CTEIF5                ((uint32_t) (1 << 9))   /* DMA stream 5 clear transfer error flag */
#define DMA_HIFCR_CHTIF5                ((uint32_t) (1 << 10))  /* DMA stream 5 clear halt transfer flag */
#define DMA_HIFCR_CTCIF5                ((uint32_t) (1 << 11))  /* DMA stream 5 clear transfer complete flag */

#define DMA_HIFCR_CFEIF6                ((uint32_t) (1 << 16))  /* DMA stream 6 clear FIFO error flag */
#define DMA_HIFCR_CDMEIF6               ((uint32_t) (1 << 18))  /* DMA stream 6 clear direct mode error flag */
#define DMA_HIFCR_CTEIF6                ((uint32_t) (1 << 19))  /* DMA stream 6 clear transfer error flag */
#define DMA_HIFCR_CHTIF6                ((uint32_t) (1 << 20))  /* DMA stream 6 clear halt transfer flag */
#define DMA_HIFCR_CTCIF6                ((uint32_t) (1 << 21))  /* DMA stream 6 clear transfer complete flag */

#define DMA_HIFCR_CFEIF7                ((uint32_t) (1 << 22))  /* DMA stream 7 clear FIFO error flag */
#define DMA_HIFCR_CDMEIF7               ((uint32_t) (1 << 24))  /* DMA stream 7 clear direct mode error flag */
#define DMA_HIFCR_CTEIF7                ((uint32_t) (1 << 25))  /* DMA stream 7 clear transfer error flag */
#define DMA_HIFCR_CHTIF7                ((uint32_t) (1 << 26))  /* DMA stream 7 clear halt transfer flag */
#define DMA_HIFCR_CTCIF7                ((uint32_t) (1 << 27))  /* DMA stream 7 clear transfer complete flag */

#define DMA_SxCR_EN                     ((uint32_t) (1 << 0))   /* DMA stream enable */
#define DMA_SxCR_DMEIE                  ((uint32_t) (1 << 1))   /* DMA stream direct mode error interrupt enable */
#define DMA_SxCR_TEIE                   ((uint32_t) (1 << 2))   /* DMA stream transmit error interrupt enable */
#define DMA_SxCR_HTIE                   ((uint32_t) (1 << 3))   /* DMA stream half transfer interrupt enable */
#define DMA_SxCR_TCIE                   ((uint32_t) (1 << 4))   /* DMA stream transfer complete interrupt enable */
#define DMA_SxCR_PFCTRL                 ((uint32_t) (1 << 5))   /* DMA stream peripheral flow control */
#define DMA_SxCR_DIR_PM                 ((uint32_t) (0 << 6))   /* DMA stream peripheral-to-memory data transfer */
#define DMA_SxCR_DIR_MP                 ((uint32_t) (1 << 6))   /* DMA stream memory-to-peripheral data transfer */
#define DMA_SxCR_DIR_MM                 ((uint32_t) (2 << 6))   /* DMA stream memory-to-memory data transfer */
#define DMA_SxCR_DIR_MASK               ((uint32_t) (3 << 6))   /* DMA stream data transfer direction mask */
#define DMA_SxCR_CIRC                   ((uint32_t) (1 << 8))   /* DMA stream circular mode enable */
#define DMA_SxCR_PINC                   ((uint32_t) (1 << 9))   /* DMA stream peripheral increment mode enable */
#define DMA_SxCR_MINC                   ((uint32_t) (1 << 10))  /* DMA stream memory increment mode enable */
#define DMA_SxCR_PSIZE_BYTE             ((uint32_t) (0 << 11))  /* DMA stream peripheral data size - Byte */
#define DMA_SxCR_PSIZE_HW               ((uint32_t) (1 << 11))  /* DMA stream peripheral data size - Half-word */
#define DMA_SxCR_PSIZE_WORD             ((uint32_t) (2 << 11))  /* DMA stream peripheral data size - Word */
#define DMA_SxCR_PSIZE_MASK             ((uint32_t) (3 << 11))  /* DMA stream peripheral data size - Mask */
#define DMA_SxCR_MSIZE_BYTE             ((uint32_t) (0 << 13))  /* DMA stream memory data size - Byte */
#define DMA_SxCR_MSIZE_HW               ((uint32_t) (1 << 13))  /* DMA stream memory data size - Half-word */
#define DMA_SxCR_MSIZE_WORD             ((uint32_t) (2 << 13))  /* DMA stream memory data size - Word */
#define DMA_SxCR_MSIZE_MASK             ((uint32_t) (3 << 13))  /* DMA stream memory data size - Mask */
#define DMA_SxCR_PINCOS_LINKED          ((uint32_t) (0 << 15))  /* DMA stream peripheral increment offset size linked to PSIZE */
#define DMA_SxCR_PINCOS_FIXED           ((uint32_t) (1 << 15))  /* DMA stream peripheral increment offset size fixed */
#define DMA_SxCR_PL_LOW                 ((uint32_t) (0 << 16))  /* DMA stream priority level low */
#define DMA_SxCR_PL_MED                 ((uint32_t) (1 << 16))  /* DMA stream priority level medium */
#define DMA_SxCR_PL_HIGH                ((uint32_t) (2 << 16))  /* DMA stream priority level high */
#define DMA_SxCR_PL_VHIGH               ((uint32_t) (3 << 16))  /* DMA stream priority level very high */
#define DMA_SxCR_DBM                    ((uint32_t) (1 << 18))  /* DMA stream double buffer mode */
#define DMA_SxCR_CT                     ((uint32_t) (1 << 19))  /* DMA stream current target (M0 or M1) */
#define DMA_SxCR_PBURST_NO              ((uint32_t) (0 << 21))  /* DMA stream peripheral burst disable */
#define DMA_SxCR_PBURST_4               ((uint32_t) (1 << 21))  /* DMA stream peripheral burst of 4 beats */
#define DMA_SxCR_PBURST_8               ((uint32_t) (2 << 21))  /* DMA stream peripheral burst of 8 beats */
#define DMA_SxCR_PBURST_16              ((uint32_t) (3 << 21))  /* DMA stream peripheral burst of 16 beats */
#define DMA_SxCR_MBURST_NO              ((uint32_t) (0 << 23))  /* DMA stream memory burst disable */
#define DMA_SxCR_MBURST_4               ((uint32_t) (1 << 23))  /* DMA stream memory burst of 4 beats */
#define DMA_SxCR_MBURST_8               ((uint32_t) (2 << 23))  /* DMA stream memory burst of 8 beats */
#define DMA_SxCR_MBURST_16              ((uint32_t) (3 << 23))  /* DMA stream memory burst of 16 beats */
#define DMA_SxCR_CHSEL(x)               ((uint32_t) (x << 25))  /* DMA stream channel select */
#define DMA_SxCR_CHSEL_MASK             ((uint32_t) (0x7 << 25))/* DMA stream channel select mask */

#define DMA_SxFCR_FTH_1                 ((uint32_t) (0 << 0))   /* DMA stream FIFO threshold 1/4 */
#define DMA_SxFCR_FTH_2                 ((uint32_t) (1 << 0))   /* DMA stream FIFO threshold 1/2 */
#define DMA_SxFCR_FTH_3                 ((uint32_t) (2 << 0))   /* DMA stream FIFO threshold 3/4 */
#define DMA_SxFCR_FTH_4                 ((uint32_t) (3 << 0))   /* DMA stream FIFO threshold full*/
#define DMA_SxFCR_DMDIS                 ((uint32_t) (1 << 2))   /* DMA stream direct mode disable */
#define DMA_SxFCR_FS                    ((uint32_t) (7 << 3))   /* DMA stream FIFO status */
#define DMA_SxFCR_FEIE                  ((uint32_t) (1 << 7))   /* DMA stream FIFO error interrupt enable */

#endif
