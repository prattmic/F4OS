import gdb

class Print_Buddy(gdb.Command):
    """Prints out an F4OS buddy in a pretty format"""

    def __init__(self):
        super(Print_Buddy, self).__init__("print-buddy", gdb.COMMAND_DATA, gdb.COMPLETE_SYMBOL)

    def invoke(self, arg, from_tty):
        buddy = gdb.parse_and_eval(arg)
        self.print_buddy(buddy)

    def print_buddy(self, buddy):
        for i in range(buddy['min_order'], buddy['max_order']+1):
            heapnode = buddy['list'][i]
            if heapnode:
                print "Order %d:" % i,
                self.print_heapnode(heapnode)
            else:
                print "Order %d: NULL" % i

    def print_heapnode(self, heapnode):
        print heapnode, "(%d)" % heapnode['order'], "->",
        while heapnode['next']:
            heapnode = heapnode['next']
            print heapnode, "(%d)" % heapnode['order'], "->",
        print "NULL"

Print_Buddy()
