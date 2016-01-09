#!/bin/bash
## clean_this.sh for SSL NFTL driver and access library

make clean 

cd $EMLXBASE/rootdisk/lib/modules/
DRV=`find -name nftl.ko`
if [ ! "a${DRV}" = "a" ]; then
	echo Remove $DRV 
	rm -f $DRV
fi

