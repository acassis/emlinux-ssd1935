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

srcdir = ../../../drivers/lirc_i2c
top_srcdir = ../../..
VPATH = ../../../drivers/lirc_i2c
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
top_builddir = ../..

ACLOCAL = ${SHELL} /projects/CirrusEP93xx/emlinux-ep93xx/apps/gpl/lirc-0.6.6/missing --run aclocal
AUTOCONF = ${SHELL} /projects/CirrusEP93xx/emlinux-ep93xx/apps/gpl/lirc-0.6.6/missing --run autoconf
AUTOMAKE = ${SHELL} /projects/CirrusEP93xx/emlinux-ep93xx/apps/gpl/lirc-0.6.6/missing --run automake
AUTOHEADER = ${SHELL} /projects/CirrusEP93xx/emlinux-ep93xx/apps/gpl/lirc-0.6.6/missing --run autoheader

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
AMTAR = ${SHELL} /projects/CirrusEP93xx/emlinux-ep93xx/apps/gpl/lirc-0.6.6/missing --run tar
AS = @AS@
AWK = gawk
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
RANLIB = ranlib
STRIP = strip
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
install_sh = /projects/CirrusEP93xx/emlinux-ep93xx/apps/gpl/lirc-0.6.6/install-sh
irman_lib = 
irtty = 
kernelcc = /bin/arm-linux-gcc
kerneldir = /projects/CirrusEP93xx/emlinux-ep93xx/kernel/
lirc_driver = lirc_edb9315
lirc_major = 61
lircd_conf = 
lircmd_conf = 
maintmode_daemons_extra = 
mknod = /usr/bin/mknod
moduledir = /lib/modules/2.4.22/misc
vga_progs = smode2
x_progs = 

# some magic for using linux kernel settings
# when compiling module(s)
CC = /bin/arm-linux-gcc

EXTRA_DIST = 

EXTRA_PROGRAMS = automake_dummy
automake_dummy_SOURCES = lirc_i2c.c

module_DATA = lirc_i2c.o

# where the kernel sources are located
KERNEL_LOCATION = /projects/CirrusEP93xx/emlinux-ep93xx/kernel/
MI_OBJS = $(module_DATA)
EXTRA_CFLAGS = $(DEFS) $(DEFAULT_INCLUDES)

# the spaces above are necessary because automake otherwise will complain
# and even worse, it will delete the line
CLEANFILES = $(module_DATA) *~ \#*\#
subdir = drivers/lirc_i2c
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = $(top_builddir)/config.h
CONFIG_CLEAN_FILES =
EXTRA_PROGRAMS = automake_dummy$(EXEEXT)
am_automake_dummy_OBJECTS = lirc_i2c.$(OBJEXT)
automake_dummy_OBJECTS = $(am_automake_dummy_OBJECTS)
automake_dummy_LDADD = $(LDADD)
automake_dummy_DEPENDENCIES =
automake_dummy_LDFLAGS =

DEFS = -DHAVE_CONFIG_H
DEFAULT_INCLUDES =  -I. -I$(srcdir) -I$(top_builddir)
CPPFLAGS = 
LDFLAGS = 
LIBS = 
depcomp = $(SHELL) $(top_srcdir)/depcomp
DEP_FILES = $(DEPDIR)/lirc_i2c.Po
COMPILE = $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) \
	$(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
LTCOMPILE = $(LIBTOOL) --mode=compile $(CC) $(DEFS) $(DEFAULT_INCLUDES) \
	$(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
CCLD = $(CC)
LINK = $(LIBTOOL) --mode=link $(CCLD) $(AM_CFLAGS) $(CFLAGS) \
	$(AM_LDFLAGS) $(LDFLAGS) -o $@
CFLAGS = -O2 -g -Wall
DIST_SOURCES = $(automake_dummy_SOURCES)
DATA = $(module_DATA)

DIST_COMMON = Makefile.am Makefile.in
SOURCES = $(automake_dummy_SOURCES)

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
	  $(AUTOMAKE) --gnu  drivers/lirc_i2c/Makefile
Makefile:  $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) && \
	  CONFIG_HEADERS= CONFIG_LINKS= \
	  CONFIG_FILES=$(subdir)/$@ $(SHELL) ./config.status
automake_dummy$(EXEEXT): $(automake_dummy_OBJECTS) $(automake_dummy_DEPENDENCIES) 
	@rm -f automake_dummy$(EXEEXT)
	$(LINK) $(automake_dummy_LDFLAGS) $(automake_dummy_OBJECTS) $(automake_dummy_LDADD) $(LIBS)

mostlyclean-compile:
	-rm -f *.$(OBJEXT) core *.core

distclean-compile:
	-rm -f *.tab.c

include $(DEPDIR)/lirc_i2c.Po

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
CCDEPMODE = depmode=gcc3
uninstall-info-am:
install-moduleDATA: $(module_DATA)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(moduledir)
	@list='$(module_DATA)'; for p in $$list; do \
	  if test -f "$$p"; then d=; else d="$(srcdir)/"; fi; \
	  f="`echo $$p | sed -e 's|^.*/||'`"; \
	  echo " $(INSTALL_DATA) $$d$$p $(DESTDIR)$(moduledir)/$$f"; \
	  $(INSTALL_DATA) $$d$$p $(DESTDIR)$(moduledir)/$$f; \
	done

uninstall-moduleDATA:
	@$(NORMAL_UNINSTALL)
	@list='$(module_DATA)'; for p in $$list; do \
	  f="`echo $$p | sed -e 's|^.*/||'`"; \
	  echo " rm -f $(DESTDIR)$(moduledir)/$$f"; \
	  rm -f $(DESTDIR)$(moduledir)/$$f; \
	done

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

top_distdir = ../..
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
all-am: Makefile $(DATA)

installdirs:
	$(mkinstalldirs) $(DESTDIR)$(moduledir)

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

clean-am: clean-generic clean-libtool mostlyclean-am

distclean: distclean-am

distclean-am: clean-am distclean-compile distclean-depend \
	distclean-generic distclean-libtool distclean-tags

dvi: dvi-am

dvi-am:

info: info-am

info-am:

install-data-am: install-moduleDATA

install-exec-am:

install-info: install-info-am

install-man:

installcheck-am:

maintainer-clean: maintainer-clean-am

maintainer-clean-am: distclean-am maintainer-clean-generic

mostlyclean: mostlyclean-am

mostlyclean-am: mostlyclean-compile mostlyclean-generic \
	mostlyclean-libtool

uninstall-am: uninstall-info-am uninstall-moduleDATA

.PHONY: GTAGS all all-am check check-am clean clean-generic \
	clean-libtool distclean distclean-compile distclean-depend \
	distclean-generic distclean-libtool distclean-tags distdir dvi \
	dvi-am info info-am install install-am install-data \
	install-data-am install-exec install-exec-am install-info \
	install-info-am install-man install-moduleDATA install-strip \
	installcheck installcheck-am installdirs maintainer-clean \
	maintainer-clean-generic mostlyclean mostlyclean-compile \
	mostlyclean-generic mostlyclean-libtool tags uninstall \
	uninstall-am uninstall-info-am uninstall-moduleDATA


$(module_DATA): ${automake_dummy_SOURCES} $(top_srcdir)/config.h ../lirc.h
  ifndef MAKING_MODULES
	DIR=`pwd`; (cd $(KERNEL_LOCATION); make SUBDIRS=$$DIR obj-m=$(module_DATA) modules)
  endif

include $(KERNEL_LOCATION)/Rules.make
# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
