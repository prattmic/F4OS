/* This file needs stdint.h and dev/hw/perfcounter.h to be included */
#ifdef CONFIG_PERFCOUNTER

#define PROF_DEFINE_COUNTER(name) \
    uint64_t name##_start_timestamp, name##_end_timestamp, name##_total

#define PROF_USING_COUNTER(name) \
    extern uint64_t name##_start_timestamp, name##_end_timestamp, name##_total

#define PROF_START_COUNTER(name) do {               \
    name##_start_timestamp = perfcounter_getcount();\
    } while(0);

#define PROF_STOP_COUNTER(name) do {                                \
    name##_end_timestamp = perfcounter_getcount();                \
    name##_total += name##_end_timestamp - name##_start_timestamp;  \
    } while(0)

#define PROF_COUNTER_DELTA(name) (name##_end_timestamp - name##_start_timestamp)

#else

#include <profiling_stubs.h>

#endif
