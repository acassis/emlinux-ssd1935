#!/bin/sh
# Build_this.sh for e2fsprogs-1.39 library
#	Created by Sky Huang, U-MEDIA Communications, Inc



PATH=${TOOLDIR}/bin:$PATH
export PATH

if [ ! "a${PLATFORM}" = "a" ]; then
if [ "a${PLATFORM}" = "aARM_SSD1933" ]; then
cd build-arm-ep93xx
fi
fi
cd lib/uuid
make clean
make

## This library is used by application software. 
## Install to target/lib

${CROSS}strip -S ../libuuid.so.1.2
mkdir -p ${EMLXBASE}/target/lib/
cp ../libuuid.so.1.2 ${EMLXBASE}/target/lib/
cd ${EMLXBASE}/target/lib/
rm -rf libuuid.so
ln -s libuuid.so.1.2 libuuid.so

