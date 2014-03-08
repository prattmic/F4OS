SRCS += chip.c
SRCS += clocks.c
SRCS += interrupts.c
SRCS += timer.c

SRCS_$(CONFIG_HAVE_USART) += usart.c

binary: $(PREFIX)/MLO

$(PREFIX)/MLO: $(PREFIX)/$(PROJ_NAME).bin
	$(call print_command,"MKIMAGE",$(call relative_path,$@))
	$(VERBOSE)mkimage -T omapimage -a 402f0400 -d $< $@

include $(BASE)/tools/submake.mk
