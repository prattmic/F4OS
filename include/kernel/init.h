/*
 * INITIALIZERS - Initialize something at system start
 *
 * These are essentially the same as initcalls in linux, but simpler.
 * In the future, these may be replaced by constructors for global objs,
 * if everything switches over to using objs. These may still have a place
 * afterwards though.
 *
 * I may also make an initializer a struct, containing the initializer
 * function and its stringified name, making it easy to print out which
 * initializers fail to run, if there is ever a need for it.
 *
 * There are three ypes of initializers, all run in the following order:
 * early_initializer
 * core_initializer
 * late_initializer
 */

typedef int (*initializer_t)(void);

extern initializer_t __begin_early_initializer, __end_early_initializer;
extern initializer_t __begin_core_initializer, __end_core_initializer;
extern initializer_t __begin_late_initializer, __end_late_initializer;

#define __initializer(fn, stage) \
    initializer_t initializer_##fn __attribute__((unused, section(".initializers." #stage))) = fn;

#define EARLY_INITIALIZER(fn) __initializer(fn, early)

#define CORE_INITIALIZER(fn) __initializer(fn, core)

#define LATE_INITIALIZER(fn) __initializer(fn, late)

void do_early_initializers(void);
void do_core_initializers(void);
void do_late_initializers(void);
