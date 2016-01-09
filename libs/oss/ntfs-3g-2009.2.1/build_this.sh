#!/bin/sh
## Build_this script for FUSE
#
# Created by Max Yin, U-MEDIA Communications, Inc.
# Date: 2007/3/29

. ${EMLXBASE}/configs/env

PATH=${TOOLDIR}/bin:$PATH
export PATH

make

cp -a libntfs-3g/.libs/lib*.so* ${EMLXBASE}/rootdisk/lib/

cp -a src/.libs/ntfs-3g ${EMLXBASE}/rootdisk/sbin/mount.ntfs-3g
${STRIP} -S ${EMLXBASE}/rootdisk/sbin/mount.ntfs-3g

