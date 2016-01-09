# Makefile.in generated automatically by automake 1.5 from Makefile.am.

# Copyright 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001
# Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.



SHELL = /bin/sh

srcdir = ../../daemons
top_srcdir = ../..
VPATH = ../../daemons
prefix = /usr/local
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
libexecdir = ${exec_prefix}/libexec
datadir = ${prefix}/share
sysconfdir = /etc
sharedstatedir = ${prefix}/com
localstatedir = /var
libdir = ${exec_prefix}/lib
infodir = ${prefix}/info
mandir = ${prefix}/man
includedir = ${prefix}/include
oldincludedir = /usr/include
pkgdatadir = $(datadir)/lirc
pkglibdir = $(libdir)/lirc
pkgincludedir = $(includedir)/lirc
top_builddir = ..

ACLOCAL = ${SHELL} ${EMLXBASE}/apps/gpl/lirc-0.6.6/missing --run aclocal
AUTOCONF = ${SHELL} ${EMLXBASE}/apps/gpl/lirc-0.6.6/missing --run autoconf
AUTOMAKE = ${SHELL} ${EMLXBASE}/apps/gpl/lirc-0.6.6/missing --run automake
AUTOHEADER = ${SHELL} ${EMLXBASE}/apps/gpl/lirc-0.6.6/missing --run autoheader

INSTALL = /usr/bin/ginstall -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL}
INSTALL_HEADER = $(INSTALL_DATA)
transform = s,x,x,
NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
host_alias = 
host_triplet = i686-pc-linux-gnu
AMTAR = ${SHELL} ${EMLXBASE}/apps/gpl/lirc-0.6.6/missing --run tar
AS = @AS@
AWK = gawk
#CC = gcc
DEPDIR = .deps
DLLTOOL = @DLLTOOL@
ECHO = echo
EXEEXT = 
INSTALL_STRIP_PROGRAM = ${SHELL} $(install_sh) -c -s
LIBTOOL = $(SHELL) $(top_builddir)/libtool
LN_S = ln -s
OBJDUMP = @OBJDUMP@
OBJEXT = o
PACKAGE = lirc
#RANLIB = ranlib
#STRIP = strip
VERSION = 0.6.6
X_CFLAGS = 
X_EXTRA_LIBS = 
X_LIBS = 
X_PRE_LIBS = 
ac_pkss_mktemp = yes
am__include = include
am__quote = 
caraca_lib = 
daemon = 
depmod = /sbin/depmod
devdir = /dev
driver = edb9315
forkpty = -lutil
hw_module = hw_default.o receive.o transmit.o
install_sh = ${EMLXBASE}/apps/gpl/lirc-0.6.6/install-sh
irman_lib = 
irtty = 
#kernelcc = /bin/arm-linux-gcc
kernelcc = ${KTOOLDIR}/bin/arm-linux-gcc
kerneldir = ${EMLXBASE}/kernel/
lirc_driver = lirc_edb9315
lirc_major = 61
lircd_conf = 
lircmd_conf = 
maintmode_daemons_extra = 
mknod = /usr/bin/mknod
#moduledir = /lib/modules/2.4.22/misc
moduledir = /lib/modules/2.4.21-rmk1-crus1.4.3/misc
vga_progs = smode2
x_progs = 

EXTRA_DIST = dump_config.c dump_config.h

sbin_PROGRAMS = lircd lircmd

lircd_SOURCES = lircd.c ir_remote.c config_file.c \
		lircd.h ir_remote.h config_file.h \
		hw-types.c hw-types.h hardware.h

lircd_LDADD = hw_default.o receive.o transmit.o   
lircd_DEPENDENCIES = hw_default.o receive.o transmit.o
EXTRA_lircd_SOURCES = hw_default.c hw_default.h \
			hw_pixelview.c hw_pixelview.h \
			hw_pinsys.c hw_pinsys.h \
			hw_logitech.c hw_logitech.h \
			hw_slinke.c hw_slinke.h \
			hw_irman.c hw_irman.h \
			hw_creative.c hw_creative.h \
			hw_silitek.c hw_silitek.h \
		        hw_dsp.c \
			receive.c receive.h \
			transmit.c transmit.h \
			serial.c serial.h


lircmd_SOURCES = lircmd.c
lircmd_LDADD = 

bin_PROGRAMS = irrecord

irrecord_SOURCES = irrecord.c dump_config.c dump_config.h \
		config_file.c config_file.h hardware.h \
		hw-types.c hw-types.h ir_remote.c ir_remote.h

irrecord_LDADD = hw_default.o receive.o transmit.o  
irrecord_DEPENDENCIES = hw_default.o receive.o transmit.o
EXTRA_irrecord_SOURCES = hw_default.c hw_default.h \
			hw_pixelview.c hw_pixelview.h \
			hw_pinsys.c hw_pinsys.h \
			hw_logitech.c hw_logitech.h \
			hw_slinke.c hw_slinke.h \
			hw_irman.c hw_irman.h \
			hw_creative.c hw_creative.h \
			hw_silitek.c hw_silitek.h \
			hw_dsp.c \
			receive.c receive.h \
			transmit.c transmit.h \
			serial.c serial.h


EXTRA_PROGRAMS = lircd.simsend lircd.simrec slinke
noinst_PROGRAMS = 
lircd_simsend_SOURCES = lircd.c ir_remote.c config_file.c \
		lircd.h ir_remote.h config_file.h \
		hw-types.c hw-types.h hardware.h \
		hw_default.c hw_default.h \
		receive.c receive.h \
		transmit.c transmit.h

lircd_simsend_CFLAGS = -DSIM_SEND
lircd_simrec_SOURCES = lircd.c ir_remote.c config_file.c \
		lircd.h ir_remote.h config_file.h \
		hw-types.c hw-types.h hardware.h \
		hw_default.c hw_default.h \
		receive.c receive.h \
		transmit.c transmit.h

lircd_simrec_CFLAGS = -DSIM_REC

slinke_SOURCES = slinke.c slinke.h config_file.c config_file.h ir_remote.h \
		dump_config.c dump_config.h


