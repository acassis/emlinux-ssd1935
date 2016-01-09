#!/bin/sh
# Build_this.sh for libiconv library
#	Created by Max Yin, U-MEDIA Communications, Inc



PATH=${TOOLDIR}/bin:$PATH
export PATH

cd build_${ARCH}
make

install lib/libiconv_plug_linux.so ${EMLXBASE}/rootdisk/lib
