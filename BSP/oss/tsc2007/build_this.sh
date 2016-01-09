#!/bin/bash
## build_this.sh script for TSC2007 touch screen driver
. ${EMLXBASE}/configs/env
make

mkdir -p ${EMLXBASE}/rootdisk/lib/modules/misc/
cp tsc2007.ko ${EMLXBASE}/rootdisk/lib/modules/misc/
