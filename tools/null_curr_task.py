import gdb

cont = True

def break_handler(event):
    curr_task = gdb.parse_and_eval("curr_task")

    if curr_task == 0:
        cont = False
        print "curr_task == NULL"

watch = gdb.Breakpoint("curr_task", type=gdb.BP_WATCHPOINT)

# Run forever
gdb.execute("set height 0")

# Initialize breakpoint handler
gdb.events.stop.connect(break_handler)

while cont:
    gdb.execute("continue")
