# The goal here is to create usr_$(USR).o for the rest of the OS to link against

# /
SRCS += main.c shell.c ipctest.c top.c uname.c rd_test.c getchar.c

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

SRCS += $(SRCS_y)

# Date and rev for uname
DATE := "$(shell date -u)"
SHA := $(shell git log HEAD -n 1 --format='%h')
DIRTY := $(shell git status --porcelain | wc -c)
CFLAGS += -D BUILD_TIME='$(DATE)' -D BUILD_SHA='"$(SHA)"' -D BUILD_DIRTY=$(DIRTY)

include $(BASE)/tools/submake.mk
