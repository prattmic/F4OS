# Primary rules for building source files

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
