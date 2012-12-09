#!/bin/sh
if [ $# -ne 1 ]
then
    echo "Usage $0 base-dir"
    exit $E_BADARGS
fi

cd $1/tools/kconfig-frontends
./bootstrap
./configure --disable-shared --enable-static --disable-gconf --disable-qconf --disable-utils 
make
