#!/bin/bash
## build_this.sh script for MAGUS-Backlight driver
. ${EMLXBASE}/configs/env
cd src
make

mkdir -p ${EMLXBASE}/rootdisk/lib/modules/misc/
cp gpio.ko ${EMLXBASE}/rootdisk/lib/modules/misc/

cd api
make
cp libgpioctrl.so ${EMLXBASE}/rootdisk/lib/

cd ../test
make
#install HCA700_IO ${EMLXBASE}/bin
