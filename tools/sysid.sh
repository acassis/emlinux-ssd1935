#!/bin/sh

if [ $# -lt 5 ]; then
	echo "Usage: sysid.sh sys_id.txt [platform] [customer] [model] [flags]"
	exit 1
fi

SYSID=$1
PLATFORM=$2
CUSTOMER=$3
MODEL=$4
FLAGS=$5

if [ ! -f $SYSID ]; then
	echo "Cannot find ${SYSID}"
	exit 1
fi


# Search platform
PLATFORM_ID_STR=`grep ${PLATFORM} ${SYSID}`
#echo "${PLATFORM_ID_STR}"
if [ "a${PLATFORM_ID_STR}" = "a" ]; then
	# Platform not found
	echo "Platform [${PLATFORM}] not found"
	exit 1
fi
set $PLATFORM_ID_STR
if [ "a$1" = "a${PLATFORM}" ]; then
	PLATFORM_ID=$2
else
	# Platform not found
	echo "Platform [${PLATFORM}] not found"
	exit 1
fi

# Search customer
CUSTOMER_ID_STR=`grep ${CUSTOMER} ${SYSID}`
#echo "${CUSTOMER_ID_STR}"
if [ "a${CUSTOMER_ID_STR}" = "a" ]; then
	# Customer not found
	echo "Cusomter [${CUSTOMER}] not found"
	exit 1
fi
set $CUSTOMER_ID_STR
if [ "a$1" = "a${CUSTOMER}" ]; then
	CUSTOMER_ID=$2
else
	# Customer not found
	echo "Cusomter [${CUSTOMER}] not found"
	exit 1
fi

# Search model 
MODEL_ID_STR=`grep ${MODEL} ${SYSID}`
#echo "${MODEL_ID_STR}"
if [ "a${MODEL_ID_STR}" = "a" ]; then
	# Model not found
	echo "Model [${MODEL}] not found"
	exit 1
fi
set $MODEL_ID_STR
if [ "a$1" = "a${MODEL}" ]; then
	MODEL_ID=$2
else
	# Model not found
	echo "Model [${MODEL}] not found"
	exit 1
fi

# Search flags 
FLAGS_ID_STR=`grep ${FLAGS} ${SYSID}`
#echo "${FLAGS_ID_STR}"
if [ "a${FLAGS_ID_STR}" = "a" ]; then
	# Flags not found
	echo "Flags [${FLAGS}] not found"
	exit 1
fi
set $FLAGS_ID_STR
if [ "a$1" = "a${FLAGS}" ]; then
	FLAGS_ID=$2
else
	# FLAGS not found
	echo "Flags [${FLAGS}] not found"
	exit 1
fi

echo ${PLATFORM_ID} ${CUSTOMER_ID} ${MODEL_ID} ${FLAGS_ID}
exit 0
