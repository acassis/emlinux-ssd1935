#!/bin/sh
## Build_this script for FUSE
#
# Created by Max Yin, U-MEDIA Communications, Inc.
# Date: 2007/3/29

. $EMLXBASE/configs/env

export PATH=${TOOLDIR}/bin/:${PATH}

make -C kernel
make -C lib
make -C util

cp kernel/fuse.ko  ${EMLXBASE}/rootdisk/lib/modules/misc
cp -a lib/.libs/lib*.so* $EMLXBASE/rootdisk/lib/
cp util/ulockmgr_server $EMLXBASE/rootdisk/bin/
${CROSS}strip -S $EMLXBASE/rootdisk/bin/ulockmgr_server
cp util/fusermount $EMLXBASE/rootdisk/bin/
${CROSS}strip -S $EMLXBASE/rootdisk/bin/fusermount
