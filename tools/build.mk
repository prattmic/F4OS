# Primary rules for building source files

# For nonrecursive builds, obj_prefix will be empty,
# so the normal prefix will be used
obj_prefix ?= $(PREFIX)/

$(obj_prefix)%.o : %.S $(BASE)/include/config/autoconf.h
	@echo "CC $<" && $(CC) -MD -c $(CFLAGS) $< -o $@
	$(generate_header_depends)

$(obj_prefix)%.o : %.c $(BASE)/include/config/autoconf.h
	@echo "CC $<" && $(CC) -MD -c $(CFLAGS) $< -o $@
	$(generate_header_depends)

-include $(addprefix $(obj_prefix), $(SRCS:.S=.P))
-include $(addprefix $(obj_prefix), $(SRCS:.c=.P))

define generate_header_depends
	@cp $(obj_prefix)$*.d $(obj_prefix)$*.P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
			-e '/^$$/ d' -e 's/$$/ :/' < $(obj_prefix)$*.d >> $(obj_prefix)$*.P; \
		rm -f $(obj_prefix)$*.d
endef

.FORCE:
