#!/bin/bash
## clean_this.sh for MAGUS Backlight driver

make clean 

cd $EMLXBASE/rootdisk/lib/modules/
DRV=`find -name ssl_bl.ko`
if [ ! "a${DRV}" = "a" ]; then
	echo Remove $DRV 
	rm -f $DRV
fi