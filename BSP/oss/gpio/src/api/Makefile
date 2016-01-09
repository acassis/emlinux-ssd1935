#TARGET := libgpioctrl.so
#DFLAGS := -share  -lpthread
TARGET := libgpioctrl.so
LDFLAGS := -rv
CFLAGS  := -Wall -g -rdynamic
OBJS    := gpio_ctrl.o
CC      := arm-linux-gcc
LDFLAGS  := -share
LD	:= arm-linux-ld
all:$(TARGET)
$(TARGET):$(OBJS)
	$(LD) $(LDFLAGS) -g -o $@ $^
	mkdir -p ../../lib && cp libgpioctrl.so ../../lib
.cpp.o:
	$(CC) $(CFLAGS) -o $@ $^
clean:
	@rm -fv *.o ${OBJS} $(TARGET)

