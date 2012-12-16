# all the files will be generated with this name (main.elf, main.bin, main.hex, etc)

PROJ_NAME=f4os
BASE := $(CURDIR)
PREFIX := $(BASE)/out

USR ?= shell

-include $(BASE)/.config

# The quotes from Kconfig are annoying
CONFIG_CHIP := $(shell echo $(CONFIG_CHIP))

LINK_SCRIPT = chip/$(CONFIG_CHIP)/link.ld

###################################################

# boot/
VPATH += boot/
SRCS += boot_main.c cortex_m.c
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
SRCS += kernel_task.c sched_end.c sched_interrupts.c sched_new.c sched_start.c sched_switch.c sched_tasks.c
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

# that's it, no need to change anything below this line!

###################################################

CC=arm-none-eabi-gcc
LD=arm-none-eabi-ld
OBJCOPY=arm-none-eabi-objcopy

CFLAGS += -g3 -Wall --std=gnu99 -isystem $(BASE)/include/ -isystem $(BASE)/chip/$(CONFIG_CHIP)/include/ -include $(BASE)/include/config/autoconf.h
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork -Xassembler -mimplicit-it=thumb
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16 -nostdlib -ffreestanding
CFLAGS += -Wdouble-promotion -fsingle-precision-constant -fshort-double

#CFLAGS += -save-temps --verbose -Xlinker --verbose

LFLAGS=

KCONFIG_DIR = $(BASE)/tools/kconfig-frontends/frontends/

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

.PHONY: proj unoptimized ctags cscopse .FORCE

all: CFLAGS += -O2
all: $(PREFIX) proj

unoptimized: CFLAGS += -O0
unoptimized: $(PREFIX) proj

again: clean all

# Flash the board
burn:
	$(MAKE) -C chip/$(CONFIG_CHIP)/ burn

# Create tags
ctags:
	ctags -R .

cscope:
	find . -name "*.[chS]" -print | xargs cscope -b -q -k

# defconfigs
include $(BASE)/configs/Makefile.in
-include $(BASE)/include/config/auto.conf.cmd

$(KCONFIG_DIR)/conf/conf:
	env - PATH=$(PATH) $(BASE)/tools/build_kconfig.sh $(BASE)

menuconfig: $(KCONFIG_DIR)/conf/conf
	$(KCONFIG_DIR)/nconf/nconf $(BASE)/Kconfig

$(BASE)/.config: ;

include/config/auto.conf $(BASE)/include/config/autoconf.h: $(BASE)/.config $(KCONFIG_DIR)/conf/conf
	@if ! test -e $(BASE)/.config;	\
	then	\
		echo;	\
		echo "ERROR: F4OS not configured.";	\
		echo "Please run 'make menuconfig' or one of the 'make *_defconfig' before continuing.";	\
		echo;	\
		exit 1;	\
	fi;
	-@mkdir $(BASE)/include/config
	KCONFIG_AUTOHEADER=$(BASE)/include/config/autoconf.h $(KCONFIG_DIR)/conf/conf --silentoldconfig Kconfig

$(PREFIX)/chip_$(CONFIG_CHIP).o: $(BASE)/include/config/autoconf.h .FORCE
	$(MAKE) -C chip/$(CONFIG_CHIP)/

$(PREFIX)/usr_$(USR).o: $(BASE)/include/config/autoconf.h .FORCE
	$(MAKE) -C usr/$(USR)/

$(PREFIX)/%.o : %.S $(BASE)/include/config/autoconf.h
	@echo "CC $<" && $(CC) -MD -c $(CFLAGS) $< -o $@ 
	@cp $(PREFIX)/$*.d $(PREFIX)/$*.P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
			-e '/^$$/ d' -e 's/$$/ :/' < $(PREFIX)/$*.d >> $(PREFIX)/$*.P; \
		rm -f $(PREFIX)/$*.d

-include $(addprefix $(PREFIX)/, $(ASM_SRCS:.S=.P))

$(PREFIX)/%.o : %.c $(BASE)/include/config/autoconf.h
	@echo "CC $<" && $(CC) -MD -c $(CFLAGS) $< -o $@ 
	@cp $(PREFIX)/$*.d $(PREFIX)/$*.P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
			-e '/^$$/ d' -e 's/$$/ :/' < $(PREFIX)/$*.d >> $(PREFIX)/$*.P; \
		rm -f $(PREFIX)/$*.d

-include $(addprefix $(PREFIX)/, $(SRCS:.c=.P))

proj: 	$(PREFIX)/$(PROJ_NAME).elf

$(PREFIX):
	mkdir -p $(PREFIX)

$(PREFIX)/$(PROJ_NAME).elf: $(PREFIX)/chip_$(CONFIG_CHIP).o $(PREFIX)/usr_$(USR).o $(OBJS) 
	@echo "LD $(subst $(PREFIX)/,,$@)" && $(LD) $^ -o $@ $(LFLAGS) -T $(LINK_SCRIPT)
	@echo "OBJCOPY $(PROJ_NAME).hex" && $(OBJCOPY) -O ihex $(PREFIX)/$(PROJ_NAME).elf $(PREFIX)/$(PROJ_NAME).hex
	@echo "OBJCOPY $(PROJ_NAME).bin" && $(OBJCOPY) -O binary $(PREFIX)/$(PROJ_NAME).elf $(PREFIX)/$(PROJ_NAME).bin

clean:
	-rm -rf $(PREFIX)

distclean: clean
	-rm -rf .config
	-rm -rf $(BASE)/include/config/

.FORCE:

DEFCONFIGS := $(wildcard $(BASE)/configs/*_defconfig)
DEFCONFIGS := $(notdir $(DEFCONFIGS))

help:
	@echo	'Cleaning:'
	@echo	'	clean       - removes all generated object files'
	@echo	'	distclean   - removes all generated files, including config'
	@echo	''
	@echo	'Targets:'
	@echo	'	all         - build all of OS'
	@echo	'	again       - clean + all'
	@echo	'	unoptimized - build all of OS at -O0'
	@echo	''
	@echo	'Configuration:'
	@echo	'	menuconfig  - run nconf for selecting configuration'
	@$(if $(DEFCONFIGS), \
		$(foreach d, $(DEFCONFIGS), \
		printf "	%-24s - configure for %s\\n" $(d) $(subst _defconfig,,$(d));))
	@echo	''
	@echo	'Misc:'
	@echo	'	burn        - burn to configured board'
	@echo	'	ctags       - generate ctags file'
	@echo	'	cscope      - generate cscope file'
