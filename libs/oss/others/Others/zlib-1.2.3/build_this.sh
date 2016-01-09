#!/bin/bash
# Build_this script for zlib

. $EMLXBASE/configs/env

make

## install header files
cp zlib.h zconf.h ../include/

cp libz.a ../lib

