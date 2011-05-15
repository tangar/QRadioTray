#!/bin/bash

PROJECTNAME=qradiotray
TEMPDIR="debdir"
BINDIR="/usr/bin"
SHARE="/usr/share"
SHAREDIR="$SHARE/$PROJECTNAME/"

MKDIR=mkdir
COPY=cp
MOVE=mv
VERSION=$1
DELETE="rm -rf"

MAXFILECOUNT=100

#Put here binary of your project
BINFILES=( ./release/$PROJECTNAME )
#put here comman files of your project
SHAREFILES=( ./config.ini ./Changelog.txt ./README ./gnu_licence.txt ./images/radio-active.png ) DESKTOP=qradiotray.desktop 
CONTROL=control


#enter version
echo "Enter version of package:"
read VERSION

$MKDIR $TEMPDIR
$MKDIR $TEMPDIR/DEBIAN
$MKDIR $TEMPDIR/usr
$MKDIR $TEMPDIR/usr/bin
$MKDIR $TEMPDIR/usr/share
$MKDIR $TEMPDIR/usr/share/applications
$MKDIR $TEMPDIR$SHAREDIR

$COPY ${BINFILES[0]}  $TEMPDIR$BINDIR
$COPY ./$DESKTOP $TEMPDIR$SHARE/applications
echo "Version=$VERSION" >> $TEMPDIR$SHARE/applications/$DESKTOP
$COPY ./$CONTROL $TEMPDIR/DEBIAN
echo "Version: $VERSION" >> $TEMPDIR/DEBIAN/$CONTROL
echo "" >> $TEMPDIR/DEBIAN/$CONTROL


for (( index = 0; index <= MAXFILECOUNT; index++ ))
do
    if [ "${SHAREFILES[$index]}" != "" ]
    then
        echo "Copying file ${SHAREFILES[$index]} to $TEMPDIR$SHAREDIR"
        $COPY ${SHAREFILES[$index]} $TEMPDIR$SHAREDIR
    fi
done

dpkg -b ./$TEMPDIR $PROJECTNAME-$VERSION.deb

#$COPY $TEMPDIR/$PROJECTNAME-$VERSION.deb ./

$DELETE $TEMPDIR

