#!/bin/bash
## clean_this.sh for tsc2007 driver

make clean 

cd $EMLXBASE/rootdisk/lib/modules/
DRV=`find -name tsc2007.ko`
if [ ! "a${DRV}" = "a" ]; then
	echo Remove $DRV 
	rm -f $DRV
fi