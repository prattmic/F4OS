# Cortex-M4F has a single precision floating point unit

CFLAGS += -mcpu=cortex-m4

ifeq ($(CONFIG_HAVE_FPU),y)
CFLAGS += -mfloat-abi=hard	# Use FPU and hard float point calling conventions
CFLAGS += -mfpu=fpv4-sp-d16	# Core contains an FPv4-SP-D16 FPU
else
CFLAGS += -lgcc		# libgcc to provide floating point routines
endif

ifeq ($(CONFIG_SHORT_DOUBLE),y)
CFLAGS += -fsingle-precision-constant	# Contants are floats, not doubles
CFLAGS += -fshort-double	# Treat doubles as 32-bit floats
else
CFLAGS += -lgcc		# libgcc to provide double precision floating point routines
endif
