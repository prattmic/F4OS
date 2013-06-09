#ifndef KERNEL_SVC_H_INCLUDED
#define KERNEL_SVC_H_INCLUDED

#include <stdint.h>

/* Make a SVC call */

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

/* SVC case names */
enum service_calls {
    SVC_YIELD,
    SVC_END_TASK,
    SVC_ACQUIRE,
    SVC_RELEASE,
    SVC_REGISTER_TASK,
};

#endif