CLEANFILES = *~
subdir = daemons
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = $(top_builddir)/config.h
CONFIG_CLEAN_FILES =
EXTRA_PROGRAMS = lircd.simsend$(EXEEXT) lircd.simrec$(EXEEXT) \
	slinke$(EXEEXT)
bin_PROGRAMS = irrecord$(EXEEXT)
noinst_PROGRAMS = 
sbin_PROGRAMS = lircd$(EXEEXT) lircmd$(EXEEXT)
PROGRAMS = $(bin_PROGRAMS) $(noinst_PROGRAMS) $(sbin_PROGRAMS)

am_irrecord_OBJECTS = irrecord.$(OBJEXT) dump_config.$(OBJEXT) \
	config_file.$(OBJEXT) hw-types.$(OBJEXT) ir_remote.$(OBJEXT)
irrecord_OBJECTS = $(am_irrecord_OBJECTS)
irrecord_LDFLAGS =
am_lircd_OBJECTS = lircd.$(OBJEXT) ir_remote.$(OBJEXT) \
	config_file.$(OBJEXT) hw-types.$(OBJEXT)
lircd_OBJECTS = $(am_lircd_OBJECTS)
lircd_LDFLAGS =
am_lircd_simrec_OBJECTS = lircd_simrec-lircd.$(OBJEXT) \
	lircd_simrec-ir_remote.$(OBJEXT) \
	lircd_simrec-config_file.$(OBJEXT) \
	lircd_simrec-hw-types.$(OBJEXT) \
	lircd_simrec-hw_default.$(OBJEXT) \
	lircd_simrec-receive.$(OBJEXT) lircd_simrec-transmit.$(OBJEXT)
lircd_simrec_OBJECTS = $(am_lircd_simrec_OBJECTS)
lircd_simrec_LDADD = $(LDADD)
lircd_simrec_DEPENDENCIES =
lircd_simrec_LDFLAGS =
am_lircd_simsend_OBJECTS = lircd_simsend-lircd.$(OBJEXT) \
	lircd_simsend-ir_remote.$(OBJEXT) \
	lircd_simsend-config_file.$(OBJEXT) \
	lircd_simsend-hw-types.$(OBJEXT) \
	lircd_simsend-hw_default.$(OBJEXT) \
	lircd_simsend-receive.$(OBJEXT) \
	lircd_simsend-transmit.$(OBJEXT)
lircd_simsend_OBJECTS = $(am_lircd_simsend_OBJECTS)
lircd_simsend_LDADD = $(LDADD)
lircd_simsend_DEPENDENCIES =
lircd_simsend_LDFLAGS =
am_lircmd_OBJECTS = lircmd.$(OBJEXT)
lircmd_OBJECTS = $(am_lircmd_OBJECTS)
lircmd_DEPENDENCIES =
lircmd_LDFLAGS =
am_slinke_OBJECTS = slinke.$(OBJEXT) config_file.$(OBJEXT) \
	dump_config.$(OBJEXT)
slinke_OBJECTS = $(am_slinke_OBJECTS)
slinke_LDADD = $(LDADD)
slinke_DEPENDENCIES =
slinke_LDFLAGS =

DEFS = -DHAVE_CONFIG_H
DEFAULT_INCLUDES =  -I. -I$(srcdir) -I$(top_builddir) -I$(top_srcdir)
CPPFLAGS = $(PROG_CFLAGS)
#LDFLAGS = 
LIBS = $(LDLIBS) -lc
depcomp = $(SHELL) $(top_srcdir)/depcomp
DEP_FILES = $(DEPDIR)/config_file.Po \
	$(DEPDIR)/dump_config.Po $(DEPDIR)/hw-types.Po \
	$(DEPDIR)/hw_creative.Po $(DEPDIR)/hw_default.Po \
	$(DEPDIR)/hw_dsp.Po $(DEPDIR)/hw_irman.Po \
	$(DEPDIR)/hw_logitech.Po $(DEPDIR)/hw_pinsys.Po \
	$(DEPDIR)/hw_pixelview.Po $(DEPDIR)/hw_silitek.Po \
	$(DEPDIR)/hw_slinke.Po $(DEPDIR)/ir_remote.Po \
	$(DEPDIR)/irrecord.Po $(DEPDIR)/lircd.Po \
	$(DEPDIR)/lircd_simrec-config_file.Po \
	$(DEPDIR)/lircd_simrec-hw-types.Po \
	$(DEPDIR)/lircd_simrec-hw_default.Po \
	$(DEPDIR)/lircd_simrec-ir_remote.Po \
	$(DEPDIR)/lircd_simrec-lircd.Po \
	$(DEPDIR)/lircd_simrec-receive.Po \
	$(DEPDIR)/lircd_simrec-transmit.Po \
	$(DEPDIR)/lircd_simsend-config_file.Po \
	$(DEPDIR)/lircd_simsend-hw-types.Po \
	$(DEPDIR)/lircd_simsend-hw_default.Po \
	$(DEPDIR)/lircd_simsend-ir_remote.Po \
	$(DEPDIR)/lircd_simsend-lircd.Po \
	$(DEPDIR)/lircd_simsend-receive.Po \
	$(DEPDIR)/lircd_simsend-transmit.Po \
	$(DEPDIR)/lircmd.Po $(DEPDIR)/receive.Po \
	$(DEPDIR)/serial.Po $(DEPDIR)/slinke.Po \
	$(DEPDIR)/transmit.Po
