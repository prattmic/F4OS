# The goal here is to create usr_$(USR).o for the rest of the OS to link against

# /
SRCS += main.c

###################################################

OBJS = $(addprefix $(PREFIX)/, $(SRCS:.c=.o))

###################################################

all: $(PREFIX)/usr_$(USR).o

$(PREFIX)/usr_$(USR).o: $(OBJS)
	@echo "LD $(subst $(PREFIX)/,,$@)" && $(LD) -r $^ -o $@

include $(BASE)/tools/build.mk
