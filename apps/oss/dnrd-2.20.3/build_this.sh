#!/bin/sh
# Build script for DNRD, Domain Name Relay Daemon
# Created by Max Yin, U-MEDIA Communications, Inc.
# 2005/12/27
#

. $EMLXBASE/configs/env
export PATH=$TOOLDIR/bin:$PATH
make

$STRIP -s src/dnrd

cp src/dnrd $EMLXBASE/rootdisk/usr/sbin
