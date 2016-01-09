srcdir = $(EMLXBASE)/apps/gpl/lirc-0.6.6/drivers/lirc_HCA110
top_srcdir = $(EMLXBASE)/apps/gpl/lirc-0.6.6
top_builddir = $(EMLXBASE)/apps/gpl/lirc-0.6.6/build_$(ARCH)
VPATH = $(EMLXBASE)/apps/gpl/lirc-0.6.6/drivers/lirc_HCA110


DEFAULT_INCLUDES =  -I. -I$(srcdir) -I$(top_builddir) -I$(top_srcdir)

CFLAGS += $(DEFAULT_INCLUDES) -O2 -I$(EMLXBASE)/kernel/include -DHAVE_CONFIG_H


obj-m := lirc_HCA110.o

all:
	cp $(srcdir)/lirc_HCA110.c ./
	make -C $(EMLXBASE)/kernel SUBDIRS=$(shell pwd) modules

clean:
	rm -rf *.ko *.o *.c 
