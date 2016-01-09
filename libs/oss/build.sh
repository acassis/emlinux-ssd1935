#!/bin/sh
# emlinux/libs/oss
#	Routine to build Open Source BSPs.
#

. $EMLXBASE/configs/env

if [ "a$1" = "a" ]; then
BUILD=all
# Register all LIBs
LIBs="e2fsprogs-1.39 xyssl-0.4 libredblack-1.3 fuse-2.7.4 ntfs-3g-2009.2.1"
else
BUILD=$*
LIBs="$BUILD"
fi

cd ${EMLXBASE}/libs/oss
#set -x
for lib in $LIBs; do
	echo "Build ${lib}..."
	if [ -d $lib ]; then
		cd $lib
		if [ -f build_this.sh ]; then
			sh ./build_this.sh 
		else
			if [ ! -f Makefile ] ; then
				# some applications might need other parameters
				CC=${CROSS}gcc ./configure
			fi
			make
			#FIXME: no make install here
		fi
	fi
	cd ${EMLXBASE}/libs/oss
done