COMPILE = $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) \
	$(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
LTCOMPILE = $(LIBTOOL) --mode=compile $(CC) $(DEFS) $(DEFAULT_INCLUDES) \
	$(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
CCLD = $(CC)
LINK = $(LIBTOOL) --mode=link $(CCLD) $(AM_CFLAGS) $(CFLAGS) \
	$(AM_LDFLAGS) $(LDFLAGS) -o $@
CFLAGS = -O2 -g -Wall
DIST_SOURCES = $(irrecord_SOURCES) $(EXTRA_irrecord_SOURCES) \
	$(lircd_SOURCES) $(EXTRA_lircd_SOURCES) $(lircd_simrec_SOURCES) \
	$(lircd_simsend_SOURCES) $(lircmd_SOURCES) $(slinke_SOURCES)
DIST_COMMON = Makefile.am Makefile.in
SOURCES = $(irrecord_SOURCES) $(EXTRA_irrecord_SOURCES) $(lircd_SOURCES) $(EXTRA_lircd_SOURCES) $(lircd_simrec_SOURCES) $(lircd_simsend_SOURCES) $(lircmd_SOURCES) $(slinke_SOURCES)

all: all-am

.SUFFIXES:
.SUFFIXES: .c .lo .o .obj

mostlyclean-libtool:
	-rm -f *.lo

clean-libtool:
	-rm -rf .libs _libs

distclean-libtool:
	-rm -f libtool
$(srcdir)/Makefile.in:  Makefile.am  $(top_srcdir)/configure.in $(ACLOCAL_M4)
	cd $(top_srcdir) && \
	  $(AUTOMAKE) --gnu  daemons/Makefile
Makefile:  $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) && \
	  CONFIG_HEADERS= CONFIG_LINKS= \
	  CONFIG_FILES=$(subdir)/$@ $(SHELL) ./config.status
install-binPROGRAMS: $(bin_PROGRAMS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	@list='$(bin_PROGRAMS)'; for p in $$list; do \
	  p1=`echo $$p|sed 's/$(EXEEXT)$$//'`; \
	  if test -f $$p \
	     || test -f $$p1 \
	  ; then \
	    f=`echo $$p1|sed '$(transform);s/$$/$(EXEEXT)/'`; \
	   echo " $(INSTALL_PROGRAM_ENV) $(LIBTOOL) --mode=install $(INSTALL_PROGRAM) $$p $(DESTDIR)$(bindir)/$$f"; \
	   $(INSTALL_PROGRAM_ENV) $(LIBTOOL) --mode=install $(INSTALL_PROGRAM) $$p $(DESTDIR)$(bindir)/$$f; \
	  else :; fi; \
	done

uninstall-binPROGRAMS:
	@$(NORMAL_UNINSTALL)
	@list='$(bin_PROGRAMS)'; for p in $$list; do \
	  f=`echo $$p|sed 's/$(EXEEXT)$$//;$(transform);s/$$/$(EXEEXT)/'`; \
	  echo " rm -f $(DESTDIR)$(bindir)/$$f"; \
	  rm -f $(DESTDIR)$(bindir)/$$f; \
	done

clean-binPROGRAMS:
	-test -z "$(bin_PROGRAMS)" || rm -f $(bin_PROGRAMS)

clean-noinstPROGRAMS:
	-test -z "$(noinst_PROGRAMS)" || rm -f $(noinst_PROGRAMS)
install-sbinPROGRAMS: $(sbin_PROGRAMS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(sbindir)
	@list='$(sbin_PROGRAMS)'; for p in $$list; do \
	  p1=`echo $$p|sed 's/$(EXEEXT)$$//'`; \
	  if test -f $$p \
	     || test -f $$p1 \
	  ; then \
	    f=`echo $$p1|sed '$(transform);s/$$/$(EXEEXT)/'`; \
	   echo " $(INSTALL_PROGRAM_ENV) $(LIBTOOL) --mode=install $(INSTALL_PROGRAM) $$p $(DESTDIR)$(sbindir)/$$f"; \
	   $(INSTALL_PROGRAM_ENV) $(LIBTOOL) --mode=install $(INSTALL_PROGRAM) $$p $(DESTDIR)$(sbindir)/$$f; \
	  else :; fi; \
	done

uninstall-sbinPROGRAMS:
	@$(NORMAL_UNINSTALL)
	@list='$(sbin_PROGRAMS)'; for p in $$list; do \
	  f=`echo $$p|sed 's/$(EXEEXT)$$//;$(transform);s/$$/$(EXEEXT)/'`; \
	  echo " rm -f $(DESTDIR)$(sbindir)/$$f"; \
	  rm -f $(DESTDIR)$(sbindir)/$$f; \
	done

clean-sbinPROGRAMS:
	-test -z "$(sbin_PROGRAMS)" || rm -f $(sbin_PROGRAMS)
irrecord$(EXEEXT): $(irrecord_OBJECTS) $(irrecord_DEPENDENCIES) 
	@rm -f irrecord$(EXEEXT)
	$(LINK) $(irrecord_LDFLAGS) $(irrecord_OBJECTS) $(irrecord_LDADD) $(LIBS)
lircd$(EXEEXT): $(lircd_OBJECTS) $(lircd_DEPENDENCIES) 
	@rm -f lircd$(EXEEXT)
	$(LINK) $(lircd_LDFLAGS) $(lircd_OBJECTS) $(lircd_LDADD) $(LIBS)
lircd_simrec-lircd.$(OBJEXT): lircd.c
lircd_simrec-ir_remote.$(OBJEXT): ir_remote.c
lircd_simrec-config_file.$(OBJEXT): config_file.c
lircd_simrec-hw-types.$(OBJEXT): hw-types.c
lircd_simrec-hw_default.$(OBJEXT): hw_default.c
lircd_simrec-receive.$(OBJEXT): receive.c
lircd_simrec-transmit.$(OBJEXT): transmit.c
lircd.simrec$(EXEEXT): $(lircd_simrec_OBJECTS) $(lircd_simrec_DEPENDENCIES) 
	@rm -f lircd.simrec$(EXEEXT)
	$(LINK) $(lircd_simrec_LDFLAGS) $(lircd_simrec_OBJECTS) $(lircd_simrec_LDADD) $(LIBS)
lircd_simsend-lircd.$(OBJEXT): lircd.c
lircd_simsend-ir_remote.$(OBJEXT): ir_remote.c
lircd_simsend-config_file.$(OBJEXT): config_file.c
lircd_simsend-hw-types.$(OBJEXT): hw-types.c
lircd_simsend-hw_default.$(OBJEXT): hw_default.c
lircd_simsend-receive.$(OBJEXT): receive.c
lircd_simsend-transmit.$(OBJEXT): transmit.c
lircd.simsend$(EXEEXT): $(lircd_simsend_OBJECTS) $(lircd_simsend_DEPENDENCIES) 
	@rm -f lircd.simsend$(EXEEXT)
	$(LINK) $(lircd_simsend_LDFLAGS) $(lircd_simsend_OBJECTS) $(lircd_simsend_LDADD) $(LIBS)
lircmd$(EXEEXT): $(lircmd_OBJECTS) $(lircmd_DEPENDENCIES) 
	@rm -f lircmd$(EXEEXT)
	$(LINK) $(lircmd_LDFLAGS) $(lircmd_OBJECTS) $(lircmd_LDADD) $(LIBS)
slinke$(EXEEXT): $(slinke_OBJECTS) $(slinke_DEPENDENCIES) 
	@rm -f slinke$(EXEEXT)
	$(LINK) $(slinke_LDFLAGS) $(slinke_OBJECTS) $(slinke_LDADD) $(LIBS)

mostlyclean-compile:
	-rm -f *.$(OBJEXT) core *.core

distclean-compile:
	-rm -f *.tab.c

include $(DEPDIR)/config_file.Po
include $(DEPDIR)/dump_config.Po
include $(DEPDIR)/hw-types.Po
include $(DEPDIR)/hw_creative.Po
include $(DEPDIR)/hw_default.Po
include $(DEPDIR)/hw_dsp.Po
include $(DEPDIR)/hw_irman.Po
include $(DEPDIR)/hw_logitech.Po
include $(DEPDIR)/hw_pinsys.Po
include $(DEPDIR)/hw_pixelview.Po
include $(DEPDIR)/hw_silitek.Po
include $(DEPDIR)/hw_slinke.Po
include $(DEPDIR)/ir_remote.Po
include $(DEPDIR)/irrecord.Po
include $(DEPDIR)/lircd.Po
include $(DEPDIR)/lircd_simrec-config_file.Po
include $(DEPDIR)/lircd_simrec-hw-types.Po
include $(DEPDIR)/lircd_simrec-hw_default.Po
include $(DEPDIR)/lircd_simrec-ir_remote.Po
include $(DEPDIR)/lircd_simrec-lircd.Po
include $(DEPDIR)/lircd_simrec-receive.Po
include $(DEPDIR)/lircd_simrec-transmit.Po
include $(DEPDIR)/lircd_simsend-config_file.Po
include $(DEPDIR)/lircd_simsend-hw-types.Po
include $(DEPDIR)/lircd_simsend-hw_default.Po
include $(DEPDIR)/lircd_simsend-ir_remote.Po
include $(DEPDIR)/lircd_simsend-lircd.Po
include $(DEPDIR)/lircd_simsend-receive.Po
include $(DEPDIR)/lircd_simsend-transmit.Po
include $(DEPDIR)/lircmd.Po
include $(DEPDIR)/receive.Po
include $(DEPDIR)/serial.Po
include $(DEPDIR)/slinke.Po
include $(DEPDIR)/transmit.Po

distclean-depend:
	-rm -rf $(DEPDIR)

.c.o:
	source='$<' object='$@' libtool=no \
	depfile='$(DEPDIR)/$*.Po' tmpdepfile='$(DEPDIR)/$*.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(COMPILE) -c `test -f $< || echo '$(srcdir)/'`$<

.c.obj:
	source='$<' object='$@' libtool=no \
	depfile='$(DEPDIR)/$*.Po' tmpdepfile='$(DEPDIR)/$*.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(COMPILE) -c `cygpath -w $<`

.c.lo:
	source='$<' object='$@' libtool=yes \
	depfile='$(DEPDIR)/$*.Plo' tmpdepfile='$(DEPDIR)/$*.TPlo' \
	$(CCDEPMODE) $(depcomp) \
	$(LTCOMPILE) -c -o $@ `test -f $< || echo '$(srcdir)/'`$<

lircd_simrec-lircd.o: lircd.c
	source='lircd.c' object='lircd_simrec-lircd.o' libtool=no \
	depfile='$(DEPDIR)/lircd_simrec-lircd.Po' tmpdepfile='$(DEPDIR)/lircd_simrec-lircd.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simrec_CFLAGS) $(CFLAGS) -c -o lircd_simrec-lircd.o `test -f lircd.c || echo '$(srcdir)/'`lircd.c

lircd_simrec-lircd.obj: lircd.c
	source='lircd.c' object='lircd_simrec-lircd.obj' libtool=no \
	depfile='$(DEPDIR)/lircd_simrec-lircd.Po' tmpdepfile='$(DEPDIR)/lircd_simrec-lircd.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simrec_CFLAGS) $(CFLAGS) -c -o lircd_simrec-lircd.obj `cygpath -w lircd.c`

lircd_simrec-lircd.lo: lircd.c
	source='lircd.c' object='lircd_simrec-lircd.lo' libtool=yes \
	depfile='$(DEPDIR)/lircd_simrec-lircd.Plo' tmpdepfile='$(DEPDIR)/lircd_simrec-lircd.TPlo' \
	$(CCDEPMODE) $(depcomp) \
	$(LIBTOOL) --mode=compile $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simrec_CFLAGS) $(CFLAGS) -c -o lircd_simrec-lircd.lo `test -f lircd.c || echo '$(srcdir)/'`lircd.c

lircd_simrec-ir_remote.o: ir_remote.c
	source='ir_remote.c' object='lircd_simrec-ir_remote.o' libtool=no \
	depfile='$(DEPDIR)/lircd_simrec-ir_remote.Po' tmpdepfile='$(DEPDIR)/lircd_simrec-ir_remote.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simrec_CFLAGS) $(CFLAGS) -c -o lircd_simrec-ir_remote.o `test -f ir_remote.c || echo '$(srcdir)/'`ir_remote.c

lircd_simrec-ir_remote.obj: ir_remote.c
	source='ir_remote.c' object='lircd_simrec-ir_remote.obj' libtool=no \
	depfile='$(DEPDIR)/lircd_simrec-ir_remote.Po' tmpdepfile='$(DEPDIR)/lircd_simrec-ir_remote.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simrec_CFLAGS) $(CFLAGS) -c -o lircd_simrec-ir_remote.obj `cygpath -w ir_remote.c`

lircd_simrec-ir_remote.lo: ir_remote.c
	source='ir_remote.c' object='lircd_simrec-ir_remote.lo' libtool=yes \
	depfile='$(DEPDIR)/lircd_simrec-ir_remote.Plo' tmpdepfile='$(DEPDIR)/lircd_simrec-ir_remote.TPlo' \
	$(CCDEPMODE) $(depcomp) \
	$(LIBTOOL) --mode=compile $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simrec_CFLAGS) $(CFLAGS) -c -o lircd_simrec-ir_remote.lo `test -f ir_remote.c || echo '$(srcdir)/'`ir_remote.c

lircd_simrec-config_file.o: config_file.c
	source='config_file.c' object='lircd_simrec-config_file.o' libtool=no \
	depfile='$(DEPDIR)/lircd_simrec-config_file.Po' tmpdepfile='$(DEPDIR)/lircd_simrec-config_file.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simrec_CFLAGS) $(CFLAGS) -c -o lircd_simrec-config_file.o `test -f config_file.c || echo '$(srcdir)/'`config_file.c

lircd_simrec-config_file.obj: config_file.c
	source='config_file.c' object='lircd_simrec-config_file.obj' libtool=no \
	depfile='$(DEPDIR)/lircd_simrec-config_file.Po' tmpdepfile='$(DEPDIR)/lircd_simrec-config_file.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simrec_CFLAGS) $(CFLAGS) -c -o lircd_simrec-config_file.obj `cygpath -w config_file.c`

lircd_simrec-config_file.lo: config_file.c
	source='config_file.c' object='lircd_simrec-config_file.lo' libtool=yes \
	depfile='$(DEPDIR)/lircd_simrec-config_file.Plo' tmpdepfile='$(DEPDIR)/lircd_simrec-config_file.TPlo' \
	$(CCDEPMODE) $(depcomp) \
	$(LIBTOOL) --mode=compile $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simrec_CFLAGS) $(CFLAGS) -c -o lircd_simrec-config_file.lo `test -f config_file.c || echo '$(srcdir)/'`config_file.c

