# Include chip-specific config
include $(BASE)/arch/$(CONFIG_ARCH)/chip/$(CONFIG_CHIP)/config.mk

CROSS_COMPILE ?= arm-none-eabi-

CFLAGS += -mthumb	# ARMv7-M supports Thumb and Thumb2 only
