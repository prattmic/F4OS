# The goal here is to create usr_$(USR).o for the rest of the OS to link against

# /
SRCS += main.c

###################################################

OBJS = $(addprefix $(PREFIX)/, $(SRCS:.c=.o))

###################################################

all: $(PREFIX)/usr_$(USR).o

$(PREFIX)/%.o : %.c
	@echo "CC $(subst $(BASE)/,,$(CURDIR))/$<" && $(CC) -MD -c $(CFLAGS) $< -o $@ 
	@cp $(PREFIX)/$*.d $(PREFIX)/$*.P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
			-e '/^$$/ d' -e 's/$$/ :/' < $(PREFIX)/$*.d >> $(PREFIX)/$*.P; \
		rm -f $(PREFIX)/$*.d

-include $(addprefix $(PREFIX)/, $(SRCS:.c=.P))

$(PREFIX)/usr_$(USR).o: $(OBJS)
	@echo "LD $(subst $(PREFIX)/,,$@)" && $(LD) -r $^ -o $@
