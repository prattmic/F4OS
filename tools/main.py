import gdb

# Break on main
break_main = gdb.Breakpoint("main")
gdb.execute("continue")
break_main.delete()
