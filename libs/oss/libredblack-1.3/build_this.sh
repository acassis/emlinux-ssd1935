#!/bin/sh
# build_this.sh for libredblack-1.3 library
#	Created by Sky Huang, U-MEDIA Communications, Inc



PATH=${TOOLDIR}/bin:$PATH
export PATH

LDFLAGS= make
${CROSS}strip -S .libs/libredblack.so* 
cp -a .libs/libredblack.so* ${EMLXBASE}/target/lib/