lircd_simrec-hw-types.o: hw-types.c
	source='hw-types.c' object='lircd_simrec-hw-types.o' libtool=no \
	depfile='$(DEPDIR)/lircd_simrec-hw-types.Po' tmpdepfile='$(DEPDIR)/lircd_simrec-hw-types.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simrec_CFLAGS) $(CFLAGS) -c -o lircd_simrec-hw-types.o `test -f hw-types.c || echo '$(srcdir)/'`hw-types.c

lircd_simrec-hw-types.obj: hw-types.c
	source='hw-types.c' object='lircd_simrec-hw-types.obj' libtool=no \
	depfile='$(DEPDIR)/lircd_simrec-hw-types.Po' tmpdepfile='$(DEPDIR)/lircd_simrec-hw-types.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simrec_CFLAGS) $(CFLAGS) -c -o lircd_simrec-hw-types.obj `cygpath -w hw-types.c`

lircd_simrec-hw-types.lo: hw-types.c
	source='hw-types.c' object='lircd_simrec-hw-types.lo' libtool=yes \
	depfile='$(DEPDIR)/lircd_simrec-hw-types.Plo' tmpdepfile='$(DEPDIR)/lircd_simrec-hw-types.TPlo' \
	$(CCDEPMODE) $(depcomp) \
	$(LIBTOOL) --mode=compile $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simrec_CFLAGS) $(CFLAGS) -c -o lircd_simrec-hw-types.lo `test -f hw-types.c || echo '$(srcdir)/'`hw-types.c

