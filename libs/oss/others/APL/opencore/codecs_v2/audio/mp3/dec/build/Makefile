#########################################################
### Makefile for building the packetvideo's mp3 decoder
### Date : 2009.12.15
### Author : Sky Huang
### U-Media Inc.
#########################################################

__STANDARD__=YES
SRCROOT=../../../../../..

include $(SRCROOT)/../../../obj/Envir.mk

## The compiler tools ###################################
RM				= rm -f
RM_DIR			= rm -rf
MAKE_DEP		=
MAKE_DEP_FLAGS	= $(INCLUDES) $(DEFINES)
MAKE_LIB		= $(AR) cr
MAKE_LIB_FLAGS	=
CP				= cp
MAKE			= make

CCFLAGS = -pipe -Wall -Wreturn-type -fno-exceptions -march=pentium -mcpu=pentium -O2   $(INCLUDES) $(DEFINES)
CXXFLAGS = -pipe -Wall -Wreturn-type -Wno-non-virtual-dtor -fno-exceptions --permissive -fno-rtti -Wno-ctor-dtor-privacy -march=pentium -mcpu=pentium -O2   $(INCLUDES) $(DEFINES)

## platform configuration #######################################

CCFLAGS = $(INCLUDES) $(DEFINES)
CXXFLAGS = $(INCLUDES) $(DEFINES)

## use uclibc++ or not
USE_UCLIBC_PP = NO

ifeq ($(ARCH), intel)
CCFLAGS += -pipe -Wall -Wreturn-type -fno-exceptions -march=pentium -mcpu=pentium -O2
CCFLAGS += -fPIC -DPIC
CCFLAGS += -DTARGET_AAC_RAW_FRMAE_ONLY
REALCODEC_INTERFACE = ENABLE_CXX
DYNAMIC_LIBS = -lstdc++
endif

ifeq ($(ARCH), mips)
## RealTek 865xB ##########################
## big-endian
ifeq ($(PLATFORM), MIPS_RTL865xB)
##### don't use Envir.mk CFLAGS parameter
## CFLAGS =
##### 0. need to use -O3 (or -O2) for optimization otherwise speed will slow down ..
##### definition use :
##### 1. MIPS_RTL865xB : 
##### 			in rarvcode-audio/codec/ra8lbr/fixpt/decoder/assembly.h
#####		      	define our MULSHIFT32(), CLIPTOSHORT(), ... functions for this platform
##### 2. HELIX_CONFIG_DISABLE_ATOMIC_OPERATORS :
#####			need to know ....
ADDITIONAL_CFLAGS =  -O3 \
	   	     -D_MIPS -DMIPS_RTL865xB \
	   	     -DHELIX_CONFIG_DISABLE_ATOMIC_OPERATORS

CCFLAGS += $(ADDITIONAL_CFLAGS)
CCFLAGS += -DTARGET_AAC_RAW_FRMAE_ONLY
USE_UCLIBC_PP = YES
endif
## Other platform ####

endif

ifeq ($(ARCH), arm)
## Cirrus EP93xx uclibc version ###########
## Cirrus EP93xx glibc version ############
## Star str8131 uclibc 0.9.29 version #####
## little-endian

ADDITIONAL_CFLAGS = -O2 -DARM -D$(PLATFORM)
ADDITIONAL_CXXFLAGS = -O2 -DARM -D$(PLATFORM)

ifeq ($(PLATFORM), ARM_EP93xx)
##### 0. need to use -O2 (don't use -O3 because it has problem at this platform) for optimization
CCFLAGS += -DPV_ARM_GCC_V4
CXXFLAGS += -DPV_ARM_GCC_V4
USE_UCLIBC_PP = YES
endif

ifeq ($(PLATFORM), ARM_STR8131)
##### 0. need to use -O2 (don't use -O3 because it has problem at this platform) for optimization
CCFLAGS += -DPV_ARM_GCC_V4
CXXFLAGS += -DPV_ARM_GCC_V4
USE_UCLIBC_PP = YES
endif

ifeq ($(PLATFORM), ARM_SSD1933)
ADDITIONAL_CFLAGS = -O3 -DARM -D$(PLATFORM)
ADDITIONAL_CXXFLAGS = -O3 -DARM -D$(PLATFORM)
CCFLAGS += -DPV_ARM_GCC_V5
CXXFLAGS += -DPV_ARM_GCC_V5
endif

ifeq ($(PLATFORM), ARM_SSD1935)
ADDITIONAL_CFLAGS = -O3 -DARM -D$(PLATFORM)
ADDITIONAL_CXXFLAGS = -O3 -DARM -D$(PLATFORM)
CCFLAGS += -DPV_ARM_GCC_V5
CXXFLAGS += -DPV_ARM_GCC_V5
endif

