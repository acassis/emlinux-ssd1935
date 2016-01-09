#!/bin/bash
## clean_this.sh for MAGUS PCM driver

make clean 

cd $EMLXBASE/rootdisk/lib/modules/
DRV=`find -name snd-soc-magus-pcm.ko`
if [ ! "a${DRV}" = "a" ]; then
	echo Remove $DRV 
	rm -f $DRV
fi