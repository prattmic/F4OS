#include <stdint.h>
#include <stddef.h>
#include <kernel/sched.h>
#include <kernel/semaphore.h>
#include <dev/cortex_m.h>
#include <dev/resource.h>
#include <dev/hw/itm.h>

/* The Instrumental Trace Macrocell is an ARM debug extension that
 * provides a logging/tracing interface over the debug interface;
 * in this case, SWO.  All Cortex-M processors support ITM. */

/* This prescaler sets the baud rate of SWO */
#define SWO_PRESCALER   ((CONFIG_SYS_CLOCK / CONFIG_SWO_FREQ) - 1)

struct semaphore itm_semaphore;

uint8_t itm_ready = 0;

#define DEBUG_ENABLED   (*DEBUG_DHCSR & DEBUG_DHCSR_DEBUGEN)

void init_itm(void) {
    /* Enable ITM debug extention */
    *DEBUG_DEMCR |= DEBUG_DEMCR_TRCENA;

    /* Set trace port to SWO (NRZ) output mode */
    *TPIU_SPPR = TPIU_SPPR_SWO_NRZ;

    /* Set SWO baud rate */
    *TPIU_ACPR = SWO_PRESCALER;

    /* Formatting on trigger */
    *TPIU_FFCR = TPIU_FFCR_TRIGIN;

    /* Unlock ITM registers */
    *ITM_LAR = CORESIGHT_LOCK_MAGIC;

    /* Enable ITM, including sync packets.  Set bus ID */
    *ITM_TCR = ITM_TCR_ITMENA | ITM_TCR_SYNCENA | ITM_TCR_TXENA | ITM_TCR_BUSID(CONFIG_ITM_BUS_ID);

    /* Enable ITM port 0 */
    *ITM_TER(0) |= 1;

    init_semaphore(&itm_semaphore);

    itm_ready = 1;
}

static int itm_write(char c, void *env) {
    /* Don't bother writing if debugging isn't enabled */
    if (itm_ready && DEBUG_ENABLED) {
        /* Wait for port to be ready */
        while (!*ITM_STIM(0)) {
            yield_if_possible();
        }

        *ITM_STIM(0) = c;

        return 1;
    }
    else {
        return -1;
    }
}

/* No reading */
static char itm_read(void *env, int *error) {
    if (error) {
        *error = -1;
    }
    return 0;
}

/* No closing */
static int itm_close(struct resource *resource) {
    return -1;
}

resource itm_port = {   .writer     = &itm_write,
                        .swriter    = NULL,
                        .reader     = &itm_read,
                        .closer     = &itm_close,
                        .env        = NULL,
                        .read_sem   = &itm_semaphore,
                        .write_sem  = &itm_semaphore};
