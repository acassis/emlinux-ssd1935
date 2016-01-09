#!/bin/bash
## clean_this.sh for RaLink driver

cd RT3070_Linux_STA_v2.1.0.0/
make clean 

cd $EMLXBASE/rootdisk/lib/modules/
DRV=`find -name rt3070sta.ko`
if [ ! "a${DRV}" = "a" ]; then
	echo Remove $DRV 
	rm -f $DRV
fi

