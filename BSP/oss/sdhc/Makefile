	DIR =$(TOOLDIR)
	KDIR=$(EMLXBASE)/kernel

#include $(KDIR)/.config 
	
ifneq ($(KERNELRELEASE),)
	EXTRA_CFLAGS += -D__KERNEL__ -DMODULE
	EXTRA_CFLAGS += -DIO_MAP=1 -DINTC=0 -DSDHC_IO=8 -DSD_DMA=1
	obj-m := sslsd.o
	sslsd-objs := ssl_sd.o sdhc.o
else
	PWD := $(shell pwd)
	export ARCH=arm
	export CROSS_COMPILE=${DIR}/bin/arm-linux-

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	$(CROSS_COMPILE)strip -g *.ko
#	mkdir -p ../drv && cp *.ko ../drv

clean:
	rm -rf *.o *.ko *~ *.mod.c .*cmd .tmp_versions Module.symvers
	# rm ../drv/*.ko
endif
