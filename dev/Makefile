SRCS += resource.c
SRCS += shared_mem.c
SRCS += buf_stream.c
SRCS += device.c

DIRS += hw/
DIRS += accel/
DIRS += baro/
DIRS += gyro/
DIRS += mag/

DIRS_$(CONFIG_MPU6000) += mpu6000/

include $(BASE)/tools/submake.mk
