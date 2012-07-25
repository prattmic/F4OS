import gdb
import sys

def lookup_symbol(addr):
    message = gdb.execute("i symbol 0x%x" % addr, to_string=True)

    if message.split()[0] == "No":
        return message
    else:
        return message.split()[0]

def restore_task():
    psp = gdb.parse_and_eval("$psp")
    
    stacked_pc = psp - 84

    pc = stacked_pc.cast(gdb.lookup_type("uint32_t").pointer()).dereference()

    function = lookup_symbol(pc)

    return (pc, function)

def break_handler(event):
    if type(event) == gdb.StopEvent:
        sys.exit(1)

    restore = restore_task()

    print "Restoring pc 0x%x (%s())" % (restore[0], restore[1])

tick = gdb.Breakpoint("memasm.S:163")

# Run forever
gdb.execute("set height 0")

# Initialize breakpoint handler
gdb.events.stop.connect(break_handler)

while True:
    gdb.execute("continue")
