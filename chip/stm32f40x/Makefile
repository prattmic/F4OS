# The goal here is to create chip_$(CHIP).o for the rest of the OS to link against

# /
SRCS += clock.c chip.c

# dev/hw/
VPATH += dev/hw/
SRCS += tim.c

SRCS_$(CONFIG_HAVE_USART) += usart.c
SRCS_$(CONFIG_HAVE_LED) += gpio.c
SRCS_$(CONFIG_HAVE_I2C) += i2c.c
SRCS_$(CONFIG_HAVE_SPI) += spi.c

# dev/hw/usb
VPATH += dev/hw/usb
SRCS_$(CONFIG_HAVE_USBDEV) += usbdev_init.c usbdev_int.c usbdev_core.c usbdev_desc.c usbdev_cdc.c usbdev_resource.c

# dev/periph/
VPATH += dev/periph/
SRCS_$(CONFIG_HAVE_I2C) += 9dof_gyro.c 
SRCS_$(CONFIG_HAVE_SPI) += discovery_accel.c

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
