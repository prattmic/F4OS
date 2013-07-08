# all the files will be generated with this name (main.elf, main.bin, main.hex, etc)
PROJ_NAME=f4os

# Project base
BASE := $(CURDIR)
# Output directory
PREFIX := $(BASE)/out

# Userspace program to build (folder in usr/)
USR ?= shell

-include $(BASE)/.config

# The quotes from Kconfig are annoying
CONFIG_ARCH := $(shell echo $(CONFIG_ARCH))
CONFIG_CHIP := $(shell echo $(CONFIG_CHIP))

LINK_SCRIPT = arch/$(CONFIG_ARCH)/chip/$(CONFIG_CHIP)/link.ld

CROSS_COMPILE ?= arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy

CFLAGS += -g3 -Wall --std=gnu99 -include $(BASE)/include/config/autoconf.h -isystem $(PREFIX)/include/
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork -Xassembler -mimplicit-it=thumb
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16 -nostdlib -ffreestanding
CFLAGS += -Wdouble-promotion -fsingle-precision-constant -fshort-double

#CFLAGS += -save-temps --verbose -Xlinker --verbose

LFLAGS=

KCONFIG_DIR = $(BASE)/tools/kconfig-frontends/frontends/

# Command verbosity
# Unless V=1, surpress output with @
ifneq ($(V), 1)
	VERBOSE:=@
endif

# Pass variables to submake
export

###################################################

.PHONY: proj unoptimized ctags cscope .FORCE

all: CFLAGS += -O2
all: $(PREFIX) proj

unoptimized: CFLAGS += -O0
unoptimized: $(PREFIX) proj

again: clean all

# Flash the board
burn:
	$(MAKE) -C arch/$(CONFIG_ARCH)/chip/$(CONFIG_CHIP)/ burn

# Create tags
ctags:
	find $(BASE) -name "*.[chS]" -not -path "$(PREFIX)/*" -not -path "$(BASE)/tools/*" -print | xargs ctags

cscope:
	find $(BASE) -name "*.[chS]" -not -path "$(PREFIX)/*" -not -path "$(BASE)/tools/*" -print | xargs cscope -b -q -k

# defconfig rules and DEFCONFIGS list
include $(BASE)/configs/Makefile.in
-include $(BASE)/include/config/auto.conf.cmd

$(KCONFIG_DIR)/conf/conf:
	env - PATH=$(PATH) $(BASE)/tools/build_kconfig.sh $(BASE)

menuconfig: $(KCONFIG_DIR)/conf/conf
	$(KCONFIG_DIR)/nconf/nconf $(BASE)/Kconfig

$(BASE)/.config: ;

include/config/auto.conf $(BASE)/include/config/autoconf.h: $(BASE)/.config $(KCONFIG_DIR)/conf/conf $(deps_config)
	$(VERBOSE)if ! test -e $(BASE)/.config;	\
	then	\
		echo;	\
		echo "ERROR: F4OS not configured.";	\
		echo "Please run 'make menuconfig' or one of the 'make *_defconfig' before continuing.";	\
		echo;	\
		exit 1;	\
	fi;
	$(VERBOSE)mkdir -p $(BASE)/include/config
	$(VERBOSE)KCONFIG_AUTOHEADER=$(BASE)/include/config/autoconf.h $(KCONFIG_DIR)/conf/conf --silentoldconfig Kconfig

proj: 	$(PREFIX)/$(PROJ_NAME).elf

$(PREFIX):
	mkdir -p $(PREFIX)

# Build include/ directory in $(PREFIX)/ to provide all headers for the source files.
# Place the arch and chip headers in arch/ and arch/chip/, respectively.
# This way arch and chip-specific headers are included under those directories, rather
# than globally.
#
# Copy the files from each configured include directory, preserving all attributes,
# so that source files aren't rebuilt every time the headers "change" as this diretory
# is recreated.
#
# Rerun this rule at every build in order to pick up any new headers.
$(PREFIX)/include: $(PREFIX) .FORCE
	$(VERBOSE)echo "GEN	$@"
	$(VERBOSE)rm -rf $(PREFIX)/include/
	$(VERBOSE)mkdir -p $(PREFIX)/include/arch/chip/
	$(VERBOSE)cp -a $(BASE)/include/. $(PREFIX)/include/
	$(VERBOSE)cp -a $(BASE)/arch/$(CONFIG_ARCH)/include/. $(PREFIX)/include/arch/
	$(VERBOSE)cp -a $(BASE)/arch/$(CONFIG_ARCH)/chip/$(CONFIG_CHIP)/include/. $(PREFIX)/include/arch/chip/

$(PREFIX)/$(PROJ_NAME).o: $(BASE)/include/config/autoconf.h $(PREFIX)/include .FORCE
	$(MAKE) -f f4os.mk obj=$@

$(PREFIX)/$(PROJ_NAME).elf: $(PREFIX)/$(PROJ_NAME).o
	$(VERBOSE)echo "LD $(subst $(PREFIX)/,,$@)" && $(LD) $^ -o $@ $(LFLAGS) -T $(LINK_SCRIPT)
	$(VERBOSE)echo "OBJCOPY $(PROJ_NAME).hex" && $(OBJCOPY) -O ihex $(PREFIX)/$(PROJ_NAME).elf $(PREFIX)/$(PROJ_NAME).hex
	$(VERBOSE)echo "OBJCOPY $(PROJ_NAME).bin" && $(OBJCOPY) -O binary $(PREFIX)/$(PROJ_NAME).elf $(PREFIX)/$(PROJ_NAME).bin

clean:
	-rm -rf $(PREFIX)

distclean: clean
	-rm -rf .config
	-rm -rf $(BASE)/include/config/

.FORCE:

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
	@echo	'	Specify toolchain prefix with CROSS_COMPILE environmental variable'
	@echo	'	menuconfig  - run nconf for selecting configuration'
	@echo	''
	@echo	'Defconfigs:'
	@$(if $(DEFCONFIGS), \
		$(foreach d, $(DEFCONFIGS), \
		printf "	%-30s - configure for %s\\n" $(d) $(subst _defconfig,,$(d));))
	@echo	''
	@echo	'Misc:'
	@echo	'	burn        - burn to configured board'
	@echo	'	ctags       - generate ctags file'
	@echo	'	cscope      - generate cscope file'
	@echo	'	V=1         - Verbose.  Print build commands.'
