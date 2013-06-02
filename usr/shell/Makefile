SRCS += main.c
SRCS += shell.c
SRCS += ipctest.c
SRCS += top.c
SRCS += uname.c
SRCS += rd_test.c
SRCS += getchar.c
SRCS += shared_deq_test.c
SRCS += list_test.c

SRCS_$(CONFIG_HAVE_LED) 	+= blink.c
SRCS_$(CONFIG_9DOF_GYRO) 	+= ghetto_gyro.c
SRCS_$(CONFIG_HMC5883) 		+= px4_mag.c
SRCS_$(CONFIG_MS5611) 		+= px4_baro.c
SRCS_$(CONFIG_DISCOVERY_ACCEL) += accel.c lowpass.c
SRCS_$(CONFIG_MPU6000)		+= px4_accel_gyro.c
SRCS_$(CONFIG_MM_PROFILING) += mem_perf.c

# Date and rev for uname
DATE := "$(shell date -u)"
SHA := $(shell git log HEAD -n 1 --format='%h')
DIRTY := $(shell git status --porcelain | wc -c)
CFLAGS += -D BUILD_TIME='$(DATE)' -D BUILD_SHA='"$(SHA)"' -D BUILD_DIRTY=$(DIRTY)

include $(BASE)/tools/submake.mk
