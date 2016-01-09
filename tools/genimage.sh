#!/bin/sh
#set -x 
if [ "a${EMLX_PROFLE}" = "a" ]; then
EXLX_PROFILE=U-MEDIA_HCA170
export EMLX_PROFILE
fi

. ${EMLXBASE}/configs/profiles/${EMLX_PROFILE}

if [ "a${CUSTOME_VERSION}" = "a" ]; then
### not define a customed version, use system version
. ${EMLXBASE}/configs/VERSION
else
. ${CUSTOME_VERSION}
fi

VER_STR="${MAJOR}.${MINOR}.${RELEASE}.${BUILD}"


if [ ! -f ${EMLXBASE}/image/uImage -o ! -f ${EMLXBASE}/image/rootdisk.cramfs ]; then
	echo "Cannot find kernel and rootfs files..."
	exit 1
fi

if [ -f ${EMLXBASE}/target/etc/init.d/rc.app ];then
## Target application installed
cd ${EMLXBASE}/target/
ln -sf etc/init.d/rc.app rc.app
chmod +x etc/init.d/rc.app

# Write the company info into etc/customer.info
echo "${PRODUCT_NAME}" > ${EMLXBASE}/target/etc/customer.info
echo "${COMPANY}" >> ${EMLXBASE}/target/etc/customer.info
echo "${COMPANY_URL}" >> ${EMLXBASE}/target/etc/customer.info
echo "${MODEL_DESCRIPTION}" >> ${EMLXBASE}/target/etc/customer.info
echo "${MODEL_NAME}" >> ${EMLXBASE}/target/etc/customer.info
echo "${MODEL_NUMBER}" >> ${EMLXBASE}/target/etc/customer.info

# Write build time into etc/buildtime
TIME_STR=`date +"%Y %b %d, %l:%M %p"`
echo ${TIME_STR} > ${EMLXBASE}/target/etc/buildtime

cd ../image
# pack target to app.cramfs
../tools/mkfs.cramfs -r ../target app.cramfs
fi

mkdir -p ${EMLXBASE}/image/tmp/
rm -rf ${EMLXBASE}/image/tmp/*
cd ${EMLXBASE}/image/tmp/
cp -f ../uImage ./
cp -f ../rootdisk.cramfs ./
cp -f ../app.cramfs ./
cp -f ${EMLXBASE}/apps/oss/mtd-utils/nandwrite ./
cp -f ${EMLXBASE}/apps/oss/mtd-utils/flash_eraseall ./
md5sum app.cramfs > app.cramfs.md5
mkdir app_ext2
mv app.cramfs app_ext2/
mv app.cramfs.md5 app_ext2/
APP_SIZE=`stat -c %s app_ext2/app.cramfs`
EXT_SIZE=`expr ${APP_SIZE} / 1024 + 1024`
../../tools/e2fsimage -f app.ext2 -d app_ext2 -u 0 -g 0 -s ${EXT_SIZE}
rm -rf app_ext2

## extract kernel version
${EMLXBASE}/tools/kernel_version ${EMLXBASE}/kernel/arch/arm/boot/Image > uImage_version

######################################################
## The logo file appendes to kernel image ############
dd if=${EMLXBASE}/image/uImage of=logo_temp.img 1> /dev/null 2>/dev/null
if [ -f ${BL_LOGO_BMP} ] ; then
  echo "INITBMP" >> logo_temp.img 
  cat ${LOGO_BMP} >> logo_temp.img
  size=`ls -l ${LOGO_BMP} | awk '{print $5}'`
  mod_size=`expr ${size} - \( ${size} / 4 \) \* 4`
  while [ ${mod_size} != 0 ]; do
    echo "" >> logo_temp.img
    mod_size=`expr ${mod_size} - 1`
  done
fi

rm -f uImage
mv logo_temp.img uImage
cp -f uImage ../uImage
######################################################

## put all files into a tgz file
tar cvf temp.img *


MODELID_STR=`sh ${EMLXBASE}/tools/sysid.sh ${EMLXBASE}/configs/sys_id.txt ${PLATFORM} ${CUSTOMER} ${MODEL} ${FLAGS}`
${EMLXBASE}/tools/gentag temp.img temp2.img ${MODELID_STR} ${VER_STR} kr

mv temp2.img ../${PRODUCT_NAME}-${USER}-${VER_STR}.img

echo "Image: ${PRODUCT_NAME}-${USER}-${VER_STR}.img generated."
