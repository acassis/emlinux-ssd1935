#!/bin/bash
## build_this.sh script for SDHC driver
. ${EMLXBASE}/configs/env
make

mkdir -p ${EMLXBASE}/rootdisk/lib/modules/misc/
cp sslsd.ko ${EMLXBASE}/rootdisk/lib/modules/misc/

