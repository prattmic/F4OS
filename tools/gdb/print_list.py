import gdb

class Print_List(gdb.Command):
    """Prints out an F4OS linked list in a pretty format"""

    def __init__(self):
        super(Print_List, self).__init__("print-list", gdb.COMMAND_DATA, gdb.COMPLETE_SYMBOL)

    def invoke(self, arg, from_tty):
        head = gdb.parse_and_eval(arg)
        self.print_list(head)

    def print_list(self, head):
        malformed = False
        seen = []

        print "%x ->" % head.address,

        node = head['next']
        seen.append(node)

        while node != head.address:
            print "%x ->" % node,
            node = node['next']

            if node in seen:
                malformed = True
                break

            seen.append(node)

        print "%x" % node

        if malformed:
            print "(Loop detected.  Malformed list?)"

Print_List()
