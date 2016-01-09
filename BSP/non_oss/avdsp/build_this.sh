#!/bin/bash
## build_this.sh script for install avdsp
. ${EMLXBASE}/configs/env

rm -rf ${EMLXBASE}/target/avdsp
mkdir -p ${EMLXBASE}/target/avdsp
if [ -d lib ]; then
cp -R lib ${EMLXBASE}/target/avdsp
rm -rf ${EMLXBASE}/target/avdsp/lib/.svn
echo "copy lib directory in avdsp to target done"
fi
if [ -d local ]; then
cp -R local ${EMLXBASE}/target/avdsp
rm -rf ${EMLXBASE}/target/avdsp/local/.svn
rm -rf ${EMLXBASE}/target/avdsp/local/bin/.svn
rm -rf ${EMLXBASE}/target/avdsp/local/lib/.svn
echo "copy local directory in avdsp to target done"
fi
if [ -d share ]; then
cp -R share ${EMLXBASE}/target/avdsp
rm -rf ${EMLXBASE}/target/avdsp/share/.svn
echo "copy share directory in avdsp to target done"
fi

