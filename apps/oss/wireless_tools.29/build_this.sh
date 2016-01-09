
INSTALL_PROGRAMS="iwconfig iwpriv"
INSTALL_LIBRARIES="libiw.so.29"

CMD=$1

if [ "a${CMD}" = "a" ]; then
	CMD="all"
fi

case $CMD in
all)
	make

	for p in $INSTALL_PROGRAMS ; do
		echo "Install $p ..."
		${STRIP} -s $p
		cp $p ${EMLXBASE}/rootdisk/sbin/
	done
	for p in $INSTALL_LIBRARIES ; do
		echo "Install $p ..."
		cp $p ${EMLXBASE}/rootdisk/lib/
	done
	
	;;
*)
	;;
esac


