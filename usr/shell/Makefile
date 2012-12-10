# The goal here is to create usr_$(USR).o for the rest of the OS to link against

# /
SRCS += main.c shell.c ipctest.c top.c uname.c rd_test.c

SRCS_$(CONFIG_HAVE_LED) += blink.c
SRCS_$(CONFIG_HAVE_I2C) += ghetto_gyro.c
SRCS_$(CONFIG_HAVE_SPI) += accel.c lowpass.c

SRCS += $(SRCS_y)

# Date and rev for uname
DATE := "$(shell date -u)"
REV := $(shell git rev-list HEAD | wc -l | tr -d ' ')
CFLAGS += -D BUILD_TIME='$(DATE)' -D BUILD_REV=$(REV)

###################################################

OBJS = $(addprefix $(PREFIX)/, $(SRCS:.c=.o))

###################################################

all: $(PREFIX)/usr_$(USR).o

$(PREFIX)/%.o : %.c
	$(CC) -MD -c $(CFLAGS) $< -o $@ 
	@cp $(PREFIX)/$*.d $(PREFIX)/$*.P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
			-e '/^$$/ d' -e 's/$$/ :/' < $(PREFIX)/$*.d >> $(PREFIX)/$*.P; \
		rm -f $(PREFIX)/$*.d

-include $(addprefix $(PREFIX)/, $(SRCS:.c=.P))

$(PREFIX)/usr_$(USR).o: $(OBJS)
	$(LD) -r $^ -o $@
