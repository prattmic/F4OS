TI Tiva C Series Chips
======================

The Tiva C series is a series of ARM Cortex-M4F microcontrollers from TI, with
a maximum frequency of 80MHz-120MHz.

[Manufacturer product page](http://www.ti.com/lsds/ti/microcontroller/tiva_arm_cortex/c_series/overview.page)

The Tiva C series has basic support in F4OS.  Specifically development has
focused on the LM4F120H5QR, aka TM4C1233H6PM.  No regard has been given for
other variants in development, but members of the TM4C123x and TM4C129x
series should be mostly supported.

## Peripherals

### UART
UART0 supported as a standard resource.  It is hardcoded to operate on PA0 and
PA1, at a baud rate of 115200 and 8N1 configuration.

### GPIO
Any of the chip GPIOs can be used with the standard GPIO obj system.

## Flashing

Tiva C chips have on-chip flash, starting at address 0x00000000.  The output
file `f4os.bin` should be written to that address.  The officially supported
board supports flashing with `make burn`, as described below.

## Boards

F4OS officially supports, and includes defconfigs for, one Tiva C-based board

### TI Stellaris Launchpad

The Stellaris Launchpad was TI's primary development board for LM4F series
chips, before renaming the series to Tiva C.  The board has onboard a
LM4F120H5QR, aka TM4C1233H6PM.  Additionally, it has an on-board JTAG
debugger, which can be used to flash the chip.

http://www.ti.com/tool/EK-LM4F120XL

This board has since been superseded by the TI Tiva C Series Launchpad, which
is not officially supported.

The LEDs are supported by F4OS.  They are enumerated as LEDs 0-2.

The provided defconfig can be used to configure F4OS for the Stellaris
Launchpad.

    $ make stellaris_launchpad_defconfig

#### Flashing

`make burn` will use the J-Link Commander software to flash the chip.
This will only work if a Segger J-Link JTAG debugger is connected to
the board JTAG header.

Alternatively, the on-board JTAG debugger can be used to flash the chip, with
a tool such as lm4flash, part of [lm4tools](https://github.com/utzig/lm4tools).

#### Output

The Stellaris Launchpad defconfig configures UART0 as both stdout and stderr,
at a baud rate of 115200.
