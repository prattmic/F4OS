# Build symbol table into OS image
#
# kernel/symbol_table_array.c defines a weak, empty symbol table struct.
# At build time, this needs to be replaced by by a version containing the
# actual symbols in the image.  Each symbol contains the function name
# and address, sorted by address.
#
# From $(PROJ_NAME).o, an ELF is linked, containing the empty symbol table.
# From this, the symbol table is generated and linked into a new ELF.  This
# ELF contains a symbol table of the correct size, but its addition may have
# shifted other symbols, invalidating their address.  Thus, the symbol table
# is regenerated and linked into the fine ELF.

# All of the ELF files need the linker script and project object file
ELF_DEPS := $(PREFIX)/link.ld $(PREFIX)/$(PROJ_NAME).o

ifdef CONFIG_BUILD_SYMBOL_TABLE
ELF_OBJS := $(PREFIX)/$(PROJ_NAME)1.intermediate.elf \
	$(PREFIX)/$(PROJ_NAME)2.intermediate.elf \
	$(PREFIX)/$(PROJ_NAME).final.elf

# Include appropriate symbol table in ELF files
$(PREFIX)/$(PROJ_NAME)1.intermediate.elf: $(ELF_DEPS)
$(PREFIX)/$(PROJ_NAME)2.intermediate.elf: $(ELF_DEPS) $(PREFIX)/$(PROJ_NAME)1.symtab.o
$(PREFIX)/$(PROJ_NAME).final.elf: $(ELF_DEPS) $(PREFIX)/$(PROJ_NAME)2.symtab.o

SYMTAB_OBJS := $(PREFIX)/$(PROJ_NAME)1.symtab.o $(PREFIX)/$(PROJ_NAME)2.symtab.o

$(PREFIX)/$(PROJ_NAME)1.symtab.o: $(PREFIX)/$(PROJ_NAME)1.intermediate.elf
$(PREFIX)/$(PROJ_NAME)2.symtab.o: $(PREFIX)/$(PROJ_NAME)2.intermediate.elf
else
ELF_OBJS := $(PREFIX)/$(PROJ_NAME).final.elf

$(PREFIX)/$(PROJ_NAME).final.elf: $(ELF_DEPS)
endif

# Perform linking using linker script (first prereq) on object files (additional prereqs)
$(ELF_OBJS):
	$(call print_command,"LD",$(call relative_path,$@))
	$(VERBOSE)$(CC) $(filter-out $<,$^) -o $@ $(CFLAGS) -T $< $(patsubst %,-Xlinker %,$(LFLAGS))

# Generate symbol table object from ELF
$(SYMTAB_OBJS):
	$(call print_command,"CC",$(call relative_path,$@))
	$(VERBOSE)$(OBJDUMP) -t $^ | python $(BASE)/tools/symbol_table.py | $(CC) -o $@ $(CFLAGS) -c -x c -
