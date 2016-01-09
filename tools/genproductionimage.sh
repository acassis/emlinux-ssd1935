#!/bin/sh

usage(){
	echo "genproductionimage.sh: Generate image for production"
	echo "Usage:"
	echo "	genproductionimage.sh [bootloader] [system F/W]"
	echo ""

	exit 0
} 

extract_header_fw(){
INF=$1
OUT=$2

mkdir -p ${OUT}

## extract header
dd if=${INF} of=${OUT}/UMDA_header.img count=1 bs=16
dd if=${INF} of=${OUT}/FW.tar.gz bs=16 skip=1

cd ${OUT}/
tar xvf FW.tar.gz

rm FW.tar.gz
cd ../
}

attach_fw_header_2_app(){
OUT=$1

cd ${OUT}

mkdir tmp
cp UMDA_header.img tmp
${EMLXBASE}/tools/e2fsimage -n -f app.ext2 -d tmp/
rm -rf tmp

cd ../
}


if [ $# -lt 2 ]; then 
usage
fi

if [ "a${EMLXBASE}" = "a" ]; then
echo "Please specify EMLXBASE env.  var.!"
exit 0
fi

BOOT=$1
FW=$2
OUTFILE=`basename ${FW}`
OUTDIR=`basename ${FW}`_FlashImage

extract_header_fw ${FW} ${OUTDIR}

cp ${BOOT} ${OUTDIR}/
# add UMDA_header to nbbot.bin
attach_fw_header_2_app ${OUTDIR}

zip -r ${OUTDIR}.zip ${OUTDIR}

rm -rf ${OUTDIR}