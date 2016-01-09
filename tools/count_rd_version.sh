#!/bin/bash
## count rootdisk's version

VER=`cat ${EMLXBASE}/configs/rootdisk.version|cut -c 2-100`

NEW_VER=`expr $VER + 1`

echo "r${NEW_VER}" > ${EMLXBASE}/configs/rootdisk.version
