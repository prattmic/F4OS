ARMv7-M Support
===============

ARMv7-M is the microcontroller variant of the ARMv7 architecture.
It supports the Thumb and Thumb2 instruction sets.

All of the ARMv7-M chips currently supported are ARM Cortex-M4F chips, however
ARM Cortex-M4 and ARM Cortex-M3 should be supported simply by disabling FPU
support (`CONFIG_HAVE_FPU`).

Non-Cortex-M chips have not been considered, but only minor changes should be
necessary to support them.  The core architecture code is chip agnostic.

## Toolchain

An arm-none-eabi- GNU toolchain is necessary to build for ARMv7-M.  For
FPU support, the GCC build must include hard-float support for Cortex-M4F.
The following versions of the toolchain are known working, including for
FPU support:

* [GCC ARM Embedded](https://launchpad.net/gcc-arm-embedded)
* Debian's arm-none-eabi packages
    * [gcc-arm-none-eabi](https://packages.debian.org/jessie/gcc-arm-none-eabi)
    * [binutils-arm-none-eabi](https://packages.debian.org/jessie/binutils-arm-none-eabi)

## Chip porting

Porting to a new ARMv7-M chip, especially a Cortex-M4 or Cortex-M3 chip, is
fairly simple.

Only a few things should be required to get running:

* A linker script to place sections in the right portion of memory
    * An example can be found in `arch/armv7m/chip/lm4f120h5/link.ld`
* A vector table initialized with the reset and exception vectors
* An `init_clock()` function, which initializes the system and peripheral
  clocks.  The function prototype can be found in `arch/armv7m/include/chip.h`.

At that point, it should be possible to run F4OS on the chip, with all extra
features disabled (no outputs, no GPIOs, etc).  From there, chip peripheral
drivers can be written to provide additional functionality.

The architecture code uses the Cortex-M SysTick timer for preemptive
scheduling.  For chips without this timer, additional work is needed
to disable use of the SysTick timer, and to use another timer for preemptive
scheduling.
