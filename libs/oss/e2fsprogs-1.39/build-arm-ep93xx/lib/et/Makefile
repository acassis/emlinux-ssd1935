#
# Makefile for lib/et
#

srcdir = ../../../lib/et
top_srcdir = ../../..
VPATH = ../../../lib/et
top_builddir = ../..
my_dir = lib/et
INSTALL = /usr/bin/ginstall -c

DEP_MAKEFILE = $(DEP_LIB_MAKEFILES)


# Beginning of file MCONFIG

all::

check::

SHELL = /bin/sh

COMPRESS_EXT = gz bz2 bz Z

prefix = /usr
root_prefix = 
exec_prefix = ${prefix}
root_bindir = ${root_prefix}/bin
root_sbindir = ${root_prefix}/sbin
root_libdir = ${root_prefix}/lib
bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
libdir = ${exec_prefix}/lib
datadir= ${prefix}/share
localedir = $(datadir)/locale
root_sysconfdir= ${root_prefix}/etc
includedir = ${prefix}/include
mandir = /usr/share/man
man1dir = $(mandir)/man1
man3dir = $(mandir)/man3
man5dir = $(mandir)/man5
man8dir = $(mandir)/man8
infodir = ${prefix}/info
datadir = ${prefix}/share



CC = /opt/toolchain_arm_ep93xx/bin/arm-linux-uclibc-gcc
BUILD_CC = gcc
DEFS = -DLOCALEDIR=\"$(localedir)\" -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DENABLE_HTREE=1 -DENABLE_SWAPFS=1 -DPACKAGE=\"e2fsprogs\" -DVERSION=\"0.14.1\" -DHAVE_LONG_LONG=1 -DHAVE_LONG_DOUBLE=1 -DHAVE_WCHAR_T=1 -DHAVE_WINT_T=1 -DHAVE_INTTYPES_H_WITH_UINTMAX=1 -DHAVE_STDINT_H_WITH_UINTMAX=1 -DHAVE_INTMAX_T=1 -DHAVE_POSIX_PRINTF=1 -DHAVE_ALLOCA_H=1 -DHAVE_ALLOCA=1 -DHAVE_STDLIB_H=1 -DHAVE_UNISTD_H=1 -DHAVE_GETPAGESIZE=1 -DINTDIV0_RAISES_SIGFPE=0 -DHAVE_UNSIGNED_LONG_LONG=1 -DHAVE_UINTMAX_T=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_STDINT_H=1 -DHAVE_LIMITS_H=1 -DHAVE_LOCALE_H=1 -DHAVE_NL_TYPES_H=1 -DHAVE_MALLOC_H=1 -DHAVE_STDDEF_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_UNISTD_H=1 -DHAVE_SYS_PARAM_H=1 -DHAVE_ASPRINTF=1 -DHAVE_FWPRINTF=1 -DHAVE_GETCWD=1 -DHAVE_GETEGID=1 -DHAVE_GETEUID=1 -DHAVE_GETGID=1 -DHAVE_GETUID=1 -DHAVE_MEMPCPY=1 -DHAVE_MUNMAP=1 -DHAVE_PUTENV=1 -DHAVE_SETENV=1 -DHAVE_SETLOCALE=1 -DHAVE_SNPRINTF=1 -DHAVE_STPCPY=1 -DHAVE_STRCASECMP=1 -DHAVE_STRDUP=1 -DHAVE_STRTOUL=1 -DHAVE_TSEARCH=1 -DHAVE_WCSLEN=1 -DHAVE___FSETLOCKING=1 -DHAVE_DECL__SNPRINTF=0 -DHAVE_DECL__SNWPRINTF=0 -DHAVE_DECL_FEOF_UNLOCKED=1 -DHAVE_DECL_FGETS_UNLOCKED=0 -DHAVE_DECL_GETC_UNLOCKED=1 -DHAVE_LANGINFO_CODESET=1 -DHAVE_LC_MESSAGES=1 -DENABLE_NLS=1 -DHAVE_STDLIB_H=1 -DHAVE_UNISTD_H=1 -DHAVE_STDARG_H=1 -DHAVE_STDINT_H=1 -DHAVE_ERRNO_H=1 -DHAVE_MALLOC_H=1 -DHAVE_MNTENT_H=1 -DHAVE_PATHS_H=1 -DHAVE_DIRENT_H=1 -DHAVE_GETOPT_H=1 -DHAVE_SETJMP_H=1 -DHAVE_SIGNAL_H=1 -DHAVE_TERMIOS_H=1 -DHAVE_LINUX_FD_H=1 -DHAVE_LINUX_MAJOR_H=1 -DHAVE_SYS_IOCTL_H=1 -DHAVE_SYS_PRCTL_H=1 -DHAVE_SYS_QUEUE_H=1 -DHAVE_SYS_SOCKET_H=1 -DHAVE_SYS_SYSMACROS_H=1 -DHAVE_SYS_TIME_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_WAIT_H=1 -DHAVE_SYS_RESOURCE_H=1 -DHAVE_NETINET_IN_H=1 -DHAVE_SYS_MOUNT_H=1 -DHAVE_NET_IF_H=1 -DHAVE_VPRINTF=1 -DHAVE_RECLEN_DIRENT=1 -DHAVE_TYPE_SSIZE_T=1 -DHAVE_LSEEK64_PROTOTYPE=1 -DSIZEOF_SHORT=2 -DSIZEOF_INT=4 -DSIZEOF_LONG=4 -DSIZEOF_LONG_LONG=8 -DHAVE_INTTYPES_H=1 -DHAVE_INTPTR_T=1 -DHAVE_GETRUSAGE=1 -DHAVE_LLSEEK=1 -DHAVE_LSEEK64=1 -DHAVE_OPEN64=1 -DHAVE_FSTAT64=1 -DHAVE_STRTOULL=1 -DHAVE_STRCASECMP=1 -DHAVE_SRANDOM=1 -DHAVE_FCHOWN=1 -DHAVE_MALLINFO=1 -DHAVE_FDATASYNC=1 -DHAVE_STRNLEN=1 -DHAVE_STRPTIME=1 -DHAVE_SYSCONF=1 -DHAVE_PATHCONF=1 -DHAVE_MEMALIGN=1 -DHAVE_VALLOC=1 -DHAVE_PRCTL=1 -DHAVE_DLOPEN=1 -DHAVE_EXT2_IOCTLS=1 
CFLAGS = -g -O2
CPPFLAGS =  -I$(top_builddir)/lib -I$(top_srcdir)/lib 
INTL_FLAGS = -I$(top_builddir)/intl -I$(top_srcdir)/intl
ALL_CFLAGS = $(CPPFLAGS) $(DEFS) $(USE_WFLAGS) $(CFLAGS) $(XTRA_CFLAGS) \
	$(INTL_FLAGS) $(LINUX_INCLUDE) 
