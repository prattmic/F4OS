import gdb

class Acquire(gdb.Breakpoint):
    def stop(self):
        print "Attempting acquire... ",
        return False

class Fail(gdb.Breakpoint):
    def stop(self):
        print "Failed"
        return False

class Succeed(gdb.Breakpoint):
    def stop(self):
        print "Succeeded"
        return False

break_acquire = Acquire("*0x08001690")
break_fail = Fail("*0x080016a4")
break_succeed = Succeed("*0x080016a2")

gdb.execute("continue")
