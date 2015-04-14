# Core is a Cortex-M4F
include $(BASE)/arch/$(CONFIG_ARCH)/chip/cortex-m4f.mk

# For arch/chip/rom.h
CFLAGS += -DTARGET_IS_MSP432P4XX
