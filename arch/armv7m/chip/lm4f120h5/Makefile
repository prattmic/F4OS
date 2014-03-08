SRCS += vector.S
SRCS += clock.c
SRCS += gpio.c

DIRS += boards/
DIRS += dev/

binary: $(PREFIX)/$(PROJ_NAME).bin

# Define burn target for flashing chip
burn:
	./flash.sh $(PREFIX)/$(PROJ_NAME).bin

include $(BASE)/tools/submake.mk
