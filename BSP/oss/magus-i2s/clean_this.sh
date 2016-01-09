#!/bin/bash
## clean_this.sh for MAGUS I2S driver

make clean 

cd $EMLXBASE/rootdisk/lib/modules/
DRV=`find -name snd-soc-magus-i2s.ko`
if [ ! "a${DRV}" = "a" ]; then
	echo Remove $DRV 
	rm -f $DRV
fi