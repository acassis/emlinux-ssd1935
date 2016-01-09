#!/bin/bash
## clean_this.sh for PIU driver and access library

make clean 
rm -f api/libpiu.so
rm -f libcalpiu/libcalpiu.so

cd $EMLXBASE/rootdisk/lib/modules/
DRV=`find -name sslpiu.ko`
if [ ! "a${DRV}" = "a" ]; then
	echo Remove $DRV 
	rm -f $DRV
fi
rm -f ${EMLXBASE}/rootdisk/lib/modules/sslvpp.ko

echo "Remove libpiu.so..."
if [ -f ${EMLXBASE}/rootdisk/lib/libpiu.so ]; then
rm -f ${EMLXBASE}/rootdisk/lib/libpiu.so
fi

echo "Remove libcalpiu.so..."
if [ -f ${EMLXBASE}/rootdisk/lib/libcalpiu.so ]; then
rm -f ${EMLXBASE}/rootdisk/lib/libcalpiu.so
fi

