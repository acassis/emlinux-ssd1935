#!/bin/bash
## build_this.sh script for SSL NFTL driver 
. ${EMLXBASE}/configs/env
make

mkdir -p ${EMLXBASE}/rootdisk/lib/modules/misc/
cp nftl_swap.ko ${EMLXBASE}/rootdisk/lib/modules/misc/
cp nftl_app.ko ${EMLXBASE}/rootdisk/lib/modules/misc/
