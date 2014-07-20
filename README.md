F4OS
======

F4OS is a small real-time operating system intended for use in embedded
applications.  F4OS is designed to be chip-agnostic, fairly simple to port
to new chips, or even new architectures.  To that end, the hardware abstration
model is designed to be as generic as possible, so that only minimal
configuration changes are required to move between chips.

F4OS was originally developed on the STMicro STM32F4DISCOVERY Board, which is
where the name F4OS originates.

## Supported Hardware

F4OS currently supports the following architectures:

* ARMv7-A
    * Currently entirely MMU-less rudimentary support
    * See [ARMv7-A docs](docs/armv7a.md) for more details on support for
      this architecture
* ARMv7-M
    * See [ARMv7-M docs](docs/armv7m.md) for more details on support for
      this architecture

Currently, the supported chips include the following.  See the chip
documentation pages for more details on chip support.

| Architecture  | Chip                                                          | Officially Supported Boards                     |
| ------------- | ------------------------------------------------------------- | -----------------------------                   |
| ARMv7-M       | [STMicro STM32F4 series](docs/stm32f4.md)                     | STM32F4DISCOVERY, 32F401CDISCOVERY, PX4FMU 1.x  |
| ARMv7-M       | [TI Tiva C series](docs/tivac.md), aka TI Stellaris LM4F      | TI Stellaris Launchpad                          |
| ARMv7-A       | [TI Sitara AM335x series](docs/am335x.md)                     | BeagleBone Black                                |

## Building F4OS

### Requirements

A cross-compiling toolchain is required to build F4OS for the target
architecture.  See the architecture file in `docs/` for recommended toolchains.

