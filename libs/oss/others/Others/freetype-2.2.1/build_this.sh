#!/bin/bash
# Build_this script for FreeType
# Author: Max Yin, U-MEDiA Communications, Inc.
# All right reserved. 2006(C)
#
if [ ! "a${EMLXBASE}" = "a" ]; then
. ${EMLXBASE}/configs/env
fi

FT_DIR=`pwd`
INSTALL_DIR=${FT_DIR}/../install/

cd build_${ARCH}
make clean
make

if [ ! $? -eq 0 ]; then
	echo FreeType build process failed!!
	exit 1
fi

## install FreeType library to install dir
## install header files
cd ${FT_DIR}

echo "Install header files to ${INSTALL_DIR}..."
cp -R include/* ${INSTALL_DIR}/include > /dev/null

echo "Install libraries to ${INSTALL_DIR}..."
cd build_${ARCH}/.libs
${CROSS}strip -s libfreetype.so.6
install libfreetype.so.6 ${INSTALL_DIR}/lib/
if [ ! -e ${INSTALL_DIR}/lib/libfreetype.so ]; then
	cd ${INSTALL_DIR}/lib
	ln -s libfreetype.so.6 libfreetype.so
fi

echo "Done!"
