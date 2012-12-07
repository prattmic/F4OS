# The goal here is to create chip_$(CHIP).o for the rest of the OS to link against

# /
SRCS += clock.c board.c

# dev/hw/
VPATH += dev/hw/
SRCS += gpio.c usart.c

###################################################

OBJS = $(addprefix $(PREFIX)/, $(SRCS:.c=.o))

###################################################

all: $(PREFIX)/chip_$(CHIP).o

$(PREFIX)/%.o : %.c
	$(CC) -MD -c $(CFLAGS) $< -o $@ 
	@cp $(PREFIX)/$*.d $(PREFIX)/$*.P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
			-e '/^$$/ d' -e 's/$$/ :/' < $(PREFIX)/$*.d >> $(PREFIX)/$*.P; \
		rm -f $(PREFIX)/$*.d

-include $(addprefix $(PREFIX)/, $(SRCS:.c=.P))

$(PREFIX)/chip_$(CHIP).o: $(OBJS)
	$(LD) -r $^ -o $@
