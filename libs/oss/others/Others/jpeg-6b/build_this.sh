#!/bin/bash
# JPEG lib build script for EMLX

# source in EMLX env
. ${EMLXBASE}/configs/env

# Change to target build dir and build JPEG lib
cd build_${ARCH}
touch *
make libjpeg.la

# Install to target dir
#cp .libs/libjpeg.so.62.0.0 ${EMLXBASE}/target/lib
#$STRIP ${EMLXBASE}/target/lib/libjpeg.so.62.0.0

# Install headers to UI's include dir
make install-headers

# Install libs to UI's include dir
cp -a .libs/lib* ../../lib

#Done !
