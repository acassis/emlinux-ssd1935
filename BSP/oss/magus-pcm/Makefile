	DIR =$(TOOLDIR)
	KDIR=$(EMLXBASE)/kernel
	ODIR=$(EMLXBASE)/kernel

ifneq ($(KERNELRELEASE),)
	EXTRA_CFLAGS += -D__KERNEL__ -DMODULE
	snd-soc-magus-pcm-objs := magus-pcm.o
	obj-m := snd-soc-magus-pcm.o
else
	PWD := $(shell pwd)
	export ARCH=arm
	export CROSS_COMPILE=${DIR}/bin/arm-linux-

all:
	$(MAKE) -C $(KDIR) M=$(PWD) O=$(ODIR) modules
	$(CROSS_COMPILE)strip -g *.ko
	# mkdir -p ../../drv && cp *.ko ../../drv

clean:
	rm -rf  ../../drv/snd-soc-magus-pcm.ko *.o *.ko *~ *.mod.c .*cmd .tmp_versions Module.symvers 
endif