lircd_simrec-hw_default.o: hw_default.c
	source='hw_default.c' object='lircd_simrec-hw_default.o' libtool=no \
	depfile='$(DEPDIR)/lircd_simrec-hw_default.Po' tmpdepfile='$(DEPDIR)/lircd_simrec-hw_default.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simrec_CFLAGS) $(CFLAGS) -c -o lircd_simrec-hw_default.o `test -f hw_default.c || echo '$(srcdir)/'`hw_default.c

lircd_simrec-hw_default.obj: hw_default.c
	source='hw_default.c' object='lircd_simrec-hw_default.obj' libtool=no \
	depfile='$(DEPDIR)/lircd_simrec-hw_default.Po' tmpdepfile='$(DEPDIR)/lircd_simrec-hw_default.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simrec_CFLAGS) $(CFLAGS) -c -o lircd_simrec-hw_default.obj `cygpath -w hw_default.c`

lircd_simrec-hw_default.lo: hw_default.c
	source='hw_default.c' object='lircd_simrec-hw_default.lo' libtool=yes \
	depfile='$(DEPDIR)/lircd_simrec-hw_default.Plo' tmpdepfile='$(DEPDIR)/lircd_simrec-hw_default.TPlo' \
	$(CCDEPMODE) $(depcomp) \
	$(LIBTOOL) --mode=compile $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simrec_CFLAGS) $(CFLAGS) -c -o lircd_simrec-hw_default.lo `test -f hw_default.c || echo '$(srcdir)/'`hw_default.c

lircd_simrec-receive.o: receive.c
	source='receive.c' object='lircd_simrec-receive.o' libtool=no \
	depfile='$(DEPDIR)/lircd_simrec-receive.Po' tmpdepfile='$(DEPDIR)/lircd_simrec-receive.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simrec_CFLAGS) $(CFLAGS) -c -o lircd_simrec-receive.o `test -f receive.c || echo '$(srcdir)/'`receive.c

lircd_simrec-receive.obj: receive.c
	source='receive.c' object='lircd_simrec-receive.obj' libtool=no \
	depfile='$(DEPDIR)/lircd_simrec-receive.Po' tmpdepfile='$(DEPDIR)/lircd_simrec-receive.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simrec_CFLAGS) $(CFLAGS) -c -o lircd_simrec-receive.obj `cygpath -w receive.c`

