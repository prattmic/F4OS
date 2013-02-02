import gdb

gdb.Breakpoint("i2c_read")
gdb.Breakpoint("i2c_write")

gdb.execute("set height 0")
gdb.execute("continue")

while True:
    gdb.execute("finish")

    ret = gdb.parse_and_eval("$1")

    if ret > 0:
        gdb.execute("continue")
    else:
        break
