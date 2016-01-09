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

srcdir = ../../tools
top_srcdir = ../..
VPATH = ../../tools
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
moduledir = /lib/modules/2.4.21-rmk1-curs1.4.3/misc
#vga_progs = smode2
x_progs = 

#EXTRA_PROGRAMS = smode2 xmode2 irxevent
EXTRA_PROGRAMS = irxevent
#bin_PROGRAMS = irw irpty irexec ircat mode2 rc smode2 
bin_PROGRAMS = irw irpty irexec ircat mode2 rc
AM_CPPFLAGS = 

irw_SOURCES = irw.c
irpty_SOURCES = irpty.c
irpty_DEPENDENCIES = liblirc_client.la
irpty_LDADD = liblirc_client.la -lutil
irexec_SOURCES = irexec.c
irexec_DEPENDENCIES = liblirc_client.la
irexec_LDADD = liblirc_client.la 
ircat_SOURCES = ircat.c
ircat_DEPENDENCIES = liblirc_client.la
ircat_LDADD = liblirc_client.la
mode2_SOURCES = mode2.c
rc_SOURCES = rc.c

smode2_SOURCES = smode2.c
smode2_LDADD = -lvga -lvgagl

irxevent_SOURCES = irxevent.c
irxevent_LDADD =   -lX11  liblirc_client.la
irxevent_DEPENDENCIES = liblirc_client.la
xmode2_SOURCES = xmode2.c
xmode2_LDADD =   -lX11 

lircincludedir = $(includedir)/lirc

lib_LTLIBRARIES = liblirc_client.la
liblirc_client_la_SOURCES = lirc_client.c lirc_client.h
lircinclude_HEADERS = lirc_client.h

CLEANFILES = *~
subdir = tools
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = $(top_builddir)/config.h
CONFIG_CLEAN_FILES =
LTLIBRARIES = $(lib_LTLIBRARIES)

liblirc_client_la_LDFLAGS =
liblirc_client_la_LIBADD =
am_liblirc_client_la_OBJECTS = lirc_client.lo
liblirc_client_la_OBJECTS = $(am_liblirc_client_la_OBJECTS)
#EXTRA_PROGRAMS = smode2$(EXEEXT) xmode2$(EXEEXT) irxevent$(EXEEXT)
EXTRA_PROGRAMS = irxevent$(EXEEXT)
#bin_PROGRAMS = irw$(EXEEXT) irpty$(EXEEXT) irexec$(EXEEXT) \
#	ircat$(EXEEXT) mode2$(EXEEXT) rc$(EXEEXT) smode2 
bin_PROGRAMS = irw$(EXEEXT) irpty$(EXEEXT) irexec$(EXEEXT) \
	ircat$(EXEEXT) mode2$(EXEEXT) rc$(EXEEXT)
PROGRAMS = $(bin_PROGRAMS)

am_ircat_OBJECTS = ircat.$(OBJEXT)
ircat_OBJECTS = $(am_ircat_OBJECTS)
ircat_LDFLAGS =
am_irexec_OBJECTS = irexec.$(OBJEXT)
irexec_OBJECTS = $(am_irexec_OBJECTS)
irexec_LDFLAGS =
am_irpty_OBJECTS = irpty.$(OBJEXT)
irpty_OBJECTS = $(am_irpty_OBJECTS)
irpty_LDFLAGS =
am_irw_OBJECTS = irw.$(OBJEXT)
irw_OBJECTS = $(am_irw_OBJECTS)
irw_LDADD = $(LDADD)
irw_DEPENDENCIES =
irw_LDFLAGS =
am_irxevent_OBJECTS = irxevent.$(OBJEXT)
irxevent_OBJECTS = $(am_irxevent_OBJECTS)
irxevent_LDFLAGS =
am_mode2_OBJECTS = mode2.$(OBJEXT)
mode2_OBJECTS = $(am_mode2_OBJECTS)
mode2_LDADD = $(LDADD)
mode2_DEPENDENCIES =
mode2_LDFLAGS =
am_rc_OBJECTS = rc.$(OBJEXT)
rc_OBJECTS = $(am_rc_OBJECTS)
rc_LDADD = $(LDADD)
rc_DEPENDENCIES =
rc_LDFLAGS =
am_smode2_OBJECTS = smode2.$(OBJEXT)
smode2_OBJECTS = $(am_smode2_OBJECTS)
smode2_DEPENDENCIES =
smode2_LDFLAGS =
am_xmode2_OBJECTS = xmode2.$(OBJEXT)
xmode2_OBJECTS = $(am_xmode2_OBJECTS)
xmode2_DEPENDENCIES =
xmode2_LDFLAGS =

