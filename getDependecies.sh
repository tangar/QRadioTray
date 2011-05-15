#!/bin/bash
LIBS=`ldd $1 | awk '{print $3}'`

PKGS=`(for lib in $LIBS; do
apt-file -Fl search $lib
done) | sort | uniq`

(for pkg in $PKGS; do
echo $pkg `apt-cache show $pkg | grep Version | awk '{print $2}'` >> dependencies
done)
