/*
 * This file is to be included when a compilation unit
 * wants to disable profiling but CONFIG_PERFCOUNTER is enabled
 */

#define PROF_DEFINE_COUNTER(name)

#define PROF_USING_COUNTER(name)

#define PROF_START_COUNTER(name) do {} while(0)

#define PROF_STOP_COUNTER(name) do {} while(0)

#define PROF_COUNTER_DELTA(name) 0
