### 2005.04.25 Sky U-Media ###


VPATH = ../src

CFLAGS += -DOPTIMIZE_SRC_CODE_SIZE \
	  -DDISABLE_ID3_V1

libmp.so: mplib.o mplib_s.o xmalloc.o
	$(CC) -shared -o libmp.so mplib.o mplib_s.o xmalloc.o
	cp libmp.so ../../../../../obj/

clean :
	rm -f *.o *.so
