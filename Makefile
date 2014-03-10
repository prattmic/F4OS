# all the files will be generated with this name (main.elf, main.bin, main.hex, etc)
PROJ_NAME=f4os

# Use greadlink if on Mac OS X
READLINK := readlink
UNAME := $(shell uname -s)
ifeq ($(UNAME), Darwin)
READLINK := greadlink
endif

# Project base
# Use readlink to ensure path is canonical
BASE := $(shell $(READLINK) -f $(CURDIR))
# Output directory
PREFIX := $(BASE)/out

# Userspace program to build (folder in usr/)
USR ?= shell

# Folder to look for/build kconfig in
KCONFIG_CUSTOM_DIR := $(BASE)/tools/kconfig-frontends

# (n)conf can either be provided by the environment, PATH, or
# our custom installation
CONF ?= $(shell \
	which conf > /dev/null;	\
	if [ $$? -eq 0 ];	\
	then echo `which conf`;	\
	else echo $(KCONFIG_CUSTOM_DIR)/frontends/conf/conf;	\
	fi)

NCONF ?= $(shell \
	which nconf > /dev/null;	\
	if [ $$? -eq 0 ];	\
	then echo `which nconf`;	\
	else echo $(KCONFIG_CUSTOM_DIR)/frontends/nconf/nconf;	\
	fi)

# Config header for source files
KCONFIG_HEADER := $(BASE)/include/config/autoconf.h
# Config defines for Make
# Do not depend on this! It has a dummy rule to prevent make from trying
# to make it on include.  Depend on KCONFIG_HEADER, its rule will build this.
KCONFIG_MAKE_DEFS := $(BASE)/include/config/auto.conf

# Include configuration
# Ignores errors because we need to support make *_defconfig or menuconfig
# to create the configuration.  However, if the file doesn't exist, do not
# allow building
-include $(KCONFIG_MAKE_DEFS)
ifeq ($(wildcard $(KCONFIG_MAKE_DEFS)),)
	# Disallow build because OS not configured
	DISALLOW_BUILD = F4OS not configured
endif

# Don't build if applying a defconfig
# This way `make -j4 defconfig all` will only do the config,
# and not try to build.
ifneq ($(findstring defconfig,$(MAKECMDGOALS)),)
	# Disallow build because this is a defconfig run
	DISALLOW_BUILD = Parallel configure and build not supported
endif

# The quotes from Kconfig are annoying
CONFIG_ARCH := $(shell echo $(CONFIG_ARCH))
CONFIG_CHIP := $(shell echo $(CONFIG_CHIP))
CONFIG_DEVICE_TREE := $(shell echo $(CONFIG_DEVICE_TREE))

ifeq ($(wildcard $(CONFIG_DEVICE_TREE)),)
	# Disallow build because not device tree specified
	DISALLOW_BUILD = Device tree source must be configured (CONFIG_DEVICE_TREE)
endif

# Include all arch-specific configs
# CROSS_COMPILE, CFLAGS, etc
-include $(BASE)/arch/$(CONFIG_ARCH)/config.mk

LINK_SCRIPT = $(BASE)/arch/$(CONFIG_ARCH)/chip/$(CONFIG_CHIP)/link.ld

# Command verbosity
# Unless V=1, surpress output with @
ifneq ($(V), 1)
	VERBOSE:=@
endif

CC = $(CROSS_COMPILE)gcc
CPP = $(CROSS_COMPILE)cpp
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy

# Establish system includes directory, auto-include config
INCLUDE_FLAGS := -isystem $(PREFIX)/include/ -include $(BASE)/include/config/autoconf.h

CFLAGS += --std=gnu99	# Use C99 with GNU extensions
CFLAGS += -Wall			# Enable "all" warnings
CFLAGS += -g3			# Lots of debugging info
CFLAGS += -nostdlib		# Do not use standard system libraries
CFLAGS += -ffreestanding# Do not assume any standard library exists
CFLAGS += $(INCLUDE_FLAGS)

LFLAGS +=

CPPFLAGS := -P $(INCLUDE_FLAGS)

# Surpress Make directory printing unless verbose
ifneq ($(VERBOSE),)
MAKEFLAGS += --no-print-directory
else
MAKEFLAGS += --print-directory
endif

# Pass variables to submake
export

include $(BASE)/tools/common.mk

###################################################

.PHONY: proj unoptimized ctags cscope .FORCE

ifeq ($(DISALLOW_BUILD),)
all: CFLAGS += -O2
all: $(PREFIX) proj

unoptimized: CFLAGS += -O0
unoptimized: $(PREFIX) proj

proj: $(PREFIX)/$(PROJ_NAME).elf $(PREFIX)/$(PROJ_NAME).bin
else
all unoptimized proj:
	$(error $(DISALLOW_BUILD))
endif

# Flash the board
burn: $(KCONFIG_HEADER)
	$(VERBOSE)$(MAKE) -C arch/$(CONFIG_ARCH)/chip/$(CONFIG_CHIP)/ burn

# Create tags
ctags:
	$(VERBOSE)find $(BASE) -name "*.[chS]" -not -path "$(PREFIX)/*" -not -path "$(BASE)/tools/*" -print | xargs ctags

cscope:
	$(VERBOSE)find $(BASE) -name "*.[chS]" -not -path "$(PREFIX)/*" -not -path "$(BASE)/tools/*" -print | xargs cscope -b -q -k

