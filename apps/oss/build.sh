#!/bin/sh
# emlinux/apps/oss
#	Routine to build Open Source apps.
#

. $EMLXBASE/configs/env

if [ "a$1" = "a" ]; then
BUILD=all
# Register all APPs
#APPS="wireless_tools.26 openntpd-3.7p1 dnrd-2.20.1 lirc-0.6.6"
APPS="mtd-utils wireless_tools.29 dnrd-2.20.3 openntpd-3.7p1"
else
BUILD=$*
APPS="$BUILD"
fi

# For BusyBox 1.xx
if [ "a$BUILD" = "aall" -o "a$BUILD" = "abusybox" ]; then
cd busybox-1.8.2-ssl
if [ -f ${EMLXBASE}/configs/apps/oss/busybox-config ]; then
	cp ${EMLXBASE}/configs/apps/oss/busybox-config ./.config
else
	make menuconfig
	cp .config ${EMLXBASE}/configs/apps/oss/busybox-config
fi
CROSS_COMPILE=${CROSS} make
CROSS_COMPILE=${CROSS} make install
fi
cd ${EMLXBASE}/apps/oss
#set -x
for app in $APPS; do
	echo "Build ${app}..."
	if [ -d $app ]; then
		cd $app
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
	cd ${EMLXBASE}/apps/oss
done
