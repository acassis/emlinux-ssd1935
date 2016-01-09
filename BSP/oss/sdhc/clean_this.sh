#!/bin/bash
## clean_this.sh for SDHC driver

make clean 

cd $EMLXBASE/rootdisk/lib/modules/
DRV=`find -name sslsd.ko`
if [ ! "a${DRV}" = "a" ]; then
	echo Remove $DRV 
	rm -f $DRV
fi

