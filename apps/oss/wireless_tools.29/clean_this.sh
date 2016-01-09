make clean

. build_this.sh none

for p in $INSTALL_PROGRAMS ; do
	echo "Remove $p ..."
	rm -f ${EMLXBASE}/rootdisk/sbin/${p}
done

for p in $INSTALL_LIBRARIES ; do
	echo "Remove $p ..."
	rm -f ${EMLXBASE}/rootdisk/sbin/${p}
done
