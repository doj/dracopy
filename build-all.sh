#!/bin/bash
# build all color schemes of dracopy.
# build versions with REU support.
# cp all builds into DESTDIR

DESTDIR=../doj/c64

set -e

VER=$(make printversion)

# color schemes
for i in BLUE SX 128 ; do
    make clean

    echo
    echo "##### $i #####"
    echo
    CFLAGS=-DCOLOR_SCHEME_$i make zip
    mv dracopy-${VER}.zip $DESTDIR/dracopy-${VER}-${i}.zip
done

# REU versions
make clean
echo
echo '##### Kerberos #####'
echo
CFLAGS=-DKERBEROS make dc64.zip
mv dc64.zip $DESTDIR/dracopy-${VER}-kerberos.zip

make clean
echo
echo '##### REU #####'
echo
REU=c64-reu.emd make dc64.zip
mv dc64.zip $DESTDIR/dracopy-${VER}-reu.zip

# build the default version
make clean
echo
echo '##### default #####'
echo
make all zip
mv dracopy-${VER}.zip $DESTDIR/
mv dracopy-${VER}.d64 $DESTDIR/

make clean
