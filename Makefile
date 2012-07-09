SRCS = bootmain.c mem.c mpu.c buddy.c usart.c interrupt.c usermode.c systick.c context.c task.c semaphore.c spi.c shell.c
SRCS += string.c math.c
ASM_SRCS = bootasm.S memasm.S

LINK_SCRIPT = kernel.ld

# all the files will be generated with this name (main.elf, main.bin, main.hex, etc)

PROJ_NAME=os

# that's it, no need to change anything below this line!

###################################################

CC=arm-none-eabi-gcc
LD=arm-none-eabi-ld
OBJCOPY=arm-none-eabi-objcopy

CFLAGS  = -g3 -Wall --std=gnu99 -I./inc/ -I./lib/inc/
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork -Xassembler -mimplicit-it=thumb
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16 -nostdlib -ffreestanding
CFLAGS += -Wdouble-promotion -fsingle-precision-constant -fshort-double
CFLAGS += -O2 
#CFLAGS += -save-temps --verbose -Xlinker --verbose

LFLAGS=

VPATH = src/ lib/src/

###################################################

OBJS = $(SRCS:.c=.o)
OBJS += $(ASM_SRCS:.S=.o)

###################################################

.PHONY: proj generated_headers

all: proj

again: clean all

# Flash the STM32F4
burn:
	st-flash write $(PROJ_NAME).bin 0x8000000

# Create tags
ctags:
	ctags -R .

%.o : %.S | generated_headers
	$(CC) -MD -c $(CFLAGS) $< -o $@ 
	@cp $*.d $*.P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
			-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
		rm -f $*.d

-include $(ASM_SRCS:.S=.P)

%.o : %.c | generated_headers
	$(CC) -MD -c $(CFLAGS) $< -o $@ 
	@cp $*.d $*.P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
			-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
		rm -f $*.d

-include $(SRCS:.c=.P)

generated_headers: builddefs.h

builddefs.h:
	$(SHELL) ./builddefs.sh

proj: 	$(PROJ_NAME).elf

$(PROJ_NAME).elf: $(OBJS) 
	$(LD) $^ -o $@ $(LFLAGS) -T $(LINK_SCRIPT)
	$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin

clean:
	-rm -f *.o *.i *.s *.P
	-rm -f $(PROJ_NAME).elf
	-rm -f $(PROJ_NAME).hex
	-rm -f $(PROJ_NAME).bin
