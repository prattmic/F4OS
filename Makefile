CHIP ?= stm32f40x
USR ?= shell

LINK_SCRIPT = chip/$(CHIP)/link.ld

###################################################

# boot/
VPATH += boot/
SRCS += boot_main.c
ASM_SRCS += boot_asm.S

# dev/
VPATH += dev/
SRCS += resource.c shared_mem.c buf_stream.c

# dev/hw
VPATH += dev/hw/
SRCS += systick.c

# kernel/
VPATH += kernel/
SRCS += fault.c semaphore.c

# kernel/sched/
VPATH += kernel/sched/
SRCS += kernel_task.c sched_end.c sched_interrupts.c sched_new.c sched_start.c sched_switch.c
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
BASE := $(PWD)
PREFIX := $(PWD)/out

# that's it, no need to change anything below this line!

###################################################

CC=arm-none-eabi-gcc
LD=arm-none-eabi-ld
OBJCOPY=arm-none-eabi-objcopy

CFLAGS += -g3 -Wall --std=gnu99 -isystem $(BASE)/include/ -isystem $(BASE)/chip/$(CHIP)/include/
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork -Xassembler -mimplicit-it=thumb
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16 -nostdlib -ffreestanding
CFLAGS += -Wdouble-promotion -fsingle-precision-constant -fshort-double

#CFLAGS += -save-temps --verbose -Xlinker --verbose

LFLAGS=

###################################################

OBJS = $(addprefix $(PREFIX)/, $(SRCS:.c=.o))
OBJS += $(addprefix $(PREFIX)/, $(ASM_SRCS:.S=.o))

# Pass variables to submake
export
unexport VPATH
unexport SRCS
unexport ASM_SRCS
unexport OBJS

###################################################

.PHONY: proj unoptimized .FORCE

all: CFLAGS += -O2
all: $(PREFIX) proj

unoptimized: CFLAGS += -O0
unoptimized: $(PREFIX) proj

again: clean all

# Flash the board
burn:
	$(MAKE) -C chip/$(CHIP)/ burn

# Create tags
ctags:
	ctags -R .

cscope:
	find . -name "*.[chS]" -print | xargs cscope -b -q -k

$(PREFIX)/chip_$(CHIP).o: .FORCE
	$(MAKE) -C chip/$(CHIP)/

$(PREFIX)/usr_$(USR).o: .FORCE
	$(MAKE) -C usr/$(USR)/

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

$(PREFIX)/$(PROJ_NAME).elf: $(PREFIX)/chip_$(CHIP).o $(PREFIX)/usr_$(USR).o $(OBJS) 
	$(LD) $^ -o $@ $(LFLAGS) -T $(LINK_SCRIPT)
	$(OBJCOPY) -O ihex $(PREFIX)/$(PROJ_NAME).elf $(PREFIX)/$(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(PREFIX)/$(PROJ_NAME).elf $(PREFIX)/$(PROJ_NAME).bin

clean:
	-rm -rf $(PREFIX)

.FORCE:
