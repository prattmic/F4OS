# put your *.o targets here, make should handle the rest!

SRCS = bootasm.S bootmain.c usermode.c mem.S mem.c
LINK_SCRIPT = kernel.ld

# all the files will be generated with this name (main.elf, main.bin, main.hex, etc)

PROJ_NAME=os

# Put your stlink folder here so make burn will work.

STLINK=/mnt/share/Programming/embedded/stm32/stlink

# that's it, no need to change anything below this line!

###################################################

CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy

CFLAGS  = -g -Wall -T$(LINK_SCRIPT) 
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16 -nostdlib
#CFLAGS += -save-temps --verbose -Xlinker --verbose

###################################################

ROOT=$(shell pwd)

OBJS = $(SRCS:.c=.o)

###################################################

.PHONY: lib proj

all: lib proj

again: clean all

# Flash the STM32F4
burn:
	$(STLINK)/flash/st-flash write $(PROJ_NAME).bin 0x8000000

# Create tags; assumes ctags exists
ctags:
	ctags -R .

proj: 	$(PROJ_NAME).elf

$(PROJ_NAME).elf: $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@
	$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin

clean:
	rm -f *.o *.i *.s
	rm -f $(PROJ_NAME).elf
	rm -f $(PROJ_NAME).hex
	rm -f $(PROJ_NAME).bin
