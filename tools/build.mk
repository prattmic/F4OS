# Primary rules for building source files

# For nonrecursive builds, obj_prefix will be empty,
# so the normal prefix will be used
obj_prefix ?= $(PREFIX)/

$(obj_prefix)%.o : %.S $(BASE)/include/config/autoconf.h
	$(VERBOSE)echo "CC $<" && $(CC) -MD -c $(CFLAGS) $< -o $@

$(obj_prefix)%.o : %.c $(BASE)/include/config/autoconf.h
	$(VERBOSE)echo "CC $<" && $(CC) -MD -c $(CFLAGS) $< -o $@

-include $(addprefix $(obj_prefix), $(SRCS:.S=.d))
-include $(addprefix $(obj_prefix), $(SRCS:.c=.d))

.FORCE:
