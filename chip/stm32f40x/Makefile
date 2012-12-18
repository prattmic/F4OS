# The goal here is to create chip_$(CHIP).o for the rest of the OS to link against

# /
SRCS += clock.c chip.c

# dev/hw/
VPATH += dev/hw/
SRCS += tim.c

SRCS_$(CONFIG_HAVE_USART) += usart.c
SRCS_$(CONFIG_HAVE_I2C) += i2c.c
SRCS_$(CONFIG_HAVE_SPI) += spi.c

ifdef CONFIG_HAVE_LED
	ifdef CONFIG_STM32_BOARD_DISCOVERY
		SRCS += discovery_led.c
	endif
	ifdef CONFIG_STM32_BOARD_PX4
		SRCS += px4_led.c
	endif
endif

# dev/hw/usb
VPATH += dev/hw/usb
SRCS_$(CONFIG_HAVE_USBDEV) += usbdev_init.c usbdev_int.c usbdev_core.c usbdev_desc.c usbdev_cdc.c usbdev_resource.c

# dev/periph/
VPATH += dev/periph/
ifdef CONFIG_HAVE_I2C
	ifdef CONFIG_STM32_BOARD_DISCOVERY
		SRCS += 9dof_gyro.c
	endif
	ifdef CONFIG_STM32_BOARD_PX4
		SRCS += px4_hmc5883.c px4_ms5611.c
	endif
endif

ifdef CONFIG_HAVE_SPI
	ifdef CONFIG_STM32_BOARD_DISCOVERY
		SRCS += discovery_accel.c
	endif
	ifdef CONFIG_STM32_BOARD_PX4
		SRCS += 
	endif
endif

SRCS += $(SRCS_y)

###################################################

OBJS = $(addprefix $(PREFIX)/, $(SRCS:.c=.o))

###################################################

all: $(PREFIX)/chip_$(CONFIG_CHIP).o

burn:
	./flash.sh $(CONFIG_STM32_BOARD) $(PREFIX)/$(PROJ_NAME).bin

$(PREFIX)/%.o : %.c
	@echo "CC $(subst $(BASE)/,,$(CURDIR))/$<" && $(CC) -MD -c $(CFLAGS) $< -o $@ 
	@cp $(PREFIX)/$*.d $(PREFIX)/$*.P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
			-e '/^$$/ d' -e 's/$$/ :/' < $(PREFIX)/$*.d >> $(PREFIX)/$*.P; \
		rm -f $(PREFIX)/$*.d

-include $(addprefix $(PREFIX)/, $(SRCS:.c=.P))

$(PREFIX)/chip_$(CONFIG_CHIP).o: $(OBJS)
	@echo "LD $(subst $(PREFIX)/,,$@)" && $(LD) -r $^ -o $@
