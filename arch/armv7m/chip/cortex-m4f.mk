# Cortex-M4F has a single precision floating point unit

CFLAGS += -mcpu=cortex-m4	# STM32F4 is a Cortex-M4F
CFLAGS += -mfloat-abi=hard	# Use FPU and hard float point calling conventions
CFLAGS += -mfpu=fpv4-sp-d16	# Core contains an FPv4-SP-D16 FPU
CFLAGS += -fsingle-precision-constant	# Contants are floats, not doubles
CFLAGS += -fshort-double	# Treat doubles as 32-bit floats
CFLAGS += -Wdouble-promotion# Warn when a float is promoted to a double
