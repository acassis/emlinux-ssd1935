PF_APHRODITE = 1	# must correspond with platform.h
PF_ARTEMIS = 2		# must correspond with platform.h

OUTNAME = ddrtune
TOOLS = /usr/local/arm/3.4.6/bin
CC = $(TOOLS)/arm-linux-gcc
LD = $(TOOLS)/arm-linux-ld
OBJCP = $(TOOLS)/arm-linux-objcopy
#LIB = ../lib/magus_v1_1.a
#LIB = ../lib/artemis.a
#INC = ../lib
SRCS = ddrtune.c dmacapi.c dma.c gpt.c arm.s uart.c str.c nfc.c nand.c 
OBJS = ddrtune.o dmacapi.o dma.o gpt.o arm.o uart.o str.o nfc.o nand.o

CFLAGS = -O2 -Wall -nostdlib  -march=armv4 \
	-DPLATFORM=$(PF_APHRODITE) \
	-DPATTERN_RANDOM\
#	-DDDR_TUNE_WORD=0\
#	-DRUN_BENCHMARK \
#	-DPRISM_TEST
# -DUSE_DDR_SDRAM
 
LDFLAGS=-M=$(OUTNAME).map -T lds

all: $(OUTNAME).bin
#all: $(OBJS)

$(OBJS): $(SRCS) Makefile
	$(CC) $(CFLAGS) -c $(SRCS)

$(OUTNAME).exe: $(OBJS)
	$(LD) $(LDFLAGS) -o $(OUTNAME).exe $(OBJS)

$(OUTNAME).bin: $(OUTNAME).exe
	$(OBJCP) -Obinary $(OUTNAME).exe $(OUTNAME).bin

clean: 
	@rm -rf $(OBJS) *.exe *.bin *.elf
#	make 

