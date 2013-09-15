# Primary rules for building source files

include $(BASE)/tools/common.mk

# For nonrecursive builds, obj_prefix will be empty,
# so the normal prefix will be used
obj_prefix ?= $(PREFIX)/

$(obj_prefix)%.o : %.S $(KCONFIG_HEADER)
	$(call print_command,"CC","$(RELATIVE_CURDIR)/$<")
	$(VERBOSE)$(CC) -MD -c $(CFLAGS) $< -o $@

$(obj_prefix)%.o : %.c $(KCONFIG_HEADER)
	$(call print_command,"CC","$(RELATIVE_CURDIR)/$<")
	$(VERBOSE)$(CC) -MD -c $(CFLAGS) $< -o $@

-include $(addprefix $(obj_prefix), $(SRCS:.S=.d))
-include $(addprefix $(obj_prefix), $(SRCS:.c=.d))

.FORCE:
