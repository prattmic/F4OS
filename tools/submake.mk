# Error if obj wasn't passed in,
# and there isn't a custom goal declared
ifndef MAKECMDGOALS
ifndef obj
$(error submake obj not specified)
endif
endif

include $(BASE)/tools/common.mk

.DEFAULT_GOAL := $(obj)

obj_prefix := $(basename $(obj))_

# Only build "yes" conditional sources
SRCS += $(SRCS_y)

OBJS := $(patsubst %.c, %.o, $(filter %.c, $(SRCS)))	# Add C files
OBJS += $(patsubst %.S, %.o, $(filter %.S, $(SRCS)))	# Add ASM files
OBJS := $(addprefix $(obj_prefix), $(OBJS))				# Add output prefix

# Only build "yes" conditional directories
DIRS += $(DIRS_y)

# Convert DIR into matching .o file
# First removes slashes, then adds
# prefix, which includes the current obj
# name, then adds .o
dir_obj = $(addsuffix .o,\
$(addprefix $(obj_prefix),\
$(subst /,,$(1))))

# Create a compilation rule for directory
# $(1) = Directory
# $(2) = Directory obj to create
define define_compile_rules
$(2): .FORCE
	$(call print_command,"MAKE",$(call relative_path,$(1))/)
	$(VERBOSE)+$(MAKE) -C $(1) obj=$(2)
endef

# Create rule for each directory
$(foreach DIR, $(DIRS), $(eval $(call define_compile_rules,$(DIR),$(call dir_obj, $(DIR)))))

# All of the directory object files
DIR_OBJS := $(foreach DIR, $(DIRS), $(call dir_obj, $(DIR)))

ifeq ($(strip $(OBJS)$(DIR_OBJS)),)
# Nothing to build!  Generate an empty object
$(obj):
	$(call print_command,"LD",$(call relative_path,$@))
	$(VERBOSE)echo "" | $(CC) -c -xc $(CFLAGS) -o $@ -
else
# Actual directory obj rule
# Links all source and subdirectory objects
$(obj): $(OBJS) $(DIR_OBJS)
	$(call print_command,"LD",$(call relative_path,$@))
	$(VERBOSE)$(LD) -r $^ -o $@
endif

include $(BASE)/tools/build.mk
