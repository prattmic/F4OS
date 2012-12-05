#!/bin/bash

if [ ! -n "$1" ] 
then
    echo "Usage: $0 object.bin"
    exit $E_BADARGS
fi

st-flash write $1 0x8000000
