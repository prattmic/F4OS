import gdb

def create_and_register_task():
    create_task = gdb.parse_and_eval("create_task")
    register_task = gdb.parse_and_eval("register_task")
    blue_led = gdb.parse_and_eval("blue_led")

    for i in range(10):
        task = create_task(blue_led.address, 1, 0)
        register_task(task)

gdb.Breakpoint("init_heap")
gdb.execute("continue")
gdb.FinishBreakpoint()
gdb.execute("continue")

create_and_register_task()
