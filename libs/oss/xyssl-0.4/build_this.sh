#!/bin/sh
# build_this.sh for xyssl-0.4 library
#	Created by Sky Huang, U-MEDIA Communications, Inc



PATH=${TOOLDIR}/bin:$PATH
export PATH

USE_AES=y USE_RIJNDAEL_CODE=y USE_RC4=y USE_MD2=y USE_MD4=y USE_MD5=y USE_DES=y USE_RSA=y USE_BLOWFISH=y USE_SHA1=y USE_X509_IN=y USE_BASE64=y make
${CROSS}strip -S libxyssl.so* 

## This library is used by application software. 
## Install to target/lib

mkdir -p ${EMLXBASE}/target/lib/
rm -rf ${EMLXBASE}/target/lib/libxyssl.*
cp -a libxyssl.so* ${EMLXBASE}/target/lib/

