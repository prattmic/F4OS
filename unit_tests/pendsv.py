import gdb

def lookup_symbol(addr):
    message = gdb.execute("i symbol 0x%x" % addr, to_string=True)

    if message.split()[0] == "No":
        return message
    else:
        return message.split()[0]

def interrupted_task(big_stack=False):
    psp = gdb.parse_and_eval("$psp")
    
    if big_stack:
        stacked_pc = psp + 14*4
    else:
        stacked_pc = psp + 6*4

    pc = stacked_pc.cast(gdb.lookup_type("uint32_t").pointer()).dereference()

    function = lookup_symbol(pc)

    curr_task = gdb.parse_and_eval("k_curr_task")

    task_name = lookup_symbol(curr_task["task"]["fptr"].cast(gdb.lookup_type("uint32_t")))

    return (curr_task["task"], task_name, function)

def break_handler(event):
    if type(event) == gdb.StopEvent:
        return

    if event.breakpoints[0] == swap:
        interrupted = interrupted_task(big_stack=True)
    else:
        interrupted = interrupted_task()

    print "'%s()' interrupted while in '%s()'" % (interrupted[1], interrupted[2])

tick = gdb.Breakpoint("pendsv_handler")
swap = gdb.Breakpoint("context.c:160")
svc  = gdb.Breakpoint("svc_handler")

# Run forever
gdb.execute("set height 0")

# Initialize breakpoint handler
gdb.events.stop.connect(break_handler)

while True:
    gdb.execute("continue")
