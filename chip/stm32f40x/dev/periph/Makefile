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
		SRCS += px4_mpu6000.c
	endif
endif

include $(BASE)/tools/submake.mk
