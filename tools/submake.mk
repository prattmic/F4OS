# Error if obj wasn't passed in,
# and there isn't a custom goal declared
ifndef MAKECMDGOALS
ifndef obj
$(error submake obj not specified)
endif
endif

# Only build "yes" conditional sources
SRCS += $(SRCS_y)

OBJS := $(patsubst %.c, %.o, $(filter %.c, $(SRCS)))	# Add C files
OBJS += $(patsubst %.S, %.o, $(filter %.S, $(SRCS)))	# Add ASM files
OBJS := $(addprefix $(PREFIX)/, $(OBJS))				# Add output prefix

$(obj): $(OBJS)
	@echo "LD $(subst $(PREFIX)/,,$@)" && $(LD) -r $^ -o $@

.DEFAULT_GOAL := $(obj)

include $(BASE)/tools/build.mk
