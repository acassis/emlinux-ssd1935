
sysconfdir=${prefix}/etc
piddir=@piddir@
srcdir=.
top_srcdir=..

all:	libopenbsd-compat.a

OPENBSD=	asprintf.o daemon.o inet_pton.o strlcpy.o
COMPAT=		atomicio.o bsd-arc4random.o bsd-misc.o bsd-poll.o \
		bsd-snprintf.o fake-rfc2553.o uidswap.o
PORT=		port-qnx.o


#CC=gcc
#LD=gcc
CFLAGS=-g -O2 -Wall -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wshadow -Wpointer-arith -Wcast-qual -Wsign-compare -D_GNU_SOURCE $(PROG_CFLAGS)
# LDFLAGS=
CPPFLAGS=-I. -I.. -I$(top_srcdir) -I$(srcdir)/.. 
LIBS=-lcrypto 
#AR=/usr/bin/ar
#RANLIB=ranlib
INSTALL=/usr/bin/ginstall -c

$(COMPAT):	../config.h
$(OPENBSD):	../config.h
$(PORT):	../config.h

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

libopenbsd-compat.a:	$(COMPAT) $(OPENBSD) $(PORT)
	$(AR) rv $@ $(COMPAT) $(OPENBSD) $(PORT)
	$(RANLIB) $@

clean:
	rm -f $(COMPAT) $(OPENBSD) $(PORT) libopenbsd-compat.a
