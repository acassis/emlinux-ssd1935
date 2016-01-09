#!/bin/bash
## build_this.sh script for MAGUS-I2S driver
. ${EMLXBASE}/configs/env
make

mkdir -p ${EMLXBASE}/rootdisk/lib/modules/misc/
cp snd-soc-magus-i2s.ko ${EMLXBASE}/rootdisk/lib/modules/misc/
