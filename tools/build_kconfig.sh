#!/bin/sh

VERSION=3.11.0.0
URL=http://ymorin.is-a-geek.org/download/kconfig-frontends/kconfig-frontends-$VERSION.tar.bz2

set -e

if [ $# -ne 1 ]
then
    echo "Usage $0 kconfig-install-dir"
    exit $E_BADARGS
fi

INSTALL=$1

echo "It looks like the Kconfig tools are not installed on your system."
echo "kconfig-frontends can be downloaded and installed automatically."
echo "kconfig-frontends depends on (at least) autoconf, automake,"
echo "bison, flex, gperf, libtoolize, libncurses5-dev, m4, and pkg-config."
echo "It will be installed to $INSTALL."

read -p "Download and install? (y/n) " response

if [ $response != "y" ]
then
    echo "Canceling build"
    exit 1
fi

if [ -e $INSTALL ]
then
    echo "$INSTALL already exists.  It must be deleted to continue."
    read -p "Delete it and continue? (y/n) " response
    if [ $response != "y" ]
    then
        echo "Canceling build"
        exit 1
    fi

    echo "Removing $INSTALL"
    rm -r $INSTALL
fi

# Continue with download and install

ARCHIVE=$INSTALL/kconfig-frontends.tar.bz2

echo "Creating installation directory"
mkdir -p $INSTALL
cd $INSTALL

echo "Downloading kconfig-frontends to $ARCHIVE"
curl $URL > $ARCHIVE

echo "Extracting kconfig-frontends archive"
tar xjf $ARCHIVE --strip-components=1   # Strip leading directory from archive

echo "Configuring kconfig-frontends"
./bootstrap
./configure --disable-shared --enable-static --disable-gconf --disable-qconf --disable-utils

echo "Building kconfig-frontends"
make

echo "Removing archive"
rm $ARCHIVE
