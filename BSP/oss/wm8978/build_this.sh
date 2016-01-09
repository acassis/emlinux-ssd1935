#!/bin/bash
## build_this.sh script for WM8978 driver
. ${EMLXBASE}/configs/env
make

mkdir -p ${EMLXBASE}/rootdisk/lib/modules/misc/
cp snd-codec-wm8978.ko ${EMLXBASE}/rootdisk/lib/modules/misc/
cp snd-soc-magus-wm8978.ko ${EMLXBASE}/rootdisk/lib/modules/misc/
