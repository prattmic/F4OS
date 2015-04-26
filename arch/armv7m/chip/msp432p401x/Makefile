SRCS += vector.S
SRCS += clock.c
SRCS += gpio.c

DIRS += driverlib/

binary: $(PREFIX)/$(PROJ_NAME).bin

# TODO: support multiple interfaces
burn:
	openocd -f interface/jlink.cfg \
		-c "transport select jtag" \
		-f target/msp432p401r.cfg \
		-c "msp432p4 init 0" \
		-c "msp432p4 mass_erase 0" \
		-c "msp432p4 init 0" \
		-c "flash write_image $(PREFIX)/$(PROJ_NAME).bin" \
		-c "reset run" \
		-c "exit" || true	# OpenOCD returns 1?

include $(BASE)/tools/submake.mk
