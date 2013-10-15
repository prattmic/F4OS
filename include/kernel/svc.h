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

#ifndef KERNEL_SVC_H_INCLUDED
#define KERNEL_SVC_H_INCLUDED

/*
 * Arch specific implementation for making service calls
 *
 * Provides static inline functions or macros that behave as follows:
 */
#include <arch/svc.h>

/*
 * Performs a service call with no arguments
 *
 * @param call  Service call number
 * @returns Return value for service call
 * int SVC(int call);
 */

/*
 * Performs a service call with 1 argument
 *
 * @param call  Service call number
 * @param arg   Arguement to service call
 * @returns Return value for service call
 * int SVC_ARG(int call, void *arg);
 */

/*
 * Performs a service call with 2 argument
 *
 * @param call  Service call number
 * @param arg1  Arguement 1 to service call
 * @param arg2  Arguement 2 to service call
 * @returns Return value for service call
 * int SVC_ARG(int call, void *arg1, void *arg2);
 */

/*
 * Determine if a service call is allowed
 *
 * On many architectures, there a cases when a service call cannot be made,
 * such as in interrupt space.  This function determines if a service call
 * can be made.
 *
 * @returns non-zero if a service call can be made
 * int arch_svc_legal(void);
 */

/* Available service calls */
enum service_calls {
    SVC_YIELD,
    SVC_END_TASK,
    SVC_ACQUIRE,
    SVC_RELEASE,
    SVC_REGISTER_TASK,
    SVC_TASK_SWITCH,
};

#endif
