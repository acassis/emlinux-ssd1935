#!/bin/sh
#set -x
if [ "a$1" = "a" ]; then
BUILD=all
else
BUILD=$1
fi

if [ "a$EMLX_PROFILE" = "a" ]; then
# Profile is not specified, use U-MEDIA's generic one
EMLX_PROFILE=U-MEDIA_HCA170
export EMLX_PROFILE
fi

EMLXBASE=`pwd`
#Export build environment
. $EMLXBASE/configs/env

export EMLXBASE 

export PATH=${TOOLDIR}/bin:${PATH}

if [ "a$BUILD" = "akernelconfig" ]; then
	cd kernel
	make menuconfig
	cp .config ${EMLXBASE}/configs/kernel/kernel-config
	exit 0
fi

if [ "a$BUILD" = "auclibcconfig" ]; then
	cd libs/gpl/uClibc
	make menuconfig
	cp .config ${EMLXBASE}/configs/libs/oss/uClibc-config
	exit 0
fi

if [ "a$BUILD" = "abusyboxconfig" ]; then
	cd apps/oss/busybox-1.8.2-ssl
	make menuconfig
	cp .config ${EMLXBASE}/configs/apps/oss/busybox-config
	exit 0
fi

NEED_REBUILD_ROOTDISK="n"

if [ "a$BUILD" = "aall" ]; then
	sh tools/count_version.sh
fi

if [ "a$BUILD" = "akernel" -o "a$BUILD" = "aall" ]; then
echo "Build kernel..."
CFLAGS=""
LDFLAGS=""
cd kernel
if [ -f $EMLXBASE/configs/kernel/kernel-config ]; then
	if [ ! -f $EMLXBASE/kernel/.config ]; then
		cp $EMLXBASE/configs/kernel/kernel-config ./.config
		make oldconfig
		make dep
	fi
else  # system doesn't have kernel configuration
	make menuconfig
	cp .config $EMLXBASE/configs/kernel/kernel-config
	make dep
fi

cd ${EMLXBASE}/kernel
make $KERNEL_IMAGE
if [ ! $? -eq 0 ];then
	echo "Build kernel failed!"
	exit 1
fi
cp $EMLXBASE/kernel/arch/$ARCH/boot/$KERNEL_IMAGE $EMLXBASE/image

make modules
INSTALL_MOD_PATH=$EMLXBASE/rootdisk
export INSTALL_MOD_PATH
make modules_install
cd $EMLXBASE/rootdisk/lib/modules/${KERNEL_VER_TAG}
rm -f build source
ln -s ../misc ./
cd $EMLXBASE
NEED_REBUILD_ROOTDISK="y"
fi

if [ "a$BUILD" = "absps" -o "a$BUILD" = "aall" ]; then
echo "Build BPSs..."
mkdir -p ${EMLXBASE}/rootdisk/lib/modules/${KERNEL_VER_TAG}/kernel/drivers/misc/
cd BSP/non_oss
sh ./build.sh
cd ../oss
sh ./build.sh
cd $EMLXBASE
NEED_REBUILD_ROOTDISK="y"
fi

if [ "a$BUILD" = "alibs" -o "a$BUILD" = "aall" ]; then
echo "Build Libs..."
cd libs/oss
sh ./build.sh
cd $EMLXBASE
NEED_REBUILD_ROOTDISK="y"
fi

if [ "a$BUILD" = "aapps" -o "a$BUILD" = "aall" ]; then
echo "Build Apps..."
cd apps/oss
sh ./build.sh
cd $EMLXBASE
cd apps/non_oss
sh ./build.sh
NEED_REBUILD_ROOTDISK="y"
fi

if [ "a${NEED_REBUILD_ROOTDISK}" = "ay" ]; then
## count rootdisk's revision
sh ${EMLXBASE}/tools/count_rd_version.sh
fi

