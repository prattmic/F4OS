CFLAGS += -mcpu=cortex-a8
CFLAGS += -mfloat-abi=hard	# Use FPU and hard float point calling conventions
CFLAGS += -mfpu=vfpv3-d16	# VFPv3 FPU

# Cortex-A8 does not have a division instruction, use libgcc
LFLAGS += -lgcc
