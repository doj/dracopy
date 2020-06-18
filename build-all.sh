#!/bin/sh
# build all color schemes of dracopy.
# build versions with REU support.

set -e

# color schemes
for i in BLUE SX 128 ; do
    make clean
    CFLAGS=-DCOLOR_SCHEME_$i make zip
    mv dracopy-1.0doj.zip ../doj/c64/dracopy-1.0doj-$i.zip
done

# REU versions
make clean
CFLAGS=-DKERBEROS make zip
mv dracopy-1.0doj.zip ../doj/c64/dracopy-1.0doj-kerberos.zip

make clean
REU=c64-reu.emd make zip
mv dracopy-1.0doj.zip ../doj/c64/dracopy-1.0doj-reu.zip

# build the default version
make clean
make zip
