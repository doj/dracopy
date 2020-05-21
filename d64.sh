#!/bin/sh
c1541 -format "$1" d64 $2
cmd="-attach $2"
shift
shift
for i in "$@" ; do
    cmd="$cmd -write $i"
done
c1541 $cmd
