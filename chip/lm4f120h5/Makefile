# The goal here is to create chip_$(CHIP).o for the rest of the OS to link against

# /
SRCS += clock.c

# dev/hw/
VPATH += dev/hw/
SRCS_$(CONFIG_HAVE_USART) += usart.c
SRCS_$(CONFIG_HAVE_LED) += led.c

burn:
	./flash.sh $(PREFIX)/$(PROJ_NAME).bin

include $(BASE)/tools/submake.mk