LDFLAGS = 
ALL_LDFLAGS = $(LDFLAGS)
BUILD_CFLAGS = 
BUILD_LDFLAGS = 
RM = /usr/bin/rm
LN = /usr/bin/ln
LN_S = ln -s
MV = /usr/bin/mv
CP = /usr/bin/cp
CHMOD = /usr/bin/chmod
AR = /opt/toolchain_arm_ep93xx/bin/arm-linux-uclibc-ar
AWK = gawk
SED = /usr/bin/sed
PERL = /usr/bin/perl
RANLIB = /opt/toolchain_arm_ep93xx/bin/arm-linux-uclibc-ranlib
STRIP = /opt/toolchain_arm_ep93xx/bin/arm-linux-uclibc-strip
LD = $(PURE) /opt/toolchain_arm_ep93xx/bin/arm-linux-uclibc-gcc
ARUPD = $(AR) r
ARGEN = $(AR) rc
LDCONFIG = :
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL}
MKINSTALLDIRS = $(top_builddir)/../config/mkinstalldirs

#
# Library definitions
#
DEVMAPPER_LIBS = 
STATIC_DEVMAPPER_LIBS = 

LIB = $(top_builddir)/lib
LIBSS = $(LIB)/libss.so
LIBCOM_ERR = $(LIB)/libcom_err.so
LIBE2P = $(LIB)/libe2p.so
LIBEXT2FS = $(LIB)/libext2fs.so
LIBUUID = $(LIB)/libuuid.so  
LIBBLKID = $(LIB)/libblkid.so $(DEVMAPPER_LIBS)
LIBINTL = ${top_builddir}/intl/libintl.a 
DEPLIBUUID = $(LIB)/libuuid.so

