SRCS += vector.S
SRCS += clock.c

DIRS += dev/

# Define burn target for flashing chip
burn:
	./flash.sh $(CONFIG_STM32_BOARD) $(PREFIX)/$(PROJ_NAME).bin

include $(BASE)/tools/submake.mk
