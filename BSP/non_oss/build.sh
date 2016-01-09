#!/bin/sh
# emlinux/BSP/oss
#	Routine to build Non-Open Source BSPs.
#

. $EMLXBASE/configs/env

if [ "a$1" = "a" ]; then
BUILD=all
# Register all BSPs
BSPs="avdsp"
else
BUILD=$*
BSPs="$BUILD"
fi

cd ${EMLXBASE}/BSP/non_oss
#set -x
for bsp in $BSPs; do
	echo "Build ${bsp}..."
	if [ -d $bsp ]; then
		cd $bsp
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
	cd ${EMLXBASE}/BSP/non_oss
done
