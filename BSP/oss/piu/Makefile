	DIR = $(TOOLDIR)
	KDIR= $(EMLXBASE)/kernel

ifneq ($(KERNELRELEASE),)
	obj-m := sslpiu.o
	sslpiu-objs := piu.o piuk.o
	EXTRA_CFLAGS += -DIO_MAP=1
	EXTRA_CFLAGS += -DINTC=0
	EXTRA_CFLAGS += -DLINUX_VERSION=020624
	#EXTRA_CFLAGS += -DPIU_SEMAPHORE_MODE_COMPLETE
	EXTRA_CFLAGS += -DPIU_ENABLE_TIMEOUT
	EXTRA_CFLAGS += -DPIU_NONBLOCK
ifeq ($(KVER), 020624)
	EXTRA_CFLAGS += -I${DIR}/arm-linux/include
endif
else
	PWD := $(shell pwd)
	export ARCH=arm
	export CROSS_COMPILE=${DIR}/bin/arm-linux-
default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	${CROSS_COMPILE}strip -g sslpiu.ko
	# mkdir -p ../drv && cp *.ko ../drv
clean:
	rm -rf *.o *.ko *~ *.mod.c .*cmd .tmp_versions Module.symvers
endif
