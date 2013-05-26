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

DIRS_$(CONFIG_HAVE_USBDEV) += usb/

include $(BASE)/tools/submake.mk