STATIC_LIBSS = $(LIB)/libss.a
STATIC_LIBCOM_ERR = $(LIB)/libcom_err.a
STATIC_LIBE2P = $(LIB)/libe2p.a
STATIC_LIBEXT2FS = $(LIB)/libext2fs.a
STATIC_LIBUUID = $(LIB)/libuuid.a  
STATIC_LIBBLKID = $(LIB)/libblkid.a $(STATIC_DEVMAPPER_LIBS)
DEPSTATIC_LIBUUID = $(LIB)/libuuid.a

PROFILED_LIBSS = $(LIB)/libss
PROFILED_LIBCOM_ERR = $(LIB)/libcom_err
PROFILED_LIBE2P = $(LIB)/libe2p
PROFILED_LIBEXT2FS = $(LIB)/libext2fs
PROFILED_LIBUUID = $(LIB)/libuuid  
PROFILED_LIBBLKID = $(LIB)/libblkid
DEPPROFILED_LIBUUID = $(LIB)/libuuid

# An include directive pointing to a directory holding enough linux-like
# include files to satisfy some programs here
LINUX_INCLUDE=

#
# A fast substitution command for fixing up man pages, shell scripts, etc.
#
SUBST_CONF=$(top_builddir)/util/subst.conf
SUBSTITUTE= $(top_builddir)/util/subst -f $(SUBST_CONF)
SUBSTITUTE_UPTIME= $(top_builddir)/util/subst -t -f $(SUBST_CONF)
DEP_SUBSTITUTE= $(top_builddir)/util/subst $(SUBST_CONF)

$(top_builddir)/util/subst:
	cd $(top_builddir)/util ; $(MAKE) subst

#
# Warning flags
#
# Run make gcc-wall to do a build with warning messages.
#
#
WFLAGS=		-ansi -D_POSIX_SOURCE -pedantic $(WFLAGS_EXTRA) \
			-Wall -W -Wwrite-strings -Wpointer-arith \
			-Wcast-qual -Wcast-align  \
			-Wstrict-prototypes -Wmissing-prototypes \
			-Wformat-security  -Wformat-nonliteral \
			-Wmissing-format-attribute \
			-Wnested-externs -Winline -DNO_INLINE_FUNCS -Wshadow \
			-UENABLE_NLS

gcc-wall-new:
	(make USE_WFLAGS="$(WFLAGS)" > /dev/null) 2>&1 | sed -f $(top_srcdir)/util/gcc-wall-cleanup 

gcc-wall:
	make clean > /dev/null
	make gcc-wall-new

#
# Installation user and groups
#
BINGRP=		bin
BINOWN=		bin
BINMODE=	555
INCGRP=		bin
INCOWN=		bin
INCMODE=	444
LIBOWN=		bin
LIBGRP=		bin
LIBMODE=	444
MANGRP=		bin
MANOWN=		bin
MANMODE=	444

#
# Autoconf magic...
#

DEP_LIB_MAKEFILES = $(top_srcdir)/lib/Makefile.elf-lib  \
	$(top_srcdir)/lib/Makefile.bsd-lib $(top_srcdir)/lib/Makefile.darwin-lib \
	$(top_srcdir)/lib/Makefile.solaris-lib $(top_srcdir)/lib/Makefile.checker \
	$(top_srcdir)/lib/Makefile.profile

$(top_builddir)/config.status: $(top_srcdir)/configure
	cd $(top_builddir); ./config.status --recheck

$(top_builddir)/MCONFIG: $(top_srcdir)/MCONFIG.in $(top_builddir)/config.status
	cd $(top_builddir); CONFIG_FILES=MCONFIG ./config.status

$(top_builddir)/lib/substitute_sh: $(top_srcdir)/lib/substitute_sh.in \
		$(top_builddir)/config.status
	cd $(top_builddir); CONFIG_FILES=lib/substitute_sh ./config.status

$(top_builddir)/util/subst.conf: $(top_srcdir)/util/subst.conf.in \
		$(top_builddir)/config.status
	cd $(top_builddir); CONFIG_FILES=util/subst.conf ./config.status

Makefile: $(srcdir)/Makefile.in $(top_builddir)/MCONFIG \
		$(DEP_MAKEFILE) $(top_builddir)/config.status
	cd $(top_builddir); CONFIG_FILES=$(my_dir)/Makefile ./config.status

#$(top_srcdir)/configure: $(top_srcdir)/configure.in
#	cd $(top_srcdir) && autoconf

#
# Make depend magic...
#

