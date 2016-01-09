
DEFINES= -DHAVE_RDTSC #-DHAVE_SSE2
CFLAGS=-Isrc -O2 -W -Wall $(DEFINES)
## 2007.02.14 : accumulate LDFLAGS to keep original env definitions
LDFLAGS+=-L. -lxyssl -lgcc -lc
DESTDIR=/usr/local

ifdef SELF_TEST
DEFINES += SELF_TEST
endif

## 2009.03.23 : include ARM_SSD1933 platform
## 2007.02.14 : include diffirent platform
ifdef PLATFORM
ifeq ($(PLATFORM), ARM_SSD1933)
CFLAGS += -D__arm__
endif
ifeq ($(PLATFORM), ARM_STR8131)
CFLAGS += -D__arm__
endif
ifeq ($(PLATFORM), ARM_EP93xx)
CFLAGS += -D__arm__
endif
ifeq ($(PLATFORM), ARM_EP93xx_glibc)
CFLAGS += -D__arm__
endif
ifeq ($(PLATFORM), MIPS_RTL865xB)
CFLAGS += -D__mips__
endif
endif

##LIB_OBJ=src/aes.o          src/arc4.o         src/base64.o       \
##        src/bignum.o       src/des.o          src/dhm.o          \
##        src/havege.o       src/md2.o          src/md4.o          \
##        src/md5.o          src/net.o          src/rsa.o          \
##        src/sha1.o         src/sha2.o         src/ssl_v3.o       \
##        src/ssl_cli.o      src/ssl_srv.o      src/testcert.o     \
##        src/timing.o       src/x509_in.o

## 2007.02.16 : remove useless code
LIB_OBJ=src/bignum.o
ifdef USE_AES
LIB_OBJ+=src/aes.o
endif
ifdef USE_RC4
LIB_OBJ+=src/arc4.o
endif
ifdef USE_BASE64
LIB_OBJ+=src/base64.o
endif
ifdef USE_DHM
LIB_OBJ+=src/dhm.o
endif
ifdef USE_MD2
LIB_OBJ+=src/md2.o
endif
ifdef USE_MD4
LIB_OBJ+=src/md4.o
endif
ifdef USE_MD5
LIB_OBJ+=src/md5.o
endif
ifdef USE_DES
LIB_OBJ+=src/des.o
endif
ifdef USE_SSL
LIB_OBJ+=src/ssl_cli.o	src/ssl_srv.o	src/ssl_v3.o	\
		 src/net.o	src/testcert.o	src/havege.o
endif
ifdef USE_SHA1
LIB_OBJ+=src/sha1.o
endif
ifdef USE_SHA2
LIB_OBJ+=src/sha2.o
endif
ifdef USE_RSA
LIB_OBJ+=src/rsa.o
endif
ifdef USE_X509_IN
LIB_OBJ+=src/x509_in.o
endif
ifdef USE_TIMING
LIB_OBJ+=src/timing.o
endif
##

## 2007.02.14 : add support of rijdael optimized C code v3.0
ifdef USE_RIJNDAEL_CODE
CFLAGS += -DUSE_RIJNDAEL_CODE
LIB_OBJ+=src/rijndael-alg-fst.o
endif
ifdef USE_BLOWFISH
LIB_OBJ+=src/blowfish.o
endif

## APP_OBJ=app/benchmark      app/hello          app/filecrypt      \
##        app/rsa_demo       app/selftest       app/ssl_client     \
##        app/ssl_server

## build the shared library

## all: libxyssl.so apps
all: libxyssl.so

## libxyssl.a: $(LIB_OBJ)
##	$(AR) r $@ $(LIB_OBJ); $(RANLIB) $@
libxyssl.so: $(LIB_OBJ)
	$(CC) -shared -o libxyssl.so $(LIB_OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

## apps: $(APP_OBJ)

## app/%:  app/%.c  libxyssl.so
##	$(CC) $(CFLAGS) $< -o $@ -L./ -lxyssl $(LDFLAGS)

docs:
	@rm -rf doc
	@cd src && doxygen ; cd ..
	@mv html doc

install:
	mkdir -p $(DESTDIR)/{include/xyssl,lib}
	cp src/*.h $(DESTDIR)/include/xyssl
	cp libxyssl.so $(DESTDIR)/lib

install-apps:
	mkdir -p $(DESTDIR)/bin
	for i in $(APP_OBJ); do         \
	    cp $i $(DESTDIR)/xyssl_$i;  \
	done

clean:
	@rm -f libxyssl.a src/*.o app/*.o $(APP_OBJ) libxyssl.so

