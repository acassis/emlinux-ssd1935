
INSTALL_PROGRAMS="nanddump nandwrite flash_eraseall flash_erase"

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
	;;
*)
	;;
esac


