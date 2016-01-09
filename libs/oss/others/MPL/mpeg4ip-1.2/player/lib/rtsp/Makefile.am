lib_LIBRARIES = librtsp.a
include_HEADERS = \
	rtsp_client.h \
	rtsp_private.h \
	rtsp_thread_ipc.h \
	rtsp_thread_nx.h

librtsp_a_SOURCES = \
	rtsp.c \
	rtsp_comm.c \
	rtsp_command.c \
	rtsp_resp.c \
	rtsp_thread.c \
	rtsp_thread_nx.c \
	rtsp_util.c 

EXTRA_DIST = librtsp60.dsp

INCLUDES = -I$(top_srcdir)/../../../lib/sdp -I$(top_srcdir)/../../../lib -I$(top_srcdir)/../../../include -I$(top_srcdir)/../../../

