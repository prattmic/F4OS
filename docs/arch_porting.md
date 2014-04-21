Porting to a new architecture
=============================

Porting to a new architecture is more involved than porting to a new chip.
Luckily, F4OS has been designed to be as architecture-agnostic as possible,
outside of the `arch/` folder.

Some of the things that would be necessary to begin porting to a new
architecture:

* Initial system and C runtime startup, eventually calling `os_start()`
* `init_arch()` to do architecture and chip initialization, including
  initializing the system and peripheral clocks
* Exception handling
* Scheduler support, including:
    * A preemptive interrupt for system ticks
        * This should call `sched_system_tick()` at CONFIG_SYSTICK_FREQ.
    * A software interrupt, for service calls
    * Context switching routines
    * A mechanism for differentiating kernel and user stack pointers
    * A list of Arch-provided functions can be found in
      `include/kernel/sched_internals.h`.

