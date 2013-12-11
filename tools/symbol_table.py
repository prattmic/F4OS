"""
Generate symbol table C file from ELF

Dumps function names and addresses from input ELF file into symbol table
structure defined in include/kernel/symbol_table.h.

Symbol table is sorted in ascending order, by function address.

Output of `objdump -t file.elf` should be provided on stdin.
"""

import fileinput
import re

# Match the objdump symble table format (for ELF files)
# Documented in the objdump man page
objdump_symtab_pattern = """
    (?P<address>[0-9a-fA-F]+)   # Symbol address
    \s
    (?P<scope>.)                # Local (l), global (g), unique global (u), local and global (!)
    (?P<strength>.)             # Weak (w), strong ( )
    (?P<constructor>.)          # Constructor (C) or not ( )
    (?P<warning>.)              # Symbol is a warning (W) or not ( )
    (?P<indirect>.)             # Indirect reference (I), to be reevaluated (i), or normal ( )
    (?P<debug>.)                # Debugging (d), dynamic (D), or normal ( )
    (?P<type>.)                 # Function (F), file (f), object (o), normal ( )
    \s
    (?P<section>.+)             # Symbol section
    \s
    (?P<number>[0-9a-fA-F]+)    # Alignment or size
    \s
    (?P<name>.+)                # Symbol name
"""

objdump_symtab = re.compile(objdump_symtab_pattern, re.VERBOSE)

if __name__ == '__main__':
    functions = []

    for line in fileinput.input():
        m = re.match(objdump_symtab, line)
        if m:
            if m.group('type') == 'F':
                name = m.group('name')
                address = int(m.group('address'), 16)
                functions.append((address, name))

    # Sort by address
    functions = sorted(functions, key=lambda func: func[0])

    print '#include <kernel/symbol_table.h>'

    print 'const struct symbol_table symbol_table[] = {'
    for func in functions:
        print '\t{ .addr = %#x, .name = "%s" },' % (func[0], func[1])
    print '};'

    # Total
    print 'const unsigned int symbol_table_length = %d;' % len(functions)
