#!/bin/bash
## build_this.sh script for MAGUS-Backlight driver
. ${EMLXBASE}/configs/env
make

mkdir -p ${EMLXBASE}/rootdisk/lib/modules/misc/
cp ssl_bl.ko ${EMLXBASE}/rootdisk/lib/modules/misc/
