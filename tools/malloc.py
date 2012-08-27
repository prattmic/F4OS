import gdb

def print_buddy(buddy):
    for i in range(buddy['min_order'], buddy['max_order']+1):
        heapnode = buddy['list'][i]
        if heapnode:
            print "Order %d:" % i,
            print_heapnode(heapnode)
        else:
            print "Order %d: NULL" % i

def print_heapnode(heapnode):
    print heapnode, "->",
    while heapnode['next']:
        heapnode = heapnode['next']
        print heapnode, "->",
    print "NULL"

gdb.Breakpoint("bootmain.c:28")
#gdb.Breakpoint("start_task_switching")
gdb.execute("continue")

user_buddy = gdb.parse_and_eval("user_buddy")

print "Before malloc:"
print "user_buddy: ", user_buddy
print_buddy(user_buddy)

malloc = gdb.parse_and_eval("malloc")

i = 0
returned = malloc(10000)
while returned:
    i += 1
    print "\nmalloc %d:" % i
    print "Return: ", returned
    print "user_buddy: ", user_buddy
    print_buddy(user_buddy)

    #if i == 4:
    #    gdb.Breakpoint("alloc")
    returned = malloc(10000)

print "\nmalloc %d (failed):" % (i+1)
print "user_buddy: ", user_buddy
print_buddy(user_buddy)

print "\nmalloc'd %d before fail" % i
