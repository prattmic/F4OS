SRCS += resource.c
SRCS += shared_mem.c
SRCS += buf_stream.c
SRCS += device.c

DIRS += hw/
DIRS_$(CONFIG_ACCELEROMETERS) += accel/
DIRS_$(CONFIG_BAROMETERS) += baro/
DIRS_$(CONFIG_GYROSCOPES) += gyro/
DIRS_$(CONFIG_MAGNETOMETERS) += mag/

DIRS_$(CONFIG_MPU6000) += mpu6000/

include $(BASE)/tools/submake.mk
