### 2005.04.25 Sky U-Media ###

INCLUDES = -I../include -I../ -I../lib/sdp -I../lib -I../player/lib/rtsp

VPATH = ../lib/sdp \
	../lib/utils \
	../player/lib/rtsp

libmpeg4ip.so: sdp_decode.o sdp_dump.o sdp_encode.o sdp_util.o \
	       message.o network.o url.o config_opts.o \
	       rtsp.o rtsp_comm.o rtsp_command.o rtsp_resp.o rtsp_thread.o rtsp_thread_nx.o rtsp_util.o
	$(CC) $(LDFLAGS) -shared -o libmpeg4ip.so \
		sdp_decode.o sdp_dump.o sdp_encode.o sdp_util.o \
		message.o network.o url.o config_opts.o \
                rtsp.o rtsp_comm.o rtsp_command.o rtsp_resp.o rtsp_thread.o rtsp_thread_nx.o rtsp_util.o
	cp libmpeg4ip.so ../../../../../obj/

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<
.cpp.o:
	$(CXX) $(INCLUDES) -c $< 

clean :
	rm -f *.o *.so
