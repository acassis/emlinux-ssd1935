

# Comment/uncomment the following line to disable/enable debugging
#DEBUG = y


# Add your debugging flag (or not) to CFLAGS
ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g -DSBULL_DEBUG # "-O" is needed to expand inlines
else
  DEBFLAGS = -O2
endif

EXTRA_CFLAGS = $(DEBFLAGS)
EXTRA_CFLAGS += -I..
EXTRA_CFLAGS +=-I.
EXTRA_CFLAGS +=-I$(EMLXBASE)/kernel/include/linux
EXTRA_CFLAGS +=-DMAGUS_LINUX

export ARCH=arm
export CROSS_COMPILE=arm-linux-

ifneq ($(KERNELRELEASE),)
# call from kernel build system

# nftl-objs :=nftlcore.o nftlmount.o
nftl_swap-objs :=nftlcore.o nftlmount.o
nftl_app-objs :=nftlcore_app.o nftlmount_app.o

obj-m	:= nftl_swap.o nftl_app.o

else

KERNELDIR ?= $(EMLXBASE)/kernel

PWD       := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

endif



clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions

depend .depend dep:
	$(CC) $(CFLAGS) -M *.c > .depend


ifeq (.depend,$(wildcard .depend))
include .depend
endif