.depend: Makefile $(SRCS) $(top_srcdir)/depfix.sed $(top_srcdir)/wordwrap.pl
	if test -n "$(SRCS)" ; then \
		$(CC) -M $(ALL_CFLAGS) $(SRCS) | \
			$(SED) -f $(top_srcdir)/depfix.sed \
			    -e 's; $(srcdir)/; $$(srcdir)/;g' \
			    -e 's; $(top_srcdir)/; $$(top_srcdir)/;g' \
			    -e 's; $(top_builddir)/; $$(top_builddir)/;g' \
			    -e 's; \./; ;g' \
			    -e '/^#/d' \
			    -e '/^ *\\$$/d' | \
			$(PERL) $(top_srcdir)/wordwrap.pl > .depend; \
	else :; fi

depend:: .depend
	if test -n "$(SRCS)" ; then \
		sed -e '/^# +++ Dependency line eater +++/,$$d' \
			< $(srcdir)/Makefile.in | cat - .depend \
			> $(srcdir)/Makefile.in.new; \
	if cmp -s $(srcdir)/Makefile.in $(srcdir)/Makefile.in.new ; then \
		$(RM) $(srcdir)/Makefile.in.new ; \
	else \
		$(MV) $(srcdir)/Makefile.in $(srcdir)/Makefile.in.old; \
		$(MV) $(srcdir)/Makefile.in.new $(srcdir)/Makefile.in; \
	fi ; else :; fi

# End of file MCONFIG

all:: compile_et com_err.pc

OBJS= error_message.o et_name.o init_et.o com_err.o com_right.o
SRCS = $(srcdir)/error_message.c $(srcdir)/et_name.c $(srcdir)/init_et.c \
	$(srcdir)/com_err.c $(srcdir)/com_right.c

HFILES= com_err.h
SHARE_FILES= et_c.awk et_h.awk

LIBRARY= libcom_err
LIBDIR= et

ELF_VERSION = 2.1
ELF_SO_VERSION = 2
ELF_IMAGE = libcom_err
ELF_MYDIR = et
ELF_INSTALL_DIR = $(root_libdir)
ELF_OTHER_LIBS = 

BSDLIB_VERSION = 1.1
BSDLIB_IMAGE = libcom_err
BSDLIB_MYDIR = et
BSDLIB_INSTALL_DIR = $(root_libdir)

#
# what to build...
#
.c.o:
	@echo "	CC $<"
	@$(CC) $(ALL_CFLAGS) -c $< -o $@
#	@$(CC) $(ALL_CFLAGS) -g -pg -o profiled/$*.o -c $<
#	@$(CC) $(ALL_CFLAGS) -checker -g -o checker/$*.o -c $<
	@$(CC) $(ALL_CFLAGS) -fPIC -o elfshared/$*.o -c $<
#	@$(CC) $(ALL_CFLAGS) $(BSDLIB_PIC_FLAG) -o pic/$*.o -c $<


all:: subdirs $(LIBRARY).a

install-shlibs::

uninstall-shlibs::

real-subdirs::

subdirs:: Makefile
	@$(MAKE) -s real-subdirs
	@touch subdirs

clean::
	@$(RM) -f subdirs

$(LIBRARY).a: $(OBJS)
	@echo "	GEN_LIB $@"
	@(if test -r $@; then $(RM) -f $@.bak && $(MV) $@ $@.bak; fi)
	@$(ARGEN) $@ $(OBJS)
	-@$(RANLIB) $@
	@$(RM) -f ../$@
	@$(LN) $@ ../$@


#
# This is a Makefile stub which handles the creation of Linux ELF shared
# libraries.
#
# In order to use this stub, the following makefile variables must be defined.
# 
# ELF_VERSION = 1.0
# ELF_SO_VERSION = 1
# ELF_IMAGE = libce
# ELF_MYDIR = et
# ELF_INSTALL_DIR = $(SHLIBDIR)
# ELF_OTHER_LIBS = -lc

all:: image

real-subdirs:: Makefile
	@echo "	MKDIR elfshared"
	@mkdir -p elfshared

ELF_LIB = $(ELF_IMAGE).so.$(ELF_VERSION)
ELF_SONAME = $(ELF_IMAGE).so.$(ELF_SO_VERSION)

image:		$(ELF_LIB)