lircd_simrec-receive.lo: receive.c
	source='receive.c' object='lircd_simrec-receive.lo' libtool=yes \
	depfile='$(DEPDIR)/lircd_simrec-receive.Plo' tmpdepfile='$(DEPDIR)/lircd_simrec-receive.TPlo' \
	$(CCDEPMODE) $(depcomp) \
	$(LIBTOOL) --mode=compile $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simrec_CFLAGS) $(CFLAGS) -c -o lircd_simrec-receive.lo `test -f receive.c || echo '$(srcdir)/'`receive.c

lircd_simrec-transmit.o: transmit.c
	source='transmit.c' object='lircd_simrec-transmit.o' libtool=no \
	depfile='$(DEPDIR)/lircd_simrec-transmit.Po' tmpdepfile='$(DEPDIR)/lircd_simrec-transmit.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simrec_CFLAGS) $(CFLAGS) -c -o lircd_simrec-transmit.o `test -f transmit.c || echo '$(srcdir)/'`transmit.c

lircd_simrec-transmit.obj: transmit.c
	source='transmit.c' object='lircd_simrec-transmit.obj' libtool=no \
	depfile='$(DEPDIR)/lircd_simrec-transmit.Po' tmpdepfile='$(DEPDIR)/lircd_simrec-transmit.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simrec_CFLAGS) $(CFLAGS) -c -o lircd_simrec-transmit.obj `cygpath -w transmit.c`

lircd_simrec-transmit.lo: transmit.c
	source='transmit.c' object='lircd_simrec-transmit.lo' libtool=yes \
	depfile='$(DEPDIR)/lircd_simrec-transmit.Plo' tmpdepfile='$(DEPDIR)/lircd_simrec-transmit.TPlo' \
	$(CCDEPMODE) $(depcomp) \
	$(LIBTOOL) --mode=compile $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simrec_CFLAGS) $(CFLAGS) -c -o lircd_simrec-transmit.lo `test -f transmit.c || echo '$(srcdir)/'`transmit.c

lircd_simsend-lircd.o: lircd.c
	source='lircd.c' object='lircd_simsend-lircd.o' libtool=no \
	depfile='$(DEPDIR)/lircd_simsend-lircd.Po' tmpdepfile='$(DEPDIR)/lircd_simsend-lircd.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simsend_CFLAGS) $(CFLAGS) -c -o lircd_simsend-lircd.o `test -f lircd.c || echo '$(srcdir)/'`lircd.c

lircd_simsend-lircd.obj: lircd.c
	source='lircd.c' object='lircd_simsend-lircd.obj' libtool=no \
	depfile='$(DEPDIR)/lircd_simsend-lircd.Po' tmpdepfile='$(DEPDIR)/lircd_simsend-lircd.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simsend_CFLAGS) $(CFLAGS) -c -o lircd_simsend-lircd.obj `cygpath -w lircd.c`

lircd_simsend-lircd.lo: lircd.c
	source='lircd.c' object='lircd_simsend-lircd.lo' libtool=yes \
	depfile='$(DEPDIR)/lircd_simsend-lircd.Plo' tmpdepfile='$(DEPDIR)/lircd_simsend-lircd.TPlo' \
	$(CCDEPMODE) $(depcomp) \
	$(LIBTOOL) --mode=compile $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simsend_CFLAGS) $(CFLAGS) -c -o lircd_simsend-lircd.lo `test -f lircd.c || echo '$(srcdir)/'`lircd.c

lircd_simsend-ir_remote.o: ir_remote.c
	source='ir_remote.c' object='lircd_simsend-ir_remote.o' libtool=no \
	depfile='$(DEPDIR)/lircd_simsend-ir_remote.Po' tmpdepfile='$(DEPDIR)/lircd_simsend-ir_remote.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simsend_CFLAGS) $(CFLAGS) -c -o lircd_simsend-ir_remote.o `test -f ir_remote.c || echo '$(srcdir)/'`ir_remote.c

lircd_simsend-ir_remote.obj: ir_remote.c
	source='ir_remote.c' object='lircd_simsend-ir_remote.obj' libtool=no \
	depfile='$(DEPDIR)/lircd_simsend-ir_remote.Po' tmpdepfile='$(DEPDIR)/lircd_simsend-ir_remote.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simsend_CFLAGS) $(CFLAGS) -c -o lircd_simsend-ir_remote.obj `cygpath -w ir_remote.c`

lircd_simsend-ir_remote.lo: ir_remote.c
	source='ir_remote.c' object='lircd_simsend-ir_remote.lo' libtool=yes \
	depfile='$(DEPDIR)/lircd_simsend-ir_remote.Plo' tmpdepfile='$(DEPDIR)/lircd_simsend-ir_remote.TPlo' \
	$(CCDEPMODE) $(depcomp) \
	$(LIBTOOL) --mode=compile $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simsend_CFLAGS) $(CFLAGS) -c -o lircd_simsend-ir_remote.lo `test -f ir_remote.c || echo '$(srcdir)/'`ir_remote.c

lircd_simsend-config_file.o: config_file.c
	source='config_file.c' object='lircd_simsend-config_file.o' libtool=no \
	depfile='$(DEPDIR)/lircd_simsend-config_file.Po' tmpdepfile='$(DEPDIR)/lircd_simsend-config_file.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simsend_CFLAGS) $(CFLAGS) -c -o lircd_simsend-config_file.o `test -f config_file.c || echo '$(srcdir)/'`config_file.c

lircd_simsend-config_file.obj: config_file.c
	source='config_file.c' object='lircd_simsend-config_file.obj' libtool=no \
	depfile='$(DEPDIR)/lircd_simsend-config_file.Po' tmpdepfile='$(DEPDIR)/lircd_simsend-config_file.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simsend_CFLAGS) $(CFLAGS) -c -o lircd_simsend-config_file.obj `cygpath -w config_file.c`

