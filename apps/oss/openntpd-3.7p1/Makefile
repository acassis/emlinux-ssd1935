#	$OpenBSD: Makefile,v 1.7 2004/07/04 11:01:49 alexander Exp $

prefix=/usr/local
exec_prefix=${prefix}
bindir=${exec_prefix}/bin
sbindir=${exec_prefix}/sbin
libexecdir=${exec_prefix}/libexec
datadir=${prefix}/share
mandir=${prefix}/man
mansubdir=man
sysconfdir=${prefix}/etc
srcdir=.
top_srcdir=.

DESTDIR=

#CC=gcc
CFLAGS=-g -O2 -Wall -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wshadow -Wpointer-arith -Wcast-qual -Wsign-compare -D_GNU_SOURCE $(PROG_CFLAGS)

ifeq ($(PLATFORM),MIPS_RTL865xB)
CFLAGS += -DMIPS_RTL865XB
PLATFORM_RTC_SRCS = rtl865xb_rtc.c
PLATFORM_RTC_OBJS = rtl865xb_rtc.o
endif

ifeq ($(PLATFORM),ARM_EP93xx)
CFLAGS += -DARM_EP93xx
PLATFORM_RTC_SRCS = ep93xx_rtc.c
PLATFORM_RTC_OBJS = ep93xx_rtc.o
endif

CPPFLAGS= $(PATHS) -I$(top_srcdir) -I$(top_srcdir)/openbsd-compat
#LIBS=	-lcrypto $(LDLIBS)
LIBS= $(LDLIBS)
# LDFLAGS=

INSTALL=/usr/bin/ginstall -c

PATHS= -DSYSCONFDIR=\"$(sysconfdir)\" 
PRIVSEP_USER=_ntp
PRIVSEP_PATH=/var/empty
STRIP_OPT=-s

PROG=	ntpd
LIBCOMPAT=openbsd-compat/libopenbsd-compat.a
SRCS=	ntpd.c buffer.c log.c imsg.c ntp.c ntp_msg.c config.c \
	server.c client.c util.c y.tab.c $(PLATFORM_RTC_SRCS)
OBJS=	ntpd.o buffer.o log.o imsg.o ntp.o ntp_msg.o config.o \
	server.o client.o util.o y.tab.o $(PLATFORM_RTC_OBJS)
YFLAGS=
MAN=	ntpd.8 ntpd.conf.5

all:	ntpd

$(LIBCOMPAT):	config.h  $(top_srcdir)/openbsd-compat/*.c \
	    $(top_srcdir)/openbsd-compat/*.h
	(cd openbsd-compat && $(MAKE))

$(OBJS):	config.h

ntpd:	ntpd.o buffer.o log.o imsg.o ntp.o ntp_msg.o config.o \
	server.o client.o util.o y.tab.o $(PLATFORM_RTC_OBJS) \
	$(LIBCOMPAT)
	$(CC) $(CFLAGS) $(LDFLAGS) -o ntpd $(OBJS) $(LIBCOMPAT) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

y.tab.c:	$(top_srcdir)/parse.y
	yacc $(top_srcdir)/parse.y

install: ntpd
	@if [ ! -d $(DESTDIR)$(sbindir) ]; then \
		$(INSTALL) -m 755 -d $(DESTDIR)$(sbindir); \
	fi
	@if [ ! -d $(DESTDIR)$(sysconfdir) ]; then \
		$(INSTALL) -m 755 -d $(DESTDIR)$(sysconfdir); \
	fi
	@if [ ! -d $(DESTDIR)$(mandir)/$(mansubdir)5 ]; then \
		$(INSTALL) -m 755 -d $(DESTDIR)$(mandir)/$(mansubdir)5; \
	fi
	@if [ ! -d $(DESTDIR)$(mandir)/$(mansubdir)8 ]; then \
		$(INSTALL) -m 755 -d $(DESTDIR)$(mandir)/$(mansubdir)8; \
	fi
	$(INSTALL) -m 0755 $(STRIP_OPT) ntpd $(DESTDIR)$(sbindir)/ntpd
	$(INSTALL) -m 644 ntpd.8 $(DESTDIR)$(mandir)/$(mansubdir)8/ntpd.8
	$(INSTALL) -m 644 ntpd.conf.5 $(DESTDIR)$(mandir)/$(mansubdir)5/ntpd.conf.5
	@if [ ! -d $(DESTDIR)$(sysconfdir) ]; then \
		$(INSTALL) -m 755 -d $(DESTDIR)$(sysconfdir); \
	fi
	@if [ ! -f $(DESTDIR)$(sysconfdir)/ntpd.conf ]; then \
		$(INSTALL) -m 644 ntpd.conf $(DESTDIR)$(sysconfdir)/ntpd.conf; \
	else \
		echo "$(DESTDIR)$(sysconfdir)/ntpd.conf already exists, install will not overwrite"; \
	fi
	@if [ ! -d $(DESTDIR)$(PRIVSEP_PATH) ]; then \
		mkdir -p $(DESTDIR)$(PRIVSEP_PATH) ;\
		chown 0 $(DESTDIR)$(PRIVSEP_PATH) ; \
		chgrp 0 $(DESTDIR)$(PRIVSEP_PATH) ; \
		chmod 0755 $(DESTDIR)$(PRIVSEP_PATH) ; \
	fi
	@if grep "$(PRIVSEP_USER):" /etc/group >/dev/null; then \
	  : ;\
	else \
	  echo "Please create a dedicated group for ntpd." ;\
	  echo "This is system-dependant, possibly:" ;\
	  echo "# groupadd $(PRIVSEP_USER)" ;\
	fi
	@if grep "$(PRIVSEP_USER):" /etc/passwd >/dev/null; then \
	  : ;\
	else \
	  echo "Please create a dedicated user for ntpd and ensure it can" ;\
	  echo "not be used to log in.  This is system-dependant, possibly:" ;\
	  echo "# useradd -g $(PRIVSEP_USER) -s /sbin/nologin -d $(PRIVSEP_PATH) -c 'OpenNTP daemon' $(PRIVSEP_USER)" ;\
	fi

diff:
	-diff -x CVS -x Makefile -ru /usr/src/usr.sbin/ntpd ./ | \
	    egrep -v "^Only in" >ntpd-vs-openbsd.diff

clean:
	rm -f $(OBJS) ntpd openbsd-compat/*.o openbsd-compat/*.a \
	    configure.lineno config.h.in~

distclean: clean
	rm -rf $(OBJS) config.log config.status configure.lineno \
	    config.h.in~ config.h Makefile openbsd-compat/Makefile

distprep: distclean
	autoheader
	autoreconf
	yacc parse.y
	rm -rf Makefile openbsd-compat/Makefile autom4te.cache
