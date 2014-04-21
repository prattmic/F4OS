ARMv7-A Support
===============

ARMv7-A is the application profile of the ARMv7 architecture.
It supports the ARM, Thumb, and Thumb2 instruction sets.

Currently support for ARMv7-A is very basic.  The MMU is never enabled,
nor are the instruction or data caches.

## Toolchain

An arm-none-eabi- GNU toolchain is necessary to build for ARMv7-A.
The following versions of the toolchain are known working:

* [GCC ARM Embedded](https://launchpad.net/gcc-arm-embedded)
* Debian's arm-none-eabi packages
    * [gcc-arm-none-eabi](https://packages.debian.org/jessie/gcc-arm-none-eabi)
    * [binutils-arm-none-eabi](https://packages.debian.org/jessie/binutils-arm-none-eabi)

## Chip porting

Porting to a new ARMv7-A chip, is fairly simple.

Only a few things should be required to get running:

* A linker script to place sections in the right portion of memory
    * An example can be found in `arch/armv7a/chip/am335x/link.ld`
* An `init_chip()` function, which initializes the system and peripheral
  clocks.  The function prototype can be found in `arch/armv7a/include/chip.h`.
* `irq_handler()` and `fiq_handler()` functions, which lookup and handle IRQ
  and FIQ interrupts as they occur.
* An `init_systick()` function, which initializes a timer to preempt the
  system at CONFIG_SYSTICK_FREQ, and runs the scheduler.  The interrupt
  handler should increment system_ticks, call rtos_tick(), and call
  task_switch().  The function prototype can be found in
  `include/dev/hw/systick.h`.

At that point, it should be possible to run F4OS on the chip, with all extra
features disabled (no outputs, no GPIOs, etc).  From there, chip peripheral
drivers can be written to provide additional functionality.
