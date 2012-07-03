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

break_acquire = Acquire("*0x08001678")
break_fail = Fail("*0x08001676")
break_succeed = Succeed("*0x0800167e")

gdb.execute("continue")