Runtime configuration is performed using flattened device trees.  The device
tree compiler (dtc) is required to process device tree files.  This compiler
is available in most package managers as dtc or device-tree-compiler.  It can
also be downloaded from the
[source repository](http://git.jdl.com/gitweb/?p=dtc.git;a=summary).

F4OS uses the Kconfig language for its build configuration, and needs at least
the `conf` tool for processing KConfig files.  This tool is distibuted with the
Linux kernel, but it and other tools are packaged for external use by the
[kconfig-frontends](http://ymorin.is-a-geek.org/projects/kconfig-frontends)
project.

On its first run, the F4OS build system will search for the Kconfig tools,
and if not found, will offer to automatically download and build the
kconfig-frontends project, so manually building and installing this project
is unnecessary.

kconfig-frontends does itself have several dependencies which must be installed
on the host system in order to build the project.  These include: autoconf,
automake, bison, flex, gperf, libtoolize, libncurses5-dev, m4, and pkg-config.

### Configuration and building

F4OS uses the Kconfig language for build configuration, so a configuration must
be specified before building.

Menuconfig can be used to manually select configuration options.

    $ make menuconfig

However, defconfigs are provided for all supported chips, which provide a
standard configuration for that chip.  The help displays a list of available
defconfigs.

    $ make help

To configure for the STM32F4DISCOVERY board, simply use its defconfig.

    $ make discovery_defconfig

Once configured, it is simply to build the entire OS.

    $ make

### Flashing/Booting

Building F4OS will generate `out/f4os.elf`, an ELF object, and an appropriate
binary for running the OS on the configured chip.  In order to actually run
F4OS, it needs to be flashed/booted on the chip it was built for.

See the documentation page for the configured chip for details on flashing or
booting.  For chips with internal flash, `make burn` is generally used to
automate the process of flashing the OS.

## Using F4OS

### User Application

After OS initialization, F4OS will run the user application built into binary.
This is one of the directories in the `usr/` folder.  By default, the
simple shell in `usr/shell/` is built, providing several basic programs.

The folder with the user application to build is specified by the USR
environmental variable.  For example, to build the test suite, specify
`USR=tests`.

    $ USR=tests make

Adding a custom application is easy, and has only a few requirements.

1. The application must be placed in a subdirectory of `usr/`.  This is to
   ensure it can be properly selected with the `USR` environmental variable.
2. The directory must have a Makefile in the format described below, in
   "Extending F4OS".  A large directory hierarchy can be specified, it just
   needs to follow the build system format in order to build properly.
3. The application must define a `main()` function.  This function is declared
   in `include/kernel/sched.h`, and should create the application tasks that
   should be scheduled at boot.

Other than those simple requirements, an application is free to do whatever
it would like.  Other than a few kernel tasks, the application tasks will be
the only tasks running on the chip.

A little more information about the `main()` function:  This function is called
just before the scheduler begins running tasks.  The purpose of this function
is for the user application to create the initial tasks it would like scheduled
at system boot.  If no tasks are created in `main()`, then the only tasks
available for scheduling will be the kernel tasks, and the user application
will never run.  Task creation is described below in "Extending F4OS".

The shell application is a good example application.  It has a fairly
full-featured Makefile at `usr/shell/Makefile`, and a basic `main()` in
`usr/shell/main.c`.  This application simply creates a single task, which
runs continuously, listening for user input.

### Standard Output and Standard Error

F4OS can utilize different devices for the standard output and standard error
interfaces.  Various configurations use different interfaces, but they can be
manually selected in the "Drivers" menu of the configuration.  The options are
"Standard output device" and "Standard error device".

    $ make menuconfig

See specific chip documentation pages for for more information about board
default stdout and stderr devices.

## Extending F4OS

F4OS is designed to be easy to extend, particularly with user applications
and device drivers.  Here are some notes that will aide in extending F4OS.

### Tasks

Tasks are the fundamental units run by F4OS, enabling the system to multitask.
Each task consists of a function pointer to the start of the task, a private
stack, a priority, and, if periodic, a period.

The F4OS scheduler will task switch between the registered tasks, based on
their priority and period.  The context switches are entirely transparent.
From a task's perspective, it is always running.

F4OS uses a rate-monotonic scheduler, so higher priority tasks always run in
favor of lower priority tasks, while equal priority tasks are scheduled in a
round-robin fashion.

In order to prevent starvation, task priorities should be assigned inversely
proportional to their runtime.  That is, tasks that run for a short time may
have high priorities, while those that run for a long time should have a lower
priority.  Tasks that run indefinitely should have the lowest priority.

Non-periodic tasks are destroyed upon return, while periodic tasks are
restarted at each period.  Periodic tasks may destroy themselves with a call
to `abort()`.

F4OS is a soft real-time operating system.  Periodic tasks will be queued to
run precisely at their period tick, however the actual task scheduled is
dependent upon the task priority, so if a higher priority task is available,
task deadlines may be missed.

Tasks are created using the `new_task()` function defined in
`include/kerne/sched.h`:

    task_t *new_task(void (*fptr)(void), uint8_t priority, uint32_t period);

This function takes the function used to start the task, the task priority,
and the task period in system ticks.  The system tick frequency can be
configured with `CONFIG_SYSTICK_FREQ`.  It returns a pointer to the task,
which can be used in various other scheduler API functions.

### Makefiles

F4OS uses a recursive Make-based build system, which uses a simple Makefile
in each directory to describe the build.  Here is a sample Makefile:

    SRCS += file1.c
    SRCS_$(CONFIG_BUILD_FILE2) += file2.c

    DIRS_$(CONFIG_BUILD_SUBDIR) += subdir/

    CFLAGS += -DEXTRA_OPTION

    include $(BASE)/tools/submake.mk

Let's break down what is going on here.

The build system will build all sources in `SRCS` or `SRCS_y`, so `file1.c`
will be built unconditionally.  `CONFIG_BUILD_FILE2` is a Make variable
provided based on the Kconfig option `BUILD_FILE2`.  If the option is enabled,
the variable will expand to 'y'.  If it is not enabled, it will expand either
to 'n' or to nothing.  Concatenated with `SRCS_`, this adds the file either to
`SRCS_`, `SRCS_n`, or `SRCS_y`.  Only files in `SRCS_y` are built.

Like sources, the build system will build directories in either `DIRS` or
`DIRS_y`.  Thus, subdirectories can be built conditionally, just like sources.
Note that `subdir/` should have a Makefile in this format as well.

The standard flag variables (CFLAGS, LFLAGS, MAKEFLAGS) can be extended in the
Makefile.  Changes will affect all files built in this directory and all
subdirectories.

The `include` line is required, and includes all of the magic necessary to
actually build the sources and recurse into the subdirectories.

### Kconfig

F4OS uses the Kconfig configuration language to configure builds of the OS.
Full documentation for the language can be found as part of
[kconfig-frontends](http://ymorin.is-a-geek.org/git/kconfig-frontends/tree/docs/kconfig-language.txt).

Kconfig files describe the options, and their defaults and dependencies.
Options with a prompt will be available for selection in the interactive
menuconfig.

    $ make menuconfig

A set of default configuration options is called a defconfig, and several are
provided for supported chips in `configs/`.

Additional menus and options may be added to allow additional configuration of
F4OS.

The selected options are automatically included in all Makefiles and source
files, with the `CONFIG_` prefix.

### Headers

Most global C headers are kept in `include/`, the standard system include path.
However, architecture- and chip-specific headers are kept in
`arch/$ARCH/include/`, and `arch/$ARCH/chip/$CHIP/include/`, respectively.
These headers can be included simply using the `arch/` or `arch/chip/` prefix.
For example:

    #include <arch/arch_header.h>
    #include <arch/chip/chip_header.h>

Note that use of arch- and chip-specific headers should be avoided outside of
the arch and chip directories, as the same headers may not be available for
other architectures or chips.

If an arch-specific feature is needed globally, a global interface should use
declared and used, simply implemented for the architecture.

### Drivers

Device drivers are written in F4OS using a generic object framework, defined
in several headers, including `include/kernel/obj.h` and
`include/kernel/class.h`.

This model allows generic objects to be passed around, but used powerfully
when their type is known.  All objects of the same type, or class, provide
a standard interface that is best fit for that type.  For example, all I2C
drivers provide the same interface, as do all accelerometers.  If an
accelerometer depends on a I2C bus, it can request the I2C object and use
its interface, regardless of which driver backs the I2C object.

Even more usefully, this allows an application to request an "accelerometer",
not even having to know what hardware is being used.

While there are a few legacy drivers using an old character-based interface,
all new drivers should be written using the object model.  The LIS302DL
accelerometer driver, found in `dev/accel/lis302dl.c`, provides a good example
of a driver written using the object model.

Since the is all accelerometer, it must implement the accelerometer operations,
defined in `include/dev/accel.h`.

The driver is registered with the device driver system using an OS initializer
(a function which will run at boot), providing information on how to initialize
the driver.

In the driver initialization, the driver is added as an accelerometer object,
gets the object for its parent bus, and gets objects for the GPIOs it will use,
and configures those.

Board/chip physical configurations and interconnects are described by the
configured device tree source file.  The DTS file to use for a given build
is configured with `CONFIG_DEVICE_TREE`.

### Chip and Architecture Ports

F4OS aims to make porting to new architectures, and especially new chips, as
easy as possible.

For information on porting to a new chip on a supported architecture, see the
architecture file in `docs/`.

For information on porting to a new architecture, see the
[architectyre porting document](docs/arch_porting.md).

## Contributing

F4OS would love your contributions to the project!  The vast majority of
contributions are handled through [Gerrit Code Review](http://gerrit.pratt.im),
however we will accept contributions via GitHub Pull Requests as well.
