#!/bin/sh
if [ -z "$TYPE" ]
then TYPE=d64
fi

c1541 -format "$1" $TYPE $2

cmd="-attach $2"
shift
shift

for i in "$@" ; do
    cmd="$cmd -write $i"
done

c1541 $cmd
