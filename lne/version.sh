#!/bin/sh

DEF_VER=0.0.0
if [ -d .git -o -d ../.git ]
then
	VN=`git describe --tags --abbrev=0 2>/dev/null || echo $DEF_VER`
else
	VN="$DEF_VER"
fi
echo $VN

