# Core is a Cortex-M4F
include $(BASE)/arch/$(CONFIG_ARCH)/chip/cortex-m4f.mk

# For TI driverlib
CFLAGS += -DTARGET_IS_MSP432P4XX -D__MSP432P401R__
