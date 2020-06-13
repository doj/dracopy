#!/bin/sh
# build all color schemes of dracopy

set -e

for i in BLUE SX 128 ; do
    make clean
    CFLAGS=-DCOLOR_SCHEME_$i make zip
    mv dracopy-1.0doj.zip ../doj/c64/dracopy-1.0doj-$i.zip
done

# build the default version
make clean
make zip
