STM32F4 Series Chips
====================

The STM32F4 series is a series of ARM Cortex-M4F microcontrollers, with a
maximum frequency of 84MHz-180MHz.

[Manufacturer product page](http://www.st.com/stm32f4)

At present, the STMicro's STM32F4 chip series is the best supported by F4OS,
and its namesake.  Support is currently targeted primarily at the STM32F405
and STM32F407 chips, without regard for other variants, but they are generally
compatible, and should work with only minor modifications.  In time, other
variants will be officially supported.

## Peripherals

### UART
UART1 supported as a standard resource.  It is hardcoded to operate on PB6 and
PB7, at a baud rate of 115200 and 8N1 configuration.

### USB
The USB OTG LS interface is supported as a USB CDC ACM slave.  It is
implemented as a standard resource.  It enumerates the device as an ACM device
on a host machine (e.g., `/dev/ttyACM0`).

### SPI
SPI1, SPI2, and SPI3 are supported as standard SPI objs.  Their pins are
specified in the STM32F4 device tree source.  The peripherals are set to a
default clock of PCLK/8.  For SPI1, this equates to `CONFIG_SYS_CLOCK/32`.
For SPI2 and SPI3, this equates to `CONFIG_SYS_CLOCK/16`.

### I2C
I2C1, I2C2, and I2C3 are supported as standard I2C objs.  Their pins are
specified in the STM32F4 device tree source.  The peripherals have default
clock divisors to provide a 300kHz clock when `CONFIG_SYS_CLOCK` is 168MHz.

### PWM
Several of the timers in the STM32F4 are used to provide a standard PWM
interface.  Most pins that support timer outputs support use with the PWM
interface.

### GPIO
Any of the chip GPIOs can be used with the standard GPIO obj system.

## Flashing

STM32F4 chips have on-chip flash, starting at address 0x08000000.  The output
file `f4os.bin` should be written to that address.  The officially supported
boards support flashing with `make burn`, as described below.

## Boards

F4OS officially supports, and includes defconfigs for, two STM32F4-based boards

### STM32F4DISCOVERY

The STM32F4DISCOVERY is STMicro's primary development board for the STM32F407.
It has an STM32F407VGT6, and an integrated ST-LINK V2 JTAG debugger.  Onboard,
it also has several LEDs, an accelerometer, microphone, and buttons.

http://www.st.com/web/catalog/tools/FM116/SC959/SS1532/PF252419

The LEDs and accelerometer, an STMicro LIS302DL, are supported by F4OS.  The
LEDs are enumerated as LEDs 0-3, and the accelerometer is enumerated as a
standard accelerometer obj.

The provided defconfig can be used to configure F4OS for the Discovery board.

    $ make discovery_defconfig

#### Flashing

`make burn` will use the `st-flash` tool, provided by the
[stlink](https://github.com/texane/stlink) project to flash the binary using
the onboard ST-LINK V2 debugger.

#### Output

The STM32F4DISCOVERY defconfig configures the USB peripheral as stdout, and
UART1 as stderr, at a baud rate of 115200.  Connect the Discovery board's
Micro USB port to your host machine, and it should be enumerated as an ACM
device, which can be used with applications like screen:

    $ screen /dev/ttyACM0

### PX4FMU 1.x

The PX4FMU is an autopilot designed by the Pixhawk project.  Only the 1.x
versions of the board are officially supported by F4OS.  They have the
STM32F405RGT6.  Onboard, it has a 6-axis IMU, magnetometer, and barometer.

https://pixhawk.org/modules/px4fmu

The IMU is an InvenSense MPU-6000.  It enumerates as a special mpu6000 obj,
providing both a standard accelerometer obj, and an standard gyroscope object.

The magnetometer is a Honeywell HMC5883L.  It enumerates as a standard
magnetometer obj.

The barometer is a Measurment Specialists MS5611-01BA03.  It enumerates as a
standard barometer obj.

The two LEDs enumerate as LEDs 0-1.

The provided defconfig can be used for configure F4OS for the PX4FMU.

    $ make px4_defconfig

#### Flashing

The PX4FMU does not include an on-chip debugger.  Pads for an ARM mini 10-pin
JTAG header are available on the board.  Once a header has been added, any
ARM JTAG debugger may be used to flash the board, using the debugger's
standard flashing tool.

`make burn` will use the J-Link Commander software to flash the chip.
This will only work if a Segger J-Link JTAG debugger is connected to
the board JTAG header.

#### Output

The PX4FMU defconfig configures the USB peripheral as stdout, and UART1 as
stderr, at a baud rate of 115200.  Connect the Micro USB port to your host
machine, where it should be enumerated as an ACM device, which can be used
with applications like screen:

    $ screen /dev/ttyACM0

UART1 is available on pins 7 and 8 of the expansion header.  More information
is available in the [schematic](http://pixhawk.org/_media/modules/px4fmuv1.7.pdf).
