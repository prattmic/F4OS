SRCS += vector.S
SRCS += clock.c
SRCS += rcc.c
SRCS += gpio.c
SRCS_$(CONFIG_PWM_CLASS) += pwm.c

DIRS += boards/
DIRS += dev/

binary: $(PREFIX)/$(PROJ_NAME).bin

# Define burn target for flashing chip
burn:
	./flash.sh $(CONFIG_STM32_BOARD) $(PREFIX)/$(PROJ_NAME).bin

include $(BASE)/tools/submake.mk
