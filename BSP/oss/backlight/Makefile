	DIR =$(TOOLDIR)
	KDIR=$(EMLXBASE)/kernel
	ODIR=$(EMLXBASE)/kernel	
#include $(KDIR)/.config 
	
ifneq ($(KERNELRELEASE),)
	EXTRA_CFLAGS += -D__KERNEL__ -DMODULE
	obj-m := ssl_bl.o
else
	PWD := $(shell pwd)
	export ARCH=arm
	export CROSS_COMPILE=${DIR}/bin/arm-linux-

all:
	$(MAKE) -C $(KDIR) M=$(PWD) O=$(ODIR) modules
	#$(CROSS_COMPILE)strip -g *.ko
	#mkdir -p ../drv && cp *.ko ../drv

clean:
	rm -rf *.o *.ko *~ *.mod.c .*cmd .tmp_versions Module.symvers 
	#rm ../drv/*.ko
endif