# defconfig rules and DEFCONFIGS list
include $(BASE)/configs/Makefile.in

# Kconfig file dependencies
-include $(KCONFIG_MAKE_DEFS).cmd

# Clear the build variables, so that kconfig-frontends doesn't try to build
# for our target chip.
$(CONF) $(NCONF):
	$(VERBOSE)CC= OBJCOPY= LD= CFLAGS= LDFLAGS= CPPFLAGS= MAKEFLAGS= \
		$(BASE)/tools/build_kconfig.sh $(KCONFIG_CUSTOM_DIR)

menuconfig: $(NCONF)
	$(NCONF) $(BASE)/Kconfig

# Dummy rule to tell the user to run configuration
$(BASE)/.config:
	$(VERBOSE)echo "\nERROR: F4OS not configured.";
	$(VERBOSE)echo "Please run 'make menuconfig' or one of the 'make *_defconfig' before continuing.\n";
	$(VERBOSE)exit 1;

ifeq ($(findstring defconfig,$(MAKECMDGOALS)),)
$(KCONFIG_MAKE_DEFS) $(KCONFIG_HEADER): $(CONF) $(BASE)/.config $(deps_config)
	$(call print_command,"GEN",$(call relative_path,$@))
	$(VERBOSE)mkdir -p $(BASE)/include/config
	$(VERBOSE)KCONFIG_AUTOHEADER=$(KCONFIG_HEADER) KCONFIG_AUTOCONFIG=$(KCONFIG_MAKE_DEFS) \
		$(CONF) --silentoldconfig Kconfig
else
# If one of the goals is a defconfig, don't bother trying to generate the config
$(KCONFIG_MAKE_DEFS): ;
endif

$(PREFIX):
	$(VERBOSE)mkdir -p $(PREFIX)

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
$(PREFIX)/include: $(PREFIX) $(KCONFIG_HEADER) .FORCE
	$(call print_command,"GEN",$(call relative_path,$@)/)
	$(VERBOSE)rm -rf $(PREFIX)/include/
	$(VERBOSE)mkdir -p $(PREFIX)/include/arch/chip/
	$(VERBOSE)cp -a $(BASE)/include/. $(PREFIX)/include/
	$(VERBOSE)cp -a $(BASE)/arch/$(CONFIG_ARCH)/include/. $(PREFIX)/include/arch/
	$(VERBOSE)cp -a $(BASE)/arch/$(CONFIG_ARCH)/chip/$(CONFIG_CHIP)/include/. $(PREFIX)/include/arch/chip/

$(PREFIX)/device_tree.dtb: $(BASE)/$(CONFIG_DEVICE_TREE)
	$(call print_command,"DTC",$(call relative_path,$@))
	$(VERBOSE)dtc -I dts -O dtb -d $(PREFIX)/device_tree.d -o $@ $<

# Include device tree dependencies
-include $(PREFIX)/device_tree.d

# Generate an assembly file to include the dtb in the correct section,
# and build that into an object file.
$(PREFIX)/device_tree.o: $(PREFIX)/device_tree.dtb
	$(call print_command,"GEN",$(call relative_path,$(PREFIX)/device_tree.s))
	$(VERBOSE)echo ".section .dtb, \"wa\"\n.incbin \"$<\"" > $(PREFIX)/device_tree.s
	$(call print_command,"CC",$(call relative_path,$@))
	$(VERBOSE)$(CC) $(CFLAGS) -o $@ -c $(PREFIX)/device_tree.s

$(PREFIX)/$(PROJ_NAME).o: $(KCONFIG_HEADER) $(PREFIX)/include .FORCE
	$(call print_command,"MAKE",$(call relative_path,$@))
	$(VERBOSE)$(MAKE) -f f4os.mk obj=$@

$(PREFIX)/$(PROJ_NAME).elf: $(PREFIX)/link.ld $(PREFIX)/$(PROJ_NAME).o $(PREFIX)/device_tree.o
	$(call print_command,"LD",$(call relative_path,$@))
	$(VERBOSE)$(CC) $(filter-out $<,$^) -o $@ $(CFLAGS) -T $< $(patsubst %,-Xlinker %,$(LFLAGS))

%.hex: %.elf
	$(call print_command,"OBJCOPY",$(call relative_path,$@))
	$(VERBOSE)$(OBJCOPY) -O ihex $< $@

%.bin: %.elf
	$(call print_command,"OBJCOPY",$(call relative_path,$@))
	$(VERBOSE)$(OBJCOPY) -O binary $< $@

# Preprocess the linker script
$(PREFIX)/link.ld : $(LINK_SCRIPT)
	$(call print_command,"CPP",$(call relative_path,$<))
	$(VERBOSE)$(CPP) -MD -MT $@ $(CPPFLAGS) $< -o $@

# Include linker script dependencies
-include $(PREFIX)/link.d

clean:
	$(VERBOSE)-rm -rf $(PREFIX)

distclean: clean
	$(VERBOSE)-rm -rf .config
	$(VERBOSE)-rm -rf $(BASE)/include/config/

# Parallel safe make again
again:
	$(call print_command,"MAKE","clean")
	$(VERBOSE)$(MAKE) clean
	$(call print_command,"MAKE","all")
	$(VERBOSE)$(MAKE) all

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
