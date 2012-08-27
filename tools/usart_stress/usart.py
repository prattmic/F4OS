#!/usr/bin/env python

import serial
import lorem

def test(string):
    ser.write(string)
    if ser.read(10000) == string:
        return True
    else:
        return False

ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)

n = 64
ipsum = lorem.do_lorem(0,0,n,0,0)

while test(ipsum):
    print "Passed: %d characters" % n

    if n < 500:
        n *= 2
    else:
        n += 20

    ipsum = lorem.do_lorem(0,0,n,0,0)

print "Failed: %d characters" % n

while 1:
    if test(ipsum):
        print "Passed: %d characters" % n
    else:
        print "Failed: %d characters" % n
