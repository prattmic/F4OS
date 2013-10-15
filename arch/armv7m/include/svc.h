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

#ifndef ARCH_SVC_H_INCLUDED
#define ARCH_SVC_H_INCLUDED

#include <stdint.h>
#include <arch/system.h>

/* We need to make sure that we get the return value
 * without screwing up r0, since GCC doesn't understand that
 * SVC has a return value */
#define SVC(call)  ({ \
    uint32_t ret = 0;   \
    asm volatile ("svc  %[code]  \n"    \
                  "mov  %[ret], r0  \n" \
                  :[ret] "+r" (ret)     \
                  :[code] "I" (call)    \
                  :"r0");               \
    ret;    \
})

#define SVC_ARG(call, arg)  ({ \
    uint32_t ret = 0;   \
    asm volatile ("mov  r0, %[ar]  \n"  \
                  "svc  %[code]  \n"    \
                  "mov  %[ret], r0  \n" \
                  :[ret] "+r" (ret)     \
                  :[code] "I" (call), [ar] "r" (arg)     \
                  :"r0");               \
    ret;    \
})

#define SVC_ARG2(call, arg1, arg2)  ({ \
    uint32_t ret = 0;   \
    asm volatile ("mov  r0, %[ar1]  \n"  \
                  "mov  r1, %[ar2]  \n"  \
                  "svc  %[code]  \n"    \
                  "mov  %[ret], r0  \n" \
                  :[ret] "+r" (ret)     \
                  :[code] "I" (call), [ar1] "r" (arg1), [ar2] "r" (arg2)     \
                  :"r0", "r1");               \
    ret;    \
})

static inline int arch_svc_legal(void) {
    return !IPSR();
}

#endif
