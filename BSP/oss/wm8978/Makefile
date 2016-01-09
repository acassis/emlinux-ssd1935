	DIR =/usr/local/arm/3.4.6
	KDIR=$(EMLXBASE)/kernel
	ODIR=$(EMLXBASE)/kernel

ifneq ($(KERNELRELEASE),)
	EXTRA_CFLAGS += -D__KERNEL__ -DMODULE
	snd-codec-wm8978-objs := wm8978.o wm8978-thread.o wm8978-interface.o
	snd-soc-magus-wm8978-objs := magus-wm8978.o
	obj-m := snd-soc-magus-wm8978.o snd-codec-wm8978.o
else
	PWD := $(shell pwd)
	export ARCH=arm
	export CROSS_COMPILE=${DIR}/bin/arm-linux-

all:
	$(MAKE) -C $(KDIR) M=$(PWD) O=$(ODIR) modules
	$(CROSS_COMPILE)strip -g *.ko
	# mkdir -p ../../drv && cp *.ko ../../drv

clean:
	rm -rf ../../drv/snd-soc-magus-wm8978.ko ../../drv/snd-codec-wm8978.ko *.o *.ko *~ *.mod.c .*cmd .tmp_versions Module.symvers 
endif
