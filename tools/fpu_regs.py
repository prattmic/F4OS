import gdb

# See ARMv7-M Architecture Reference Manual
# Section C1.6 for details on the debug registers

class FPU_Regs(gdb.Command):
    """Print ARMv7-M FPU registers."""

    def __init__(self):
        super(FPU_Regs, self).__init__("info fpu-regs", gdb.COMMAND_DATA, gdb.COMPLETE_NONE)

    def invoke(self, arg, from_tty):
        if not arg:
            #s0-s31
            for n in range(32):
                self.print_reg(n)
            self.print_reg(0, fpscr=True)
        else:
            for a in arg.split():
                if a == "fpscr":
                    self.print_reg(0, fpscr=True)
                elif a[0] == 's' and int(a[1:]) >= 0 and int(a[1:]) <= 31:
                    self.print_reg(int(a[1:]))
                else:
                    print "Invalid register '%s'" % a

    def print_reg(self, num, fpscr=False):
        gdb.execute("set $data_reg = 0xE000EDF8")

        if fpscr:
            # Set debug core register selection register
            gdb.execute("set *0xE000EDF4 = 0x%x" % (0x21))
            # Grab result from debug core register data register
            reg = gdb.parse_and_eval("*((unsigned int *)$data_reg)")
            print "{0:<15}{1:#x}\t{1:d}".format("fpscr", int(reg))
        else:
            # Set debug core register selection register
            gdb.execute("set *0xE000EDF4 = 0x%x" % (0x40 + num))
            # Grab result from debug core register data register
            reg = gdb.parse_and_eval("*((float *)$data_reg)")
            print "s{0:<14}{1:f}\t(raw {2})".format(num, float(reg), reg.cast(gdb.lookup_type("void").pointer()))


FPU_Regs()