DEFS = -DHAVE_CONFIG_H
DEFAULT_INCLUDES =  -I. -I$(srcdir) -I$(top_builddir) -I$(top_srcdir)
CPPFLAGS = $(PROG_CFLAGS)
#LDFLAGS = 
LIBS = $(LDLIBS) -lc
depcomp = $(SHELL) $(top_srcdir)/depcomp
DEP_FILES = $(DEPDIR)/ircat.Po $(DEPDIR)/irexec.Po \
	$(DEPDIR)/irpty.Po $(DEPDIR)/irw.Po \
	$(DEPDIR)/irxevent.Po $(DEPDIR)/lirc_client.Plo \
	$(DEPDIR)/mode2.Po $(DEPDIR)/rc.Po \
	$(DEPDIR)/smode2.Po $(DEPDIR)/xmode2.Po
COMPILE = $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) \
	$(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
LTCOMPILE = $(LIBTOOL) --mode=compile $(CC) $(DEFS) $(DEFAULT_INCLUDES) \
	$(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
CCLD = $(CC)
LINK = $(LIBTOOL) --mode=link $(CCLD) $(AM_CFLAGS) $(CFLAGS) \
	$(AM_LDFLAGS) $(LDFLAGS) -o $@
CFLAGS = -O2 -g -Wall
DIST_SOURCES = $(liblirc_client_la_SOURCES) $(ircat_SOURCES) \
	$(irexec_SOURCES) $(irpty_SOURCES) $(irw_SOURCES) \
	$(irxevent_SOURCES) $(mode2_SOURCES) $(rc_SOURCES) \
	$(smode2_SOURCES) $(xmode2_SOURCES)
HEADERS = $(lircinclude_HEADERS)

DIST_COMMON = $(lircinclude_HEADERS) Makefile.am Makefile.in
#SOURCES = $(liblirc_client_la_SOURCES) $(ircat_SOURCES) $(irexec_SOURCES) $(irpty_SOURCES) $(irw_SOURCES) $(irxevent_SOURCES) $(mode2_SOURCES) $(rc_SOURCES) $(smode2_SOURCES) $(xmode2_SOURCES)
SOURCES = $(liblirc_client_la_SOURCES) $(ircat_SOURCES) $(irexec_SOURCES) $(irpty_SOURCES) $(irw_SOURCES) $(irxevent_SOURCES) $(mode2_SOURCES) $(rc_SOURCES) $(xmode2_SOURCES)

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
	  $(AUTOMAKE) --gnu  tools/Makefile
Makefile:  $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) && \
	  CONFIG_HEADERS= CONFIG_LINKS= \
	  CONFIG_FILES=$(subdir)/$@ $(SHELL) ./config.status
install-libLTLIBRARIES: $(lib_LTLIBRARIES)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)
	@list='$(lib_LTLIBRARIES)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo " $(LIBTOOL) --mode=install $(INSTALL) $(INSTALL_STRIP_FLAG) $$p $(DESTDIR)$(libdir)/$$p"; \
	    $(LIBTOOL) --mode=install $(INSTALL) $(INSTALL_STRIP_FLAG) $$p $(DESTDIR)$(libdir)/$$p; \
	  else :; fi; \
	done

uninstall-libLTLIBRARIES:
	@$(NORMAL_UNINSTALL)
	@list='$(lib_LTLIBRARIES)'; for p in $$list; do \
	  echo " $(LIBTOOL) --mode=uninstall rm -f $(DESTDIR)$(libdir)/$$p"; \
	  $(LIBTOOL) --mode=uninstall rm -f $(DESTDIR)$(libdir)/$$p; \
	done

clean-libLTLIBRARIES:
	-test -z "$(lib_LTLIBRARIES)" || rm -f $(lib_LTLIBRARIES)
liblirc_client.la: $(liblirc_client_la_OBJECTS) $(liblirc_client_la_DEPENDENCIES) 
	$(LINK) -rpath $(libdir) $(liblirc_client_la_LDFLAGS) $(liblirc_client_la_OBJECTS) $(liblirc_client_la_LIBADD) $(LIBS)
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
ircat$(EXEEXT): $(ircat_OBJECTS) $(ircat_DEPENDENCIES) 
	@rm -f ircat$(EXEEXT)
	$(LINK) $(ircat_LDFLAGS) $(ircat_OBJECTS) $(ircat_LDADD) $(LIBS)
irexec$(EXEEXT): $(irexec_OBJECTS) $(irexec_DEPENDENCIES) 
	@rm -f irexec$(EXEEXT)
	$(LINK) $(irexec_LDFLAGS) $(irexec_OBJECTS) $(irexec_LDADD) $(LIBS)
irpty$(EXEEXT): $(irpty_OBJECTS) $(irpty_DEPENDENCIES) 
	@rm -f irpty$(EXEEXT)
	$(LINK) $(irpty_LDFLAGS) $(irpty_OBJECTS) $(irpty_LDADD) $(LIBS)
irw$(EXEEXT): $(irw_OBJECTS) $(irw_DEPENDENCIES) 
	@rm -f irw$(EXEEXT)
	$(LINK) $(irw_LDFLAGS) $(irw_OBJECTS) $(irw_LDADD) $(LIBS)