$(ELF_LIB): $(OBJS)
	@echo "	GEN_ELF_SOLIB $(ELF_LIB)"
	@(cd elfshared; $(CC) --shared -o $(ELF_LIB) $(LDFLAGS) \
		-Wl,-soname,$(ELF_SONAME) $(OBJS) $(ELF_OTHER_LIBS))
	@$(MV) elfshared/$(ELF_LIB) .
	@$(RM) -f ../$(ELF_LIB) ../$(ELF_IMAGE).so ../$(ELF_SONAME)
	@$(LN) $(ELF_LIB) ../$(ELF_LIB)
	@$(LN) ../$(ELF_LIB) ../$(ELF_IMAGE).so
	@$(LN) ../$(ELF_LIB) ../$(ELF_SONAME)

installdirs-elf-lib::
	@echo "	MKINSTALLDIRS $(ELF_INSTALL_DIR) $(libdir)"
	@$(MKINSTALLDIRS) $(DESTDIR)$(ELF_INSTALL_DIR) \
		$(DESTDIR)$(libdir)

installdirs:: installdirs-elf-lib

install-shlibs install:: $(ELF_LIB) installdirs-elf-lib
	@echo "	INSTALL-ELF-LIB $(ELF_INSTALL_DIR)/$(ELF_LIB)"
	@$(INSTALL_PROGRAM) $(ELF_LIB) $(DESTDIR)$(ELF_INSTALL_DIR)/$(ELF_LIB)
	@echo "	SYMLINK $(ELF_INSTALL_DIR)/$(ELF_SONAME)"
	@$(LN_S) -f $(ELF_LIB) $(DESTDIR)$(ELF_INSTALL_DIR)/$(ELF_SONAME)
	@echo "	SYMLINK $(libdir)/$(ELF_IMAGE).so"
	@$(LN_S) -f $(ELF_INSTALL_DIR)/$(ELF_SONAME) \
		$(DESTDIR)$(libdir)/$(ELF_IMAGE).so
	@echo "	LDCONFIG"
	@-$(LDCONFIG)

install-strip: install
	@echo "	STRIP-LIB $(ELF_INSTALL_DIR)/$(ELF_LIB)"
	@$(STRIP) --strip-unneeded --remove-section=.comment \
		--remove-section=.note $(DESTDIR)$(ELF_INSTALL_DIR)/$(ELF_LIB)

install-shlibs-strip: install-shlibs
	@echo "	STRIP-LIB $(ELF_INSTALL_DIR)/$(ELF_LIB)"
	@$(STRIP) --strip-unneeded --remove-section=.comment \
		--remove-section=.note $(DESTDIR)$(ELF_INSTALL_DIR)/$(ELF_LIB)

uninstall-shlibs uninstall::
	$(RM) -f $(DESTDIR)$(ELF_INSTALL_DIR)/$(ELF_LIB) \
		$(DESTDIR)$(ELF_INSTALL_DIR)/$(ELF_SONAME) \
		$(DESTDIR)$(libdir)/$(ELF_IMAGE).so
	-$(LDCONFIG)

clean::
	$(RM) -rf elfshared
	$(RM) -f $(ELF_LIB)
	$(RM) -f ../$(ELF_LIB) ../$(ELF_IMAGE).so ../$(ELF_SONAME)




compile_et: $(DEP_SUBSTITUTE) $(srcdir)/compile_et.sh.in
	@echo "	SUBST $@"
	@$(SUBSTITUTE) $(srcdir)/compile_et.sh.in compile_et
	@$(CHMOD) +x compile_et

com_err.ps : com_err.dvi
com_err.dvi: com_err.texinfo

com_err.pc: $(srcdir)/com_err.pc.in $(top_builddir)/config.status
	@echo "	CONFIG.STATUS $@"
	@cd $(top_builddir); CONFIG_FILES=lib/et/com_err.pc ./config.status

#libcom_err.o:	$(LIBOBJS)
#	$(LD) -r -s -o libcom_err.o $(LIBOBJS)
#	chmod -x libcom_err.o

TAGS:	$(SRCS)
	$(TAGS) $(SRCS)

installdirs::
	@echo "	MKINSTALLDIRS $(libdir) $(includedir)/et $(datadir)/et $(bindir) $(man1dir) $(man3dir)"
	@$(MKINSTALLDIRS) $(DESTDIR)$(libdir) \
		$(DESTDIR)$(includedir)/et $(DESTDIR)$(datadir)/et \
		$(DESTDIR)$(bindir) $(DESTDIR)$(man1dir) \
		$(DESTDIR)$(man3dir) $(DESTDIR)$(libdir)/pkgconfig

