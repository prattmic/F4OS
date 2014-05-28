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

#include <stdint.h>
#include <kernel/fault.h>
#include <kernel/sched.h>
#include <kernel/sched_internals.h>
#include <kernel/mutex.h>
#include <arch/system_regs.h>
#include "sched_asm.h"

void __attribute__((section(".kernel"))) svc_handler(struct stacked_registers *registers) {
    uintptr_t pc;
    uint32_t svc_number;

    /* Initial task switch! */
    if (!task_switching && (registers->cpsr & CPSR_MODE) == CPSR_MODE_USR) {
        sched_service_call(SVC_YIELD);
        return;
    }

    pc = (uintptr_t) registers->pc;

    /* Extract SVC number from ARM or Thumb SVC instruction */
    if (registers->cpsr & CPSR_THUMB) {
        uint8_t *svc = (uint8_t *) (pc - 2);
        svc_number = svc[0];
    }
    else {
        uint8_t *svc = (uint8_t *) (pc - 4);
        svc_number = svc[0] | (svc[1] << 8) | (svc[2] << 16);
    }

    switch (svc_number) {
        case SVC_YIELD:
        case SVC_END_TASK:
        case SVC_REGISTER_TASK:
        case SVC_TASK_SWITCH:
            registers->r0 = sched_service_call(svc_number, registers->r0,
                                               registers->r1);
            break;
        case SVC_ACQUIRE:
        case SVC_RELEASE:
            registers->r0 = mutex_service_call(svc_number, registers->r0);
            break;
        default:
            panic_print("Unknown SVC: %d", svc_number);
            break;
    }
}