lircd_simsend-config_file.lo: config_file.c
	source='config_file.c' object='lircd_simsend-config_file.lo' libtool=yes \
	depfile='$(DEPDIR)/lircd_simsend-config_file.Plo' tmpdepfile='$(DEPDIR)/lircd_simsend-config_file.TPlo' \
	$(CCDEPMODE) $(depcomp) \
	$(LIBTOOL) --mode=compile $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simsend_CFLAGS) $(CFLAGS) -c -o lircd_simsend-config_file.lo `test -f config_file.c || echo '$(srcdir)/'`config_file.c

lircd_simsend-hw-types.o: hw-types.c
	source='hw-types.c' object='lircd_simsend-hw-types.o' libtool=no \
	depfile='$(DEPDIR)/lircd_simsend-hw-types.Po' tmpdepfile='$(DEPDIR)/lircd_simsend-hw-types.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simsend_CFLAGS) $(CFLAGS) -c -o lircd_simsend-hw-types.o `test -f hw-types.c || echo '$(srcdir)/'`hw-types.c

lircd_simsend-hw-types.obj: hw-types.c
	source='hw-types.c' object='lircd_simsend-hw-types.obj' libtool=no \
	depfile='$(DEPDIR)/lircd_simsend-hw-types.Po' tmpdepfile='$(DEPDIR)/lircd_simsend-hw-types.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simsend_CFLAGS) $(CFLAGS) -c -o lircd_simsend-hw-types.obj `cygpath -w hw-types.c`

lircd_simsend-hw-types.lo: hw-types.c
	source='hw-types.c' object='lircd_simsend-hw-types.lo' libtool=yes \
	depfile='$(DEPDIR)/lircd_simsend-hw-types.Plo' tmpdepfile='$(DEPDIR)/lircd_simsend-hw-types.TPlo' \
	$(CCDEPMODE) $(depcomp) \
	$(LIBTOOL) --mode=compile $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simsend_CFLAGS) $(CFLAGS) -c -o lircd_simsend-hw-types.lo `test -f hw-types.c || echo '$(srcdir)/'`hw-types.c

lircd_simsend-hw_default.o: hw_default.c
	source='hw_default.c' object='lircd_simsend-hw_default.o' libtool=no \
	depfile='$(DEPDIR)/lircd_simsend-hw_default.Po' tmpdepfile='$(DEPDIR)/lircd_simsend-hw_default.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simsend_CFLAGS) $(CFLAGS) -c -o lircd_simsend-hw_default.o `test -f hw_default.c || echo '$(srcdir)/'`hw_default.c

lircd_simsend-hw_default.obj: hw_default.c
	source='hw_default.c' object='lircd_simsend-hw_default.obj' libtool=no \
	depfile='$(DEPDIR)/lircd_simsend-hw_default.Po' tmpdepfile='$(DEPDIR)/lircd_simsend-hw_default.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simsend_CFLAGS) $(CFLAGS) -c -o lircd_simsend-hw_default.obj `cygpath -w hw_default.c`

lircd_simsend-hw_default.lo: hw_default.c
	source='hw_default.c' object='lircd_simsend-hw_default.lo' libtool=yes \
	depfile='$(DEPDIR)/lircd_simsend-hw_default.Plo' tmpdepfile='$(DEPDIR)/lircd_simsend-hw_default.TPlo' \
	$(CCDEPMODE) $(depcomp) \
	$(LIBTOOL) --mode=compile $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simsend_CFLAGS) $(CFLAGS) -c -o lircd_simsend-hw_default.lo `test -f hw_default.c || echo '$(srcdir)/'`hw_default.c

lircd_simsend-receive.o: receive.c
	source='receive.c' object='lircd_simsend-receive.o' libtool=no \
	depfile='$(DEPDIR)/lircd_simsend-receive.Po' tmpdepfile='$(DEPDIR)/lircd_simsend-receive.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simsend_CFLAGS) $(CFLAGS) -c -o lircd_simsend-receive.o `test -f receive.c || echo '$(srcdir)/'`receive.c

lircd_simsend-receive.obj: receive.c
	source='receive.c' object='lircd_simsend-receive.obj' libtool=no \
	depfile='$(DEPDIR)/lircd_simsend-receive.Po' tmpdepfile='$(DEPDIR)/lircd_simsend-receive.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simsend_CFLAGS) $(CFLAGS) -c -o lircd_simsend-receive.obj `cygpath -w receive.c`

lircd_simsend-receive.lo: receive.c
	source='receive.c' object='lircd_simsend-receive.lo' libtool=yes \
	depfile='$(DEPDIR)/lircd_simsend-receive.Plo' tmpdepfile='$(DEPDIR)/lircd_simsend-receive.TPlo' \
	$(CCDEPMODE) $(depcomp) \
	$(LIBTOOL) --mode=compile $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simsend_CFLAGS) $(CFLAGS) -c -o lircd_simsend-receive.lo `test -f receive.c || echo '$(srcdir)/'`receive.c

lircd_simsend-transmit.o: transmit.c
	source='transmit.c' object='lircd_simsend-transmit.o' libtool=no \
	depfile='$(DEPDIR)/lircd_simsend-transmit.Po' tmpdepfile='$(DEPDIR)/lircd_simsend-transmit.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simsend_CFLAGS) $(CFLAGS) -c -o lircd_simsend-transmit.o `test -f transmit.c || echo '$(srcdir)/'`transmit.c

lircd_simsend-transmit.obj: transmit.c
	source='transmit.c' object='lircd_simsend-transmit.obj' libtool=no \
	depfile='$(DEPDIR)/lircd_simsend-transmit.Po' tmpdepfile='$(DEPDIR)/lircd_simsend-transmit.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simsend_CFLAGS) $(CFLAGS) -c -o lircd_simsend-transmit.obj `cygpath -w transmit.c`

lircd_simsend-transmit.lo: transmit.c
	source='transmit.c' object='lircd_simsend-transmit.lo' libtool=yes \
	depfile='$(DEPDIR)/lircd_simsend-transmit.Plo' tmpdepfile='$(DEPDIR)/lircd_simsend-transmit.TPlo' \
	$(CCDEPMODE) $(depcomp) \
	$(LIBTOOL) --mode=compile $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(lircd_simsend_CFLAGS) $(CFLAGS) -c -o lircd_simsend-transmit.lo `test -f transmit.c || echo '$(srcdir)/'`transmit.c
