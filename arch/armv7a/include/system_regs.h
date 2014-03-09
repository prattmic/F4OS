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

#ifndef ARCH_SYSTEM_REGS_H_INCLUDED
#define ARCH_SYSTEM_REGS_H_INCLUDED

#define CPSR_MODE       (0x1f)  /* CPSR processor mode mask */
#define CPSR_MODE_USR   (0x10)  /* User mode */
#define CPSR_MODE_FIQ   (0x11)  /* FIQ mode */
#define CPSR_MODE_IRQ   (0x12)  /* IRQ mode */
#define CPSR_MODE_SVC   (0x13)  /* SVC mode */
#define CPSR_MODE_MON   (0x16)  /* Monitor mode */
#define CPSR_MODE_ABT   (0x17)  /* Abort mode */
#define CPSR_MODE_HYP   (0x1A)  /* Hypervisor mode */
#define CPSR_MODE_UND   (0x1B)  /* Undefined mode */
#define CPSR_MODE_SYS   (0x1F)  /* System mode */

#define CPSR_THUMB      (0x20)  /* Thumb execution state */

#endif