install:: compile_et libcom_err.a $(HFILES) installdirs com_err.pc
	@echo "	INSTALL_DATA $(libdir)/libcom_err.a"
	@$(INSTALL_DATA) libcom_err.a $(DESTDIR)$(libdir)/libcom_err.a
	@-$(RANLIB) $(DESTDIR)$(libdir)/libcom_err.a
	@$(CHMOD) $(LIBMODE) $(DESTDIR)$(libdir)/libcom_err.a
	@for i in $(HFILES); do \
		echo "	INSTALL_DATA $(includedir)/et/$$i"; \
		$(INSTALL_DATA) $(srcdir)/$$i $(DESTDIR)$(includedir)/et/$$i; \
	done
	@for i in $(SHARE_FILES); do \
		echo "	INSTALL_DATA $(datadir)/et/$$i"; \
		$(INSTALL_DATA) $(srcdir)/$$i $(DESTDIR)$(datadir)/et/$$i; \
	done
	@echo "	INSTALL_SCRIPT $(bindir)/compile_et"
	@$(INSTALL_SCRIPT) compile_et $(DESTDIR)$(bindir)/compile_et
	@echo "	INSTALL_DATA $(man3dir)/com_err.3"
	@$(INSTALL_DATA) $(srcdir)/com_err.3 $(DESTDIR)$(man3dir)/com_err.3
	@echo "	INSTALL_DATA $(man1dir)/compile_et.1"
	@$(INSTALL_DATA) $(srcdir)/compile_et.1 \
			$(DESTDIR)$(man1dir)/compile_et.1
	@echo "	INSTALL_DATA $(libdir)/pkgconfig/com_err.pc"
	@$(INSTALL_DATA) com_err.pc $(DESTDIR)$(libdir)/pkgconfig/com_err.pc

uninstall::
	$(RM) -f $(DESTDIR)$(libdir)/libcom_err.a \
		$(DESTDIR)$(bindir)/compile_et \
		$(DESTDIR)$(libdir)/pkgconfig/com_err.pc
	$(RM) -rf $(DESTDIR)$(includedir)/et $(DESTDIR)$(datadir)/et

check:: compile_et
	for i in $(srcdir)/test_cases/*.et ; do \
		t=`basename $$i | sed -e 's/.et//'`; \
		./compile_et --build-tree $$i ; \
		diff -c $(srcdir)/test_cases/$$t.c $$t.c > $$t.failed; \
		if [ $$? -ne 0 ]; then echo Test case $$t failed; exit 1 ; fi ; \
		diff -c $(srcdir)/test_cases/$$t.h $$t.h >> $$t.failed; \
		if [ $$? -ne 0 ]; then echo Test case $$t failed; exit 1 ; fi ; \
		$(RM) -f $$t.c $$t.h $$t.failed; \
		echo "Test case $$t succeeded" ; \
	done

clean::
	$(RM) -f compile_et libcom_err.a libcom_err_p.a com_err.info
	$(RM) -f $(OBJS) profiled/*
	$(RM) -f *~ \#* *.bak *.otl *.aux *.toc *.PS *.dvi *.ps TAGS *.ln
	$(RM) -f ../libcom_err.a ../libcom_err_p.a

mostlyclean:: clean
distclean:: clean
	$(RM) -f .depend Makefile com_err.pc \
		$(srcdir)/TAGS $(srcdir)/Makefile.in.old

$(OBJS): subdirs

# +++ Dependency line eater +++
# 
# Makefile dependencies follow.  This must be the last section in
# the Makefile.in file
#
error_message.o: $(srcdir)/error_message.c $(srcdir)/com_err.h \
 $(srcdir)/error_table.h $(srcdir)/internal.h
et_name.o: $(srcdir)/et_name.c $(srcdir)/com_err.h $(srcdir)/error_table.h \
 $(srcdir)/internal.h
init_et.o: $(srcdir)/init_et.c $(srcdir)/com_err.h $(srcdir)/error_table.h
com_err.o: $(srcdir)/com_err.c $(srcdir)/com_err.h $(srcdir)/error_table.h \
 $(srcdir)/internal.h
com_right.o: $(srcdir)/com_right.c $(srcdir)/com_err.h \
 $(srcdir)/error_table.h
