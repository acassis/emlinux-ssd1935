#!/bin/bash
## clean_this.sh for WM8978 driver

make clean 

cd $EMLXBASE/rootdisk/lib/modules/
DRVs="snd-codec-wm8978.ko snd-soc-magus-wm8978.ko"
for d in ${DRVs}; do
DRV=`find -name ${d}`
if [ ! "a${DRV}" = "a" ]; then
	echo Remove $DRV
	rm -f $DRV
fi
done
