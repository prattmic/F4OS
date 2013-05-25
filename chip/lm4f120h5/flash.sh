#!/bin/bash

if [ ! -n "$1" ]
then
    echo "Usage: $0 object.bin"
    exit $E_BADARGS
fi

commands="speed 12000
exec device = lm4f120h5qr
loadbin $1,0x00000000
r
g
q"

echo "$commands" | JLinkExe