##### don't use Envir.mk CFLAGS parameter
## CFLAGS =

CCFLAGS += $(ADDITIONAL_CFLAGS)
CXXFLAGS += $(ADDITIONAL_CXXFLAGS)
CCFLAGS += -DTARGET_AAC_RAW_FRMAE_ONLY
## Other platfrom ####

endif

CXXFLAGS += -DOPTIMIZE_FOR_PERFORMANCE

##CXXFLAGS += -DENABLE_PVEQ
##CCFLAGS += -DENABLE_PVEQ

ifeq ($(USE_UCLIBC_PP), YES)
############################################################
## we currently use the RealAudio general CXX interface
## And we need the uClibc++ to replace the stdc++ library
## Also we need to use -lgcc to fix some link problem
############################################################
REALCODEC_INTERFACE = ENABLE_CXX
LDFLAGS += -L$(EMLXBASE)/libs/gpl/uClibc++/src
DYNAMIC_LIBS = -luClibc++ -lgcc
endif

##############################################################

SRCS = \
	../src/pvmp3_normalize.cpp \
	../src/pvmp3_alias_reduction.cpp \
	../src/pvmp3_crc.cpp \
	../src/pvmp3_decode_header.cpp \
	../src/pvmp3_decode_huff_cw.cpp \
	../src/pvmp3_getbits.cpp \
	../src/pvmp3_dequantize_sample.cpp \
	../src/pvmp3_framedecoder.cpp \
	../src/pvmp3_get_main_data_size.cpp \
	../src/pvmp3_get_side_info.cpp \
	../src/pvmp3_get_scale_factors.cpp \
	../src/pvmp3_mpeg2_get_scale_data.cpp \
	../src/pvmp3_mpeg2_get_scale_factors.cpp \
	../src/pvmp3_mpeg2_stereo_proc.cpp \
	../src/pvmp3_huffman_decoding.cpp \
	../src/pvmp3_huffman_parsing.cpp \
	../src/pvmp3_tables.cpp \
	../src/pvmp3_imdct_synth.cpp \
	../src/pvmp3_mdct_6.cpp \
	../src/pvmp3_dct_6.cpp \
	../src/pvmp3_poly_phase_synthesis.cpp \
	../src/pvmp3_equalizer.cpp \
	../src/pvmp3_seek_synch.cpp \
	../src/pvmp3_stereo_proc.cpp \
	../src/pvmp3_reorder.cpp \
	../src/pvmp3_polyphase_filter_window.cpp \
	../src/pvmp3_mdct_18.cpp \
	../src/pvmp3_dct_9.cpp \
	../src/pvmp3_dct_16.cpp \
	../src/decwrapper.cpp \
	../../../../../oscl/oscl/osclmemory/src/oscl_mem_imp.cpp
###	../src/pvmp3_decoder.cpp \

ifdef PV_ARM_GCC_V5
SRC += \
	../src/asm/pvmp3_polyphase_filter_window_gcc.s \
	../src/asm/pvmp3_mdct_18_gcc.s \
	../src/asm/pvmp3_dct_9_gcc.s \
	../src/asm/pvmp3_dct_16_gcc.s
else
ifdef PV_ARM_GCC_V4
SRC += \
	../src/asm/pvmp3_polyphase_filter_window_gcc.s \
	../src/asm/pvmp3_mdct_18_gcc.s \
	../src/asm/pvmp3_dct_9_gcc.s \
	../src/asm/pvmp3_dct_16_gcc.s
else
ifdef PV_ARM_V5
SRC += \
	../src/asm/pvmp3_polyphase_filter_window_gcc.s \
	../src/asm/pvmp3_mdct_18_gcc.s \
	../src/asm/pvmp3_dct_9_gcc.s \
	../src/asm/pvmp3_dct_16_gcc.s
else
ifdef PV_ARM_V4
SRC += \
	../src/asm/pvmp3_polyphase_filter_window_gcc.s \
	../src/asm/pvmp3_mdct_18_gcc.s \
	../src/asm/pvmp3_dct_9_gcc.s \
	../src/asm/pvmp3_dct_16_gcc.s 
endif
endif
endif
endif
	
OBJS = $(COMPILED_OBJS) $(SOURCE_OBJS)

