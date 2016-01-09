#!/bin/bash
# PNG lib build script for EMLX

# source in EMLX env
. ${EMLXBASE}/configs/env

# Change to target build dir and build PNG lib
cd build_${ARCH}
make

# Install headers to UI's include dir
cp ../png.h ../../include
cp ../pngconf.h ../../include

# Install libs to UI's include dir
cp -a .libs/libpng12.so* ../../lib

#Done !
