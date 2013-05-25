# The goal here is to create chip_$(CHIP).o for the rest of the OS to link against

# /
SRCS += clock.c

# dev/hw/
VPATH += dev/hw/
SRCS_$(CONFIG_HAVE_USART) += usart.c
SRCS_$(CONFIG_HAVE_LED) += led.c

SRCS += $(SRCS_y)

###################################################

OBJS = $(addprefix $(PREFIX)/, $(SRCS:.c=.o))

###################################################

all: $(PREFIX)/chip_$(CONFIG_CHIP).o

burn:
	./flash.sh $(PREFIX)/$(PROJ_NAME).bin

$(PREFIX)/chip_$(CONFIG_CHIP).o: $(OBJS)
	@echo "LD $(subst $(PREFIX)/,,$@)" && $(LD) -r $^ -o $@

include $(BASE)/tools/build.mk
