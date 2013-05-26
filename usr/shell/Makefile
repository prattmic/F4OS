SRCS += main.c
SRCS += shell.c
SRCS += ipctest.c
SRCS += top.c
SRCS += uname.c
SRCS += rd_test.c
SRCS += getchar.c

SRCS_$(CONFIG_HAVE_LED) += blink.c

ifdef CONFIG_HAVE_I2C
	ifdef CONFIG_STM32_BOARD_DISCOVERY
		SRCS += ghetto_gyro.c
	endif
	ifdef CONFIG_STM32_BOARD_PX4
		SRCS += px4_mag.c px4_baro.c
	endif
endif
ifdef CONFIG_HAVE_SPI
	ifdef CONFIG_STM32_BOARD_DISCOVERY
		SRCS += accel.c lowpass.c
	endif
	ifdef CONFIG_STM32_BOARD_PX4
		SRCS += px4_accel_gyro.c
	endif
endif

# Date and rev for uname
DATE := "$(shell date -u)"
SHA := $(shell git log HEAD -n 1 --format='%h')
DIRTY := $(shell git status --porcelain | wc -c)
CFLAGS += -D BUILD_TIME='$(DATE)' -D BUILD_SHA='"$(SHA)"' -D BUILD_DIRTY=$(DIRTY)

include $(BASE)/tools/submake.mk
