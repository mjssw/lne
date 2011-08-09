#!/bin/sh

UNAME=`uname -s 2>/dev/null || echo not`
if [ ! -f config-os.h ]; then
	if [ 'Linux' = $UNAME ]; then
		echo '#define LNE_LINUX' > config-os.h
	elif [ 'FreeBSD' = $UNAME ]; then
		echo '#define LNE_FREEBSD' > config-os.h
	else
		echo '#define LNE_NOSUPPORT' > config-os.h
	fi
fi
echo $UNAME