# Build flash image
mkdir -p ${EMLXBASE}/rootdisk/usr
mkdir -p ${EMLXBASE}/rootdisk/usr/sbin
mkdir -p ${EMLXBASE}/rootdisk/usr/bin
mkdir -p ${EMLXBASE}/rootdisk/usr/lib
mkdir -p ${EMLXBASE}/rootdisk/usr/lib/Qt
mkdir -p ${EMLXBASE}/rootdisk/usr/local
mkdir -p ${EMLXBASE}/rootdisk/usr/local/lib
mkdir -p ${EMLXBASE}/rootdisk/usr/local/bin
mkdir -p ${EMLXBASE}/rootdisk/usr/X11R6
mkdir -p ${EMLXBASE}/rootdisk/dev  # use udev
mkdir -p ${EMLXBASE}/rootdisk/var
mkdir -p ${EMLXBASE}/rootdisk/proc
mkdir -p ${EMLXBASE}/rootdisk/etc
mkdir -p ${EMLXBASE}/rootdisk/mnt
mkdir -p ${EMLXBASE}/rootdisk/mnt/nfs
mkdir -p ${EMLXBASE}/rootdisk/mnt/usb
mkdir -p ${EMLXBASE}/rootdisk/mnt/disks
mkdir -p ${EMLXBASE}/rootdisk/mnt/irdb
mkdir -p ${EMLXBASE}/rootdisk/mnt/app
mkdir -p ${EMLXBASE}/rootdisk/mnt/appdata
mkdir -p ${EMLXBASE}/rootdisk/mnt/usrdata
mkdir -p ${EMLXBASE}/rootdisk/www
mkdir -p ${EMLXBASE}/rootdisk/tmp
mkdir -p ${EMLXBASE}/rootdisk/emlx-app

cp -a ${EMLXBASE}/configs/etc/* ${EMLXBASE}/rootdisk/etc/
chmod +x ${EMLXBASE}/rootdisk/etc/init.d/rcS
cd ${EMLXBASE}/rootdisk/sbin
ln -s /etc/init.d/hotplug hotplug

# copy BOOT LOGO
mkdir -p ${EMLXBASE}/rootdisk/etc/bmps
if [ -f ${LOGO_BMP} ]; then
cp ${LOGO_BMP} ${EMLXBASE}/rootdisk/etc/bmps/splash.bmp.gz
fi
if [ -f ${NOAPP_BMP} ]; then
cp ${NOAPP_BMP} ${EMLXBASE}/rootdisk/etc/bmps/noapp.bmp.gz
fi
if [ -f ${UPGRADE_BMP} ]; then
cp ${UPGRADE_BMP} ${EMLXBASE}/rootdisk/etc/bmps/upgrade.bmp.gz
fi

# chmod +x ${EMLXBASE}/rootdisk/etc/init.d/hotplug
cd ${EMLXBASE}

# cp -a ${EMLXBASE}/configs/dev/* ${EMLXBASE}/rootdisk/dev/

# Make rootfs 
cd $EMLXBASE/image
rm -rf rootfs
mkdir rootfs

cp -a ../rootdisk/* rootfs/
# Remove SVN dirs under rootfs
cd rootfs
SVNDIR=`find -name .svn`
for svn in $SVNDIR; do rm -rf $svn; done 

# Strip symbol in modules
cd lib/modules
MODOBJ=`find *| grep "\.o"`
for m in $MODOBJ ; do
	${STRIP} --strip-unneeded $m
done

# move to app.cramfs packing
# Write the company info into /etc/customer.info
#echo "${PRODUCT_NAME}" > ${EMLXBASE}/image/rootfs/etc/customer.info
#echo "${COMPANY}" >> ${EMLXBASE}/image/rootfs/etc/customer.info
#echo "${COMPANY_URL}" >> ${EMLXBASE}/image/rootfs/etc/customer.info
#echo "${MODEL_DESCRIPTION}" >> ${EMLXBASE}/image/rootfs/etc/customer.info
#echo "${MODEL_NAME}" >> ${EMLXBASE}/image/rootfs/etc/customer.info
#echo "${MODEL_NUMBER}" >> ${EMLXBASE}/image/rootfs/etc/customer.info

# Write build time into /etc/buildtime
#TIME_STR=`date +"%Y %b %d, %l:%M %p"`
#echo ${TIME_STR} > ${EMLXBASE}/image/rootfs/etc/buildtime
######

cd ${EMLXBASE}/rootdisk/usr/bin
ln -sf /bin/busybox killall
ROOTVER=`cat ${EMLXBASE}/configs/rootdisk.version`
echo "${PRODUCT_NAME}-${ROOTVER}" > ${EMLXBASE}/image/rootfs/etc/rootdisk.version

echo "/lib" > ${EMLXBASE}/image/rootfs/etc/ld.so.conf
echo "/usr/lib" >> ${EMLXBASE}/image/rootfs/etc/ld.so.conf
/sbin/ldconfig -v -r ${EMLXBASE}/image/rootfs

cd ${EMLXBASE}/kernel
../tools/depmod -ae -F System.map -b ${EMLXBASE}/rootdisk -r ${KERNEL_VER_TAG}
cd ${EMLXBASE}/image 

${EMLXBASE}/tools/mkfs.cramfs -r rootfs rootdisk.cramfs

sh ${EMLXBASE}/tools/genimage.sh

echo "Done"
