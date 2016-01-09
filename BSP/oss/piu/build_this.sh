#!/bin/bash
## build_this.sh script for PIU driver and access library
. ${EMLXBASE}/configs/env

make
mkdir -p ${EMLXBASE}/rootdisk/lib/modules/misc/
cp sslpiu.ko ${EMLXBASE}/rootdisk/lib/modules/misc/
cp sslvpp.ko ${EMLXBASE}/rootdisk/lib/modules/misc/

cd api
sh mk_nblk.sh
cp libpiu.so ${EMLXBASE}/rootdisk/lib/
cd ../libcalpiu
sh build_this.sh
cp libcalpiu.so ${EMLXBASE}/rootdisk/lib/
