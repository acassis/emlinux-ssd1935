#!/bin/bash
## build_this.sh script for MAGUS-PCM driver
. ${EMLXBASE}/configs/env
make

mkdir -p ${EMLXBASE}/rootdisk/lib/modules/misc/
cp snd-soc-magus-pcm.ko ${EMLXBASE}/rootdisk/lib/modules/misc/
