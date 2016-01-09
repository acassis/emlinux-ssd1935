#!/bin/bash
## clean_this.sh for MAGUS Backlight driver

cd src
make clean 
cd api
make clean
cd ../test
make clean

cd $EMLXBASE/rootdisk/lib/modules/
DRV=`find -name gpio.ko`
if [ ! "a${DRV}" = "a" ]; then
	echo Remove $DRV 
	rm -f $DRV
fi

echo "Remove libgpioctrl.so..."
if [ -f ${EMLXBASE}/rootdisk/lib/libgpioctrl.so ]; then
rm -f ${EMLXBASE}/rootdisk/lib/libgpioctrl.so
fi
