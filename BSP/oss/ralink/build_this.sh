#!/bin/bash
## build_this.sh script for RaLink RT3070 driver 
. ${EMLXBASE}/configs/env
cd RT3070_Linux_STA_v2.1.2.0/
make

mkdir -p ${EMLXBASE}/rootdisk/lib/modules/misc/
cp os/linux/rt3070sta.ko ${EMLXBASE}/rootdisk/lib/modules/misc/

mkdir -p ${EMLXBASE}/rootdisk/etc/Wireless/
cp RT2870STA_def.dat ${EMLXBASE}/rootdisk/etc/Wireless/