COMPILED_OBJS = \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_normalize.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_alias_reduction.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_crc.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_decode_header.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_decode_huff_cw.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_getbits.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_dequantize_sample.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_framedecoder.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_get_main_data_size.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_get_side_info.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_get_scale_factors.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_mpeg2_get_scale_data.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_mpeg2_get_scale_factors.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_mpeg2_stereo_proc.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_huffman_decoding.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_huffman_parsing.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_tables.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_imdct_synth.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_mdct_6.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_dct_6.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_poly_phase_synthesis.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_equalizer.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_seek_synch.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_stereo_proc.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_reorder.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_polyphase_filter_window.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_mdct_18.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_dct_9.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_dct_16.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/decwrapper.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/oscl_mem_imp.o
###	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_decoder.o \

ifdef PV_ARM_GCC_V5
COMPILED_OBJS += \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_polyphase_filter_window_gcc.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_mdct_18_gcc.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_dct_9_gcc.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_dct_16_gcc.o
else
ifdef PV_ARM_GCC_V4
COMPILED_OBJS += \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_polyphase_filter_window_gcc.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_mdct_18_gcc.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_dct_9_gcc.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_dct_16_gcc.o
else
ifdef PV_ARM_V5
COMPILED_OBJS += \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_polyphase_filter_window_gcc.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_mdct_18_gcc.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_dct_9_gcc.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_dct_16_gcc.o
else
ifdef PV_ARM_V4
COMPILED_OBJS += \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_polyphase_filter_window_gcc.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_mdct_18_gcc.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_dct_9_gcc.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_dct_16_gcc.o
endif
endif
endif
endif

SOURCE_OBJS =

INCLUDES = \
	-I../include \
	-I../src \
	-I../../../../../oscl/oscl/config/android \
	-I../../../../../oscl/oscl/config/shared \
	-I../../../../../oscl/oscl/osclbase/src \
	-I../../../../../oscl/oscl/osclerror/src \
	-I../../../../../oscl/oscl/osclio/src \
	-I../../../../../oscl/oscl/oscllib/src \
	-I../../../../../oscl/oscl/osclmemory/src \
	-I../../../../../oscl/oscl/osclproc/src \
	-I../../../../../oscl/oscl/osclregcli/src \
	-I../../../../../oscl/oscl/osclregserv/src \
	-I../../../../../oscl/oscl/osclutil/src \
	-I../../../../../build_config/opencore_dynamic

STATIC_LIBS =

.SUFFIXES: .cpp .c .s .S

.c.o: 
	$(CC) $(CCFLAGS) -o  $@ -c $<

.cpp.o: 
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o  $@ -c $<

.s.o: 
	$(CC) $(CCFLAGS) -o  $@ -c $<

.S.o: 
	$(CC) $(CCFLAGS) -o  $@ -c $<

all: $(PLATFORM_BUILD_DIRECTORY)-obj pv_mp3_dec.a

$(PLATFORM_BUILD_DIRECTORY)-obj: 
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-lib || mkdir $(PLATFORM_BUILD_DIRECTORY)-lib
	
all_objects: $(OBJS)

pv_mp3_dec.a: $(OBJS)
	## static library
	$(AR) cr  $(PLATFORM_BUILD_DIRECTORY)-lib/pv_mp3_dec.a $(OBJS)
	$(RANLIB) $(PLATFORM_BUILD_DIRECTORY)-lib/pv_mp3_dec.a
	## share library
	$(CC) $(LDFLAGS) -shared -o $(PLATFORM_BUILD_DIRECTORY)-lib/libpv_mp3_dec.so $(OBJS)  $(DYNAMIC_LIBS)

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_normalize.o: ../src/pvmp3_normalize.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_normalize.o -c ../src/pvmp3_normalize.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_alias_reduction.o: ../src/pvmp3_alias_reduction.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_alias_reduction.o -c ../src/pvmp3_alias_reduction.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_crc.o: ../src/pvmp3_crc.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_crc.o -c ../src/pvmp3_crc.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_decode_header.o: ../src/pvmp3_decode_header.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_decode_header.o -c ../src/pvmp3_decode_header.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_decode_huff_cw.o: ../src/pvmp3_decode_huff_cw.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_decode_huff_cw.o -c ../src/pvmp3_decode_huff_cw.cpp