CCDEPMODE = depmode=gcc3
uninstall-info-am:

tags: TAGS

ID: $(HEADERS) $(SOURCES) $(LISP) $(TAGS_FILES)
	list='$(SOURCES) $(HEADERS) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	mkid -fID $$unique $(LISP)

TAGS:  $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES) \
		$(TAGS_FILES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SOURCES) $(HEADERS) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	test -z "$(ETAGS_ARGS)$$unique$(LISP)$$tags" \
	  || etags $(ETAGS_ARGS) $$tags  $$unique $(LISP)

GTAGS:
	here=`CDPATH=: && cd $(top_builddir) && pwd` \
	  && cd $(top_srcdir) \
	  && gtags -i $(GTAGS_ARGS) $$here

distclean-tags:
	-rm -f TAGS ID GTAGS GRTAGS GSYMS GPATH

DISTFILES = $(DIST_COMMON) $(DIST_SOURCES) $(TEXINFOS) $(EXTRA_DIST)

top_distdir = ..
distdir = $(top_distdir)/$(PACKAGE)-$(VERSION)

distdir: $(DISTFILES)
	@for file in $(DISTFILES); do \
	  if test -f $$file; then d=.; else d=$(srcdir); fi; \
	  dir=`echo "$$file" | sed -e 's,/[^/]*$$,,'`; \
	  if test "$$dir" != "$$file" && test "$$dir" != "."; then \
	    $(mkinstalldirs) "$(distdir)/$$dir"; \
	  fi; \
	  if test -d $$d/$$file; then \
	    cp -pR $$d/$$file $(distdir) \
	    || exit 1; \
	  else \
	    test -f $(distdir)/$$file \
	    || cp -p $$d/$$file $(distdir)/$$file \
	    || exit 1; \
	  fi; \
	done
check-am: all-am
check: check-am
all-am: Makefile $(PROGRAMS)

installdirs:
	$(mkinstalldirs) $(DESTDIR)$(bindir) $(DESTDIR)$(sbindir)

install: install-am
install-exec: install-exec-am
install-data: install-data-am
uninstall: uninstall-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am

installcheck: installcheck-am
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) INSTALL_PROGRAM="$(INSTALL_STRIP_PROGRAM)" \
	  `test -z '$(STRIP)' || \
	    echo "INSTALL_PROGRAM_ENV=STRIPPROG='$(STRIP)'"` install
mostlyclean-generic:

clean-generic:
	-test -z "$(CLEANFILES)" || rm -f $(CLEANFILES)

distclean-generic:
	-rm -f Makefile $(CONFIG_CLEAN_FILES) stamp-h stamp-h[0-9]*

maintainer-clean-generic:
	@echo "This command is intended for maintainers to use"
	@echo "it deletes files that may require special tools to rebuild."
clean: clean-am

clean-am: clean-binPROGRAMS clean-generic clean-libtool \
	clean-noinstPROGRAMS clean-sbinPROGRAMS mostlyclean-am

distclean: distclean-am

distclean-am: clean-am distclean-compile distclean-depend \
	distclean-generic distclean-libtool distclean-tags

dvi: dvi-am

dvi-am:

info: info-am

info-am:

install-data-am: install-data-local

install-exec-am: install-binPROGRAMS install-exec-local \
	install-sbinPROGRAMS

install-info: install-info-am

install-man:

installcheck-am:

maintainer-clean: maintainer-clean-am

maintainer-clean-am: distclean-am maintainer-clean-generic

mostlyclean: mostlyclean-am

mostlyclean-am: mostlyclean-compile mostlyclean-generic \
	mostlyclean-libtool

uninstall-am: uninstall-binPROGRAMS uninstall-info-am uninstall-local \
	uninstall-sbinPROGRAMS

.PHONY: GTAGS all all-am check check-am clean clean-binPROGRAMS \
	clean-generic clean-libtool clean-noinstPROGRAMS \
	clean-sbinPROGRAMS distclean distclean-compile distclean-depend \
	distclean-generic distclean-libtool distclean-tags distdir dvi \
	dvi-am info info-am install install-am install-binPROGRAMS \
	install-data install-data-am install-data-local install-exec \
	install-exec-am install-exec-local install-info install-info-am \
	install-man install-sbinPROGRAMS install-strip installcheck \
	installcheck-am installdirs maintainer-clean \
	maintainer-clean-generic mostlyclean mostlyclean-compile \
	mostlyclean-generic mostlyclean-libtool tags uninstall \
	uninstall-am uninstall-binPROGRAMS uninstall-info-am \
	uninstall-local uninstall-sbinPROGRAMS


install-exec-local: mkdev
uninstall-local: rmdev

install-data-local:
	test "$(lircd_conf)" = ""  || test -e $(DESTDIR)$(sysconfdir)/lircd.conf  || ($(mkinstalldirs) $(DESTDIR)$(sysconfdir) && $(INSTALL_DATA) $(top_srcdir)/remotes/$(lircd_conf) $(DESTDIR)$(sysconfdir)/lircd.conf)
	test "$(lircmd_conf)" = "" || test -e $(DESTDIR)$(sysconfdir)/lircmd.conf || ($(mkinstalldirs) $(DESTDIR)$(sysconfdir) && $(INSTALL_DATA) $(top_srcdir)/remotes/$(lircmd_conf) $(DESTDIR)$(sysconfdir)/lircmd.conf)

mkdev:
	test -e $(DESTDIR)$(devdir)/lircd || ($(mkinstalldirs) $(DESTDIR)$(devdir) && /usr/bin/mknod $(DESTDIR)$(devdir)/lircd p)
	test -e $(DESTDIR)$(devdir)/lircm || ($(mkinstalldirs) $(DESTDIR)$(devdir) && /usr/bin/mknod $(DESTDIR)$(devdir)/lircm p)

rmdev:
	-$(RM) $(DESTDIR)$(devdir)/lircd
	-$(RM) $(DESTDIR)$(devdir)/lircm
# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
