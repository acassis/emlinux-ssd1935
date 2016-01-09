#
# arch/arm/Makefile
# creator: Felix YN Wong
# creation date : 20 June 2008
#
OUTNAME=ipl
PREFIX=/usr/local/arm/3.4.6
CC=$(PREFIX)/bin/arm-linux-gcc
AS=$(PREFIX)/bin/arm-linux-as
LD=$(PREFIX)/bin/arm-linux-ld
OBJCOPY=$(PREFIX)/bin/arm-linux-objcopy

CFLAGS=-O2 -Wall -W1 -nostdlib -march=armv4
LDFLAGS=-M=$(OUTNAME).map -T ipl.lds

SRCS=uart.c ipl.c brd.c sdr.c nfc.c nand.c arm.s
DEPS=uart.h brd.h

OBJS=uart.o ipl.o brd.o sdr.o nfc.o nand.o arm.o

all: $(OUTNAME).bin

$(OBJS): $(SRCS) makefile
	$(CC) $(CFLAGS) -c $(SRCS)

$(OUTNAME).exe: $(OBJS)
	$(LD) $(LDFLAGS) -o $(OUTNAME).exe $(OBJS)

$(OUTNAME).bin: $(OUTNAME).exe
	$(OBJCOPY) -Obinary $(OUTNAME).exe $(OUTNAME).bin

clean:
	rm -rf *.exe *.o *.map *.bin