###$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_decoder.o: ../src/pvmp3_decoder.cpp
###	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
###	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_decoder.o -c ../src/pvmp3_decoder.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_getbits.o: ../src/pvmp3_getbits.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_getbits.o -c ../src/pvmp3_getbits.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_dequantize_sample.o: ../src/pvmp3_dequantize_sample.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_dequantize_sample.o -c ../src/pvmp3_dequantize_sample.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_framedecoder.o: ../src/pvmp3_framedecoder.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_framedecoder.o -c ../src/pvmp3_framedecoder.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_get_main_data_size.o: ../src/pvmp3_get_main_data_size.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_get_main_data_size.o -c ../src/pvmp3_get_main_data_size.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_get_side_info.o: ../src/pvmp3_get_side_info.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_get_side_info.o -c ../src/pvmp3_get_side_info.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_get_scale_factors.o: ../src/pvmp3_get_scale_factors.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_get_scale_factors.o -c ../src/pvmp3_get_scale_factors.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_mpeg2_get_scale_data.o: ../src/pvmp3_mpeg2_get_scale_data.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_mpeg2_get_scale_data.o -c ../src/pvmp3_mpeg2_get_scale_data.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_mpeg2_get_scale_factors.o: ../src/pvmp3_mpeg2_get_scale_factors.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_mpeg2_get_scale_factors.o -c ../src/pvmp3_mpeg2_get_scale_factors.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_mpeg2_stereo_proc.o: ../src/pvmp3_mpeg2_stereo_proc.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_mpeg2_stereo_proc.o -c ../src/pvmp3_mpeg2_stereo_proc.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_huffman_decoding.o: ../src/pvmp3_huffman_decoding.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_huffman_decoding.o -c ../src/pvmp3_huffman_decoding.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_huffman_parsing.o: ../src/pvmp3_huffman_parsing.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_huffman_parsing.o -c ../src/pvmp3_huffman_parsing.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_tables.o: ../src/pvmp3_tables.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_tables.o -c ../src/pvmp3_tables.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_imdct_synth.o: ../src/pvmp3_imdct_synth.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_imdct_synth.o -c ../src/pvmp3_imdct_synth.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_mdct_6.o: ../src/pvmp3_mdct_6.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_mdct_6.o -c ../src/pvmp3_mdct_6.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_dct_6.o: ../src/pvmp3_dct_6.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_dct_6.o -c ../src/pvmp3_dct_6.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_poly_phase_synthesis.o: ../src/pvmp3_poly_phase_synthesis.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_poly_phase_synthesis.o -c ../src/pvmp3_poly_phase_synthesis.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_equalizer.o: ../src/pvmp3_equalizer.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_equalizer.o -c ../src/pvmp3_equalizer.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_seek_synch.o: ../src/pvmp3_seek_synch.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_seek_synch.o -c ../src/pvmp3_seek_synch.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_stereo_proc.o: ../src/pvmp3_stereo_proc.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_stereo_proc.o -c ../src/pvmp3_stereo_proc.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_reorder.o: ../src/pvmp3_reorder.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_reorder.o -c ../src/pvmp3_reorder.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_polyphase_filter_window.o: ../src/pvmp3_polyphase_filter_window.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_polyphase_filter_window.o -c ../src/pvmp3_polyphase_filter_window.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_mdct_18.o: ../src/pvmp3_mdct_18.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_mdct_18.o -c ../src/pvmp3_mdct_18.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_dct_9.o: ../src/pvmp3_dct_9.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_dct_9.o -c ../src/pvmp3_dct_9.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_dct_16.o: ../src/pvmp3_dct_16.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_dct_16.o -c ../src/pvmp3_dct_16.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/decwrapper.o: ../src/decwrapper.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/decwrapper.o -c ../src/decwrapper.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/oscl_mem_imp.o: ../../../../../oscl/oscl/osclmemory/src/oscl_mem_imp.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/oscl_mem_imp.o -c ../../../../../oscl/oscl/osclmemory/src/oscl_mem_imp.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_polyphase_filter_window_gcc.o: ../src/asm/pvmp3_polyphase_filter_window_gcc.s
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CC) $(CCFLAGS) -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_polyphase_filter_window_gcc.o -c ../src/asm/pvmp3_polyphase_filter_window_gcc.s

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_mdct_18_gcc.o: ../src/asm/pvmp3_mdct_18_gcc.s
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CC) $(CCFLAGS) -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_mdct_18_gcc.o -c ../src/asm/pvmp3_mdct_18_gcc.s

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_dct_9_gcc.o: ../src/asm/pvmp3_dct_9_gcc.s
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CC) $(CCFLAGS) -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_dct_9_gcc.o -c ../src/asm/pvmp3_dct_9_gcc.s

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_dct_16_gcc.o: ../src/asm/pvmp3_dct_16_gcc.s
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CC) $(CCFLAGS) -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp3_dct_16_gcc.o -c ../src/asm/pvmp3_dct_16_gcc.s

clean: 
	$(RM_DIR) \
	$(PLATFORM_BUILD_DIRECTORY)-lib \
	$(PLATFORM_BUILD_DIRECTORY)-obj
	