irxevent$(EXEEXT): $(irxevent_OBJECTS) $(irxevent_DEPENDENCIES) 
	@rm -f irxevent$(EXEEXT)
	$(LINK) $(irxevent_LDFLAGS) $(irxevent_OBJECTS) $(irxevent_LDADD) $(LIBS)
mode2$(EXEEXT): $(mode2_OBJECTS) $(mode2_DEPENDENCIES) 
	@rm -f mode2$(EXEEXT)
	$(LINK) $(mode2_LDFLAGS) $(mode2_OBJECTS) $(mode2_LDADD) $(LIBS)
rc$(EXEEXT): $(rc_OBJECTS) $(rc_DEPENDENCIES) 
	@rm -f rc$(EXEEXT)
	$(LINK) $(rc_LDFLAGS) $(rc_OBJECTS) $(rc_LDADD) $(LIBS)
smode2$(EXEEXT): $(smode2_OBJECTS) $(smode2_DEPENDENCIES) 
	@rm -f smode2$(EXEEXT)
	$(LINK) $(smode2_LDFLAGS) $(smode2_OBJECTS) $(smode2_LDADD) $(LIBS)
xmode2$(EXEEXT): $(xmode2_OBJECTS) $(xmode2_DEPENDENCIES) 
	@rm -f xmode2$(EXEEXT)
	$(LINK) $(xmode2_LDFLAGS) $(xmode2_OBJECTS) $(xmode2_LDADD) $(LIBS)

mostlyclean-compile:
	-rm -f *.$(OBJEXT) core *.core

distclean-compile:
	-rm -f *.tab.c

include $(DEPDIR)/ircat.Po
include $(DEPDIR)/irexec.Po
include $(DEPDIR)/irpty.Po
include $(DEPDIR)/irw.Po
include $(DEPDIR)/irxevent.Po
include $(DEPDIR)/lirc_client.Plo
include $(DEPDIR)/mode2.Po
include $(DEPDIR)/rc.Po
#include $(DEPDIR)/smode2.Po
include $(DEPDIR)/xmode2.Po

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
install-lircincludeHEADERS: $(lircinclude_HEADERS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(lircincludedir)
	@list='$(lircinclude_HEADERS)'; for p in $$list; do \
	  if test -f "$$p"; then d=; else d="$(srcdir)/"; fi; \
	  f="`echo $$p | sed -e 's|^.*/||'`"; \
	  echo " $(INSTALL_HEADER) $$d$$p $(DESTDIR)$(lircincludedir)/$$f"; \
	  $(INSTALL_HEADER) $$d$$p $(DESTDIR)$(lircincludedir)/$$f; \
	done

uninstall-lircincludeHEADERS:
	@$(NORMAL_UNINSTALL)
	@list='$(lircinclude_HEADERS)'; for p in $$list; do \
	  f="`echo $$p | sed -e 's|^.*/||'`"; \
	  echo " rm -f $(DESTDIR)$(lircincludedir)/$$f"; \
	  rm -f $(DESTDIR)$(lircincludedir)/$$f; \
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
all-am: Makefile $(LTLIBRARIES) $(PROGRAMS) $(HEADERS)
install-binPROGRAMS: install-libLTLIBRARIES


installdirs:
	$(mkinstalldirs) $(DESTDIR)$(libdir) $(DESTDIR)$(bindir) $(DESTDIR)$(lircincludedir)

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

clean-am: clean-binPROGRAMS clean-generic clean-libLTLIBRARIES \
	clean-libtool mostlyclean-am

distclean: distclean-am

distclean-am: clean-am distclean-compile distclean-depend \
	distclean-generic distclean-libtool distclean-tags

dvi: dvi-am

dvi-am:

info: info-am

info-am:

install-data-am: install-lircincludeHEADERS

install-exec-am: install-binPROGRAMS install-libLTLIBRARIES

install-info: install-info-am

install-man:

installcheck-am:

maintainer-clean: maintainer-clean-am

maintainer-clean-am: distclean-am maintainer-clean-generic

mostlyclean: mostlyclean-am

mostlyclean-am: mostlyclean-compile mostlyclean-generic \
	mostlyclean-libtool

uninstall-am: uninstall-binPROGRAMS uninstall-info-am \
	uninstall-libLTLIBRARIES uninstall-lircincludeHEADERS

.PHONY: GTAGS all all-am check check-am clean clean-binPROGRAMS \
	clean-generic clean-libLTLIBRARIES clean-libtool distclean \
	distclean-compile distclean-depend distclean-generic \
	distclean-libtool distclean-tags distdir dvi dvi-am info \
	info-am install install-am install-binPROGRAMS install-data \
	install-data-am install-exec install-exec-am install-info \
	install-info-am install-libLTLIBRARIES \
	install-lircincludeHEADERS install-man install-strip \
	installcheck installcheck-am installdirs maintainer-clean \
	maintainer-clean-generic mostlyclean mostlyclean-compile \
	mostlyclean-generic mostlyclean-libtool tags uninstall \
	uninstall-am uninstall-binPROGRAMS uninstall-info-am \
	uninstall-libLTLIBRARIES uninstall-lircincludeHEADERS

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
