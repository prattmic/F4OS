# End users specify objects here

# usr/shell/
USR_VPATH = usr/shell/
USR_CFLAGS = -Iusr/shell/
USR_SRCS = main.c shell.c accel.c blink.c ghetto_gyro.c ipctest.c top.c uname.c lowpass.c

##########################
LINK_SCRIPT = boot/link.ld

UNIT_TESTS ?= 0

ifeq ($(UNIT_TESTS),1)
# usr/unit_tests/
VPATH = usr/unit_tests/
CFLAGS = 
SRCS = main.c
else
VPATH = $(USR_VPATH)
CFLAGS = $(USR_CFLAGS)
SRCS = $(USR_SRCS)
endif

# boot/
VPATH += boot/
SRCS += boot_main.c
ASM_SRCS += boot_asm.S

# dev/
VPATH += dev/
SRCS += resource.c shared_mem.c

# dev/hw
VPATH += dev/hw/
SRCS += i2c.c spi.c systick.c tim.c usart.c

# dev/periph/
VPATH += dev/periph/
SRCS += 9dof_gyro.c discovery_accel.c

# kernel/
VPATH += kernel/
SRCS += fault.c semaphore.c

# kernel/sched/
VPATH += kernel/sched/
SRCS += kernel_task.c sched_end.c sched_interrupts.c sched_new.c sched_start.c sched_swap.c sched_switch.c
ASM_SRCS += sched_asm.S

# lib/
VPATH += lib/
SRCS += stdio.c string.c

# lib/math/
VPATH += lib/math/
SRCS += math_newlib.c math_other.c math_pow.c math_trig.c

# mm/
VPATH += mm/
SRCS += mm_free.c mm_init.c mm_malloc.c mm_space.c

# all the files will be generated with this name (main.elf, main.bin, main.hex, etc)

PROJ_NAME=f4os
PREFIX = ./out

# that's it, no need to change anything below this line!

###################################################

CC=arm-none-eabi-gcc
LD=arm-none-eabi-ld
OBJCOPY=arm-none-eabi-objcopy

CFLAGS += -g3 -Wall --std=gnu99 -isystem include/
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork -Xassembler -mimplicit-it=thumb
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16 -nostdlib -ffreestanding
CFLAGS += -Wdouble-promotion -fsingle-precision-constant -fshort-double

DATE := "$(shell date -u)"
REV := $(shell git rev-list HEAD | wc -l)
CFLAGS += -D BUILD_TIME='$(DATE)' -D BUILD_REV=$(REV)

#CFLAGS += -save-temps --verbose -Xlinker --verbose

LFLAGS=

###################################################

OBJS = $(addprefix $(PREFIX)/, $(SRCS:.c=.o))
OBJS += $(addprefix $(PREFIX)/, $(ASM_SRCS:.S=.o))

###################################################

.PHONY: proj unoptimized

all: CFLAGS += -O2
all: $(PREFIX) proj

unoptimized: CFLAGS += -O0
unoptimized: $(PREFIX) proj

unit-tests:
	UNIT_TESTS=1 $(MAKE) -e

again: clean all

# Flash the STM32F4
burn:
	st-flash write $(PREFIX)/$(PROJ_NAME).bin 0x8000000

# Create tags
ctags:
	ctags -R .

$(PREFIX)/%.o : %.S
	$(CC) -MD -c $(CFLAGS) $< -o $@ 
	@cp $(PREFIX)/$*.d $(PREFIX)/$*.P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
			-e '/^$$/ d' -e 's/$$/ :/' < $(PREFIX)/$*.d >> $(PREFIX)/$*.P; \
		rm -f $(PREFIX)/$*.d

-include $(addprefix $(PREFIX)/, $(ASM_SRCS:.S=.P))

$(PREFIX)/%.o : %.c
	$(CC) -MD -c $(CFLAGS) $< -o $@ 
	@cp $(PREFIX)/$*.d $(PREFIX)/$*.P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
			-e '/^$$/ d' -e 's/$$/ :/' < $(PREFIX)/$*.d >> $(PREFIX)/$*.P; \
		rm -f $(PREFIX)/$*.d

-include $(addprefix $(PREFIX)/, $(SRCS:.c=.P))

proj: 	$(PREFIX)/$(PROJ_NAME).elf

$(PREFIX):
	mkdir -p $(PREFIX)

$(PREFIX)/$(PROJ_NAME).elf: $(OBJS) 
	$(LD) $^ -o $@ $(LFLAGS) -T $(LINK_SCRIPT)
	$(OBJCOPY) -O ihex $(PREFIX)/$(PROJ_NAME).elf $(PREFIX)/$(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(PREFIX)/$(PROJ_NAME).elf $(PREFIX)/$(PROJ_NAME).bin

clean:
	-rm -rf $(PREFIX)
