if NO_GLIB_GTK
SUBD= 
GUILIBS= 
GUIADD=
else
SUBD=gui
GUILIBS=libmp4livepreview.la
GUIADD=gui/libmp4livegui.la
endif

SUBDIRS = $(SUBD) h261

bin_PROGRAMS = mp4live

noinst_LTLIBRARIES = \
	libmp4live.la \
	$(GUILIBS)

libmp4livepreview_la_SOURCES = \
	preview_flow.cpp \
	preview_flow.h \
	video_sdl_preview.h \
	video_sdl_preview.cpp

if HAVE_V4L2
  es1= video_v4l2_source.cpp video_v4l2_source.h
  ed1= video_v4l_source.cpp video_v4l_source.h
else
  es1= video_v4l_source.cpp video_v4l_source.h
  ed1= video_v4l2_source.cpp video_v4l2_source.h
endif

if NO_XVID
   XVID_SRC=
   ed2=video_xvid10.h video_xvid10.cpp video_xvid.h video_xvid.cpp
   XVID_LIB=
else
if HAVE_XVID_1_0
  XVID_SRC=video_xvid10.h video_xvid10.cpp
  ed2 = video_xvid.h video_xvid.cpp
else
   XVID_SRC=video_xvid.cpp video_xvid.h
   ed2=video_xvid10.h video_xvid10.cpp
endif
  XVID_LIB=-lxvidcore
endif


libmp4live_la_SOURCES = \
	audio_encoder_base.cpp \
	audio_encoder.h \
	audio_faac.cpp \
	audio_faac.h \
	audio_lame.cpp \
	audio_lame.h \
	audio_oss_source.cpp \
	audio_oss_source.h \
	file_mp4_recorder.cpp \
	file_mp4_recorder.h \
	file_raw_sink.cpp \
	file_raw_sink.h \
	loop_feeder_sink.cpp \
	loop_feeder_sink.h \
	loop_source.cpp \
	loop_source.h \
	media_codec.h \
	media_flow.cpp \
	media_flow.h \
	media_frame.h \
	media_node.h \
	media_sink.h \
	media_source.cpp \
	media_source.h \
	media_time.h \
	mp4live_common.cpp \
	mp4live_common.h \
	mp4live_config.cpp \
	mp4live_config.h \
	resample.c \
	resampl.h \
	rtp_transmitter.cpp \
	rtp_transmitter.h \
	sdp_file.cpp \
	util.cpp \
	util.h \
	video_util_filter.cpp \
	video_util_filter.h \
	video_util_mpeg4.cpp \
	video_util_resize.h \
	video_util_resize.cpp \
	video_util_rgb.h \
	video_util_rgb.cpp \
	video_util_tv.h \
	video_util_tv.cpp \
	$(es1) \
	video_encoder.h \
	video_encoder_base.h \
	video_encoder_base.cpp \
	audio_ffmpeg.cpp \
	audio_ffmpeg.h \
	video_ffmpeg.cpp \
	video_ffmpeg.h $(XVID_SRC)

mp4live_SOURCES = \
	audio_encoder.cpp \
	audio_encoder_tables.cpp \
	video_encoder.cpp \
	video_encoder_tables.cpp \
	mp4live.cpp \
	mp4live.h 
# LATER
# video_1394_source
# video_dv

INCLUDES=\
	@GLIB_CFLAGS@ @GTK_CFLAGS@ \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/lib/mp4v2 \
	-I$(top_srcdir)/lib/mp4av \
	-I$(top_srcdir)/lib/msg_queue \
	-I$(top_srcdir)/lib/rtp \
	-I$(top_srcdir)/lib/sdp \
	-I$(top_srcdir)/lib/utils \
	-I$(top_srcdir)/lib \
	-I$(top_srcdir)/lib/mpeg2ps \
	-I$(top_srcdir)/player/lib \
	-I$(top_srcdir)/player/src \
	-I./h261 \
	@FFMPEG_INC@

AM_CXXFLAGS = -DADD_XVID_ENCODER \
	-DADD_LAME_ENCODER -DADD_FAAC_ENCODER \
	-D_REENTRANT -DNOCONTROLS -fexceptions @BILLS_CPPWARNINGS@

mp4live_LDADD = \
	@GTK_LIBS@ @GLIB_LIBS@ \
	$(GUIADD) \
	libmp4live.la \
	-lfaac \
	-lmp3lame \
	$(top_builddir)/lib/mpeg2ps/libmpeg2_program.la \
	$(top_builddir)/lib/msg_queue/libmsg_queue.la \
	$(top_builddir)/lib/mp4v2/libmp4v2.la \
	$(top_builddir)/lib/mp4av/libmp4av.la \
	$(top_builddir)/lib/rtp/libuclmmbase.la \
	$(top_builddir)/lib/sdp/libsdp.la \
	$(top_builddir)/lib/gnu/libmpeg4ip_gnu.la \
	$(top_builddir)/lib/utils/libutils.la \
	./h261/libmp4live_h261.la \
	-lpthread \
	@SDL_LIBS@ $(GUILIBS) @FFMPEG_LIB@ @LIBVORBIS_LIB@ $(XVID_LIB) 

EXTRA_DIST = $(ed1) $(ed2) README_MULTIOUTPUT.txt
