#!/bin/sh
### LDLIBS="$LDLIBS "
make 
${CROSS}strip ntpd
install ntpd $EMLXBASE/rootdisk/usr/sbin/ntpd

