import gdb

swap = gdb.Breakpoint("swap_task")

gdb.execute("continue")
gdb.execute("continue")

i = 13
while i:
    gdb.execute("x/25x curr_task->next->next->task->stack_top")
    gdb.execute("continue")
    i -= 1
