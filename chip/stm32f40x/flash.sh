#!/bin/bash

if [ ! -n "$2" ]
then
    echo "Usage: $0 board object.bin"
    exit $E_BADARGS
fi

if [ "$1" == "discovery" ]
then
    st-flash write $2 0x8000000
    exit 0
elif [ "$1" == "px4" ]
then
    commands="speed 12000
    exec device = stm32f405vg
    erase
    loadbin $2,0x08000000
    r
    g
    q"

    echo "$commands" | JLinkExe
else
    echo "Unknown board $BOARD"
    exit $E_BADARGS
fi
