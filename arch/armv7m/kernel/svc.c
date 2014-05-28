/*
 * Copyright (C) 2013, 2014 F4OS Authors
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

void svc_handler(uint32_t*) __attribute__((section(".kernel")));

void svc_handler(uint32_t *registers) {
    uint32_t svc_number;

    /* Stack contains:
     * r0, r1, r2, r3, r12, r14, the return address and xPSR
     * First argument and return value (r0) is registers[0] */
    svc_number = ((char *)registers[6])[-2];

    switch (svc_number) {
        case SVC_YIELD:
        case SVC_END_TASK:
        case SVC_REGISTER_TASK:
        case SVC_TASK_SWITCH:
            registers[0] = sched_service_call(svc_number, registers[0],
                                              registers[1]);
            break;
        case SVC_ACQUIRE:
        case SVC_RELEASE:
            registers[0] = mutex_service_call(svc_number, registers[0]);
            break;
        default:
            panic_print("Unknown SVC: %d", svc_number);
            break;
    }
}
