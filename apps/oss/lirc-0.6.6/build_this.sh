#!/bin/sh
#set -x
cd build_$ARCH
if [ "a${MODEL}" = "aHCA-110" ]; then
GCCPATH=`dirname ${CC}`
export PATH=${GCCPATH}:${PATH}
cd drivers/lirc_HCA110
make
cp lirc_HCA110.ko $EMLXBASE/target/lib/modules
cd ../../
fi

cd daemons
make 
${CROSS}strip lircd 
install lircd $EMLXBASE/target/usr/sbin/lircd



