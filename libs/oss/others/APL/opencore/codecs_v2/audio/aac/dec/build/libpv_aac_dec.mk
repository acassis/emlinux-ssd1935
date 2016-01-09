#########################################################
### Makefile for building the packetvideo's aac decoder
### Date : 2009.12.25
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

ifeq ($(ARCH), intel)
CCFLAGS += -pipe -Wall -Wreturn-type -fno-exceptions -march=pentium -mcpu=pentium -O2
CCFLAGS += -fPIC -DPIC
CCFLAGS += -DTARGET_AAC_RAW_FRMAE_ONLY
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
endif
## Other platform ####

endif

ifeq ($(ARCH), arm)
## Cirrus EP93xx uclibc version ###########
## Cirrus EP93xx glibc version ############
## Star str8131 uclibc 0.9.29 version #####
## little-endian

##### don't use Envir.mk CFLAGS parameter
## CFLAGS =
##### 0. need to use -O2 (don't use -O3 because it hs problem at this platform) for optimization
ADDITIONAL_CFLAGS = \
	-O2 \
	-D_ARM \
	-DARM \
	-D$(PLATFORM)
CCFLAGS += $(ADDITIONAL_CFLAGS)
CXXFLAGS += $(ADDITIONAL_CXXFLAGS)
CCFLAGS += -DTARGET_AAC_RAW_FRMAE_ONLY
## Other platfrom ####

endif

CXXFLAGS += -DAAC_PLUS -DHQ_SBR -DPARAMETRICSTEREO -DOPTIMIZE_FOR_PERFORMANCE

##############################################################

SRCS = \
	../src/analysis_sub_band.cpp \
 	../src/apply_ms_synt.cpp \
 	../src/apply_tns.cpp \
 	../src/buf_getbits.cpp \
 	../src/byte_align.cpp \
 	../src/calc_auto_corr.cpp \
 	../src/calc_gsfb_table.cpp \
 	../src/calc_sbr_anafilterbank.cpp \
 	../src/calc_sbr_envelope.cpp \
 	../src/calc_sbr_synfilterbank.cpp \
 	../src/check_crc.cpp \
 	../src/dct16.cpp \
 	../src/dct64.cpp \
 	../src/decode_huff_cw_binary.cpp \
 	../src/decode_noise_floorlevels.cpp \
 	../src/decoder_aac.cpp \
 	../src/deinterleave.cpp \
 	../src/digit_reversal_tables.cpp \
 	../src/dst16.cpp \
 	../src/dst32.cpp \
 	../src/dst8.cpp \
 	../src/esc_iquant_scaling.cpp \
 	../src/extractframeinfo.cpp \
 	../src/fft_rx4_long.cpp \
 	../src/fft_rx4_short.cpp \
 	../src/fft_rx4_tables_fxp.cpp \
 	../src/find_adts_syncword.cpp \
 	../src/fwd_long_complex_rot.cpp \
 	../src/fwd_short_complex_rot.cpp \
 	../src/gen_rand_vector.cpp \
 	../src/get_adif_header.cpp \
 	../src/get_adts_header.cpp \
 	../src/get_audio_specific_config.cpp \
 	../src/get_dse.cpp \
 	../src/get_ele_list.cpp \
 	../src/get_ga_specific_config.cpp \
 	../src/get_ics_info.cpp \
 	../src/get_prog_config.cpp \
 	../src/get_pulse_data.cpp \
 	../src/get_sbr_bitstream.cpp \
 	../src/get_sbr_startfreq.cpp \
 	../src/get_sbr_stopfreq.cpp \
 	../src/get_tns.cpp \
 	../src/getfill.cpp \
 	../src/getgroup.cpp \
 	../src/getics.cpp \
 	../src/getmask.cpp \
 	../src/hcbtables_binary.cpp \
 	../src/huffcb.cpp \
 	../src/huffdecode.cpp \
 	../src/hufffac.cpp \
 	../src/huffspec_fxp.cpp \
 	../src/idct16.cpp \
 	../src/idct32.cpp \
 	../src/idct8.cpp \
 	../src/imdct_fxp.cpp \
 	../src/infoinit.cpp \
 	../src/init_sbr_dec.cpp \
 	../src/intensity_right.cpp \
 	../src/inv_long_complex_rot.cpp \
 	../src/inv_short_complex_rot.cpp \
 	../src/iquant_table.cpp \
 	../src/long_term_prediction.cpp \
 	../src/long_term_synthesis.cpp \
 	../src/lt_decode.cpp \
 	../src/mdct_fxp.cpp \
 	../src/mdct_tables_fxp.cpp \
 	../src/mdst.cpp \
 	../src/mix_radix_fft.cpp \
 	../src/ms_synt.cpp \
 	../src/pns_corr.cpp \
 	../src/pns_intensity_right.cpp \
 	../src/pns_left.cpp \
 	../src/ps_all_pass_filter_coeff.cpp \
 	../src/ps_all_pass_fract_delay_filter.cpp \
 	../src/ps_allocate_decoder.cpp \
 	../src/ps_applied.cpp \
 	../src/ps_bstr_decoding.cpp \
 	../src/ps_channel_filtering.cpp \
 	../src/ps_decode_bs_utils.cpp \
 	../src/ps_decorrelate.cpp \
 	../src/ps_fft_rx8.cpp \
 	../src/ps_hybrid_analysis.cpp \
 	../src/ps_hybrid_filter_bank_allocation.cpp \
 	../src/ps_hybrid_synthesis.cpp \
 	../src/ps_init_stereo_mixing.cpp \
 	../src/ps_pwr_transient_detection.cpp \
 	../src/ps_read_data.cpp \
 	../src/ps_stereo_processing.cpp \
 	../src/pulse_nc.cpp \
 	../src/pv_div.cpp \
 	../src/pv_log2.cpp \
 	../src/pv_normalize.cpp \
 	../src/pv_pow2.cpp \
 	../src/pv_sine.cpp \
 	../src/pv_sqrt.cpp \
 	../src/pvmp4audiodecoderconfig.cpp \
 	../src/pvmp4audiodecoderframe.cpp \
 	../src/pvmp4audiodecodergetmemrequirements.cpp \
 	../src/pvmp4audiodecoderinitlibrary.cpp \
 	../src/pvmp4audiodecoderresetbuffer.cpp \
 	../src/q_normalize.cpp \
 	../src/qmf_filterbank_coeff.cpp \
 	../src/sbr_aliasing_reduction.cpp \
 	../src/sbr_applied.cpp \
 	../src/sbr_code_book_envlevel.cpp \
 	../src/sbr_crc_check.cpp \
 	../src/sbr_create_limiter_bands.cpp \
 	../src/sbr_dec.cpp \
 	../src/sbr_decode_envelope.cpp \
 	../src/sbr_decode_huff_cw.cpp \
 	../src/sbr_downsample_lo_res.cpp \
 	../src/sbr_envelope_calc_tbl.cpp \
 	../src/sbr_envelope_unmapping.cpp \
 	../src/sbr_extract_extended_data.cpp \
 	../src/sbr_find_start_andstop_band.cpp \
 	../src/sbr_generate_high_freq.cpp \
 	../src/sbr_get_additional_data.cpp \
 	../src/sbr_get_cpe.cpp \
 	../src/sbr_get_dir_control_data.cpp \
 	../src/sbr_get_envelope.cpp \
 	../src/sbr_get_header_data.cpp \
 	../src/sbr_get_noise_floor_data.cpp \
 	../src/sbr_get_sce.cpp \
 	../src/sbr_inv_filt_levelemphasis.cpp \
 	../src/sbr_open.cpp \
 	../src/sbr_read_data.cpp \
 	../src/sbr_requantize_envelope_data.cpp \
 	../src/sbr_reset_dec.cpp \
 	../src/sbr_update_freq_scale.cpp \
 	../src/set_mc_info.cpp \
 	../src/sfb.cpp \
 	../src/shellsort.cpp \
 	../src/synthesis_sub_band.cpp \
 	../src/tns_ar_filter.cpp \
 	../src/tns_decode_coef.cpp \
 	../src/tns_inv_filter.cpp \
 	../src/trans4m_freq_2_time_fxp.cpp \
 	../src/trans4m_time_2_freq_fxp.cpp \
 	../src/unpack_idx.cpp \
 	../src/window_tables_fxp.cpp \
 	../src/pvmp4setaudioconfig.cpp \
 	../util/getactualaacconfig/src/getactualaacconfig.cpp \
	../src/decwrapper.cpp

OBJS = $(COMPILED_OBJS) $(SOURCE_OBJS)

COMPILED_OBJS = \
	$(PLATFORM_BUILD_DIRECTORY)-obj/analysis_sub_band.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/apply_ms_synt.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/apply_tns.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/buf_getbits.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/byte_align.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/calc_auto_corr.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/calc_gsfb_table.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/calc_sbr_anafilterbank.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/calc_sbr_envelope.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/calc_sbr_synfilterbank.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/check_crc.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/dct16.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/dct64.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/decode_huff_cw_binary.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/decode_noise_floorlevels.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/decoder_aac.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/deinterleave.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/digit_reversal_tables.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/dst16.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/dst32.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/dst8.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/esc_iquant_scaling.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/extractframeinfo.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/fft_rx4_long.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/fft_rx4_short.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/fft_rx4_tables_fxp.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/find_adts_syncword.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/fwd_long_complex_rot.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/fwd_short_complex_rot.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/gen_rand_vector.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/get_adif_header.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/get_adts_header.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/get_audio_specific_config.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/get_dse.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/get_ele_list.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/get_ga_specific_config.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/get_ics_info.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/get_prog_config.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/get_pulse_data.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/get_sbr_bitstream.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/get_sbr_startfreq.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/get_sbr_stopfreq.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/get_tns.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/getfill.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/getgroup.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/getics.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/getmask.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/hcbtables_binary.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/huffcb.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/huffdecode.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/hufffac.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/huffspec_fxp.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/idct16.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/idct32.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/idct8.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/imdct_fxp.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/infoinit.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/init_sbr_dec.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/intensity_right.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/inv_long_complex_rot.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/inv_short_complex_rot.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/iquant_table.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/long_term_prediction.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/long_term_synthesis.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/lt_decode.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/mdct_fxp.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/mdct_tables_fxp.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/mdst.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/mix_radix_fft.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/ms_synt.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/pns_corr.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/pns_intensity_right.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/pns_left.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/ps_all_pass_filter_coeff.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/ps_all_pass_fract_delay_filter.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/ps_allocate_decoder.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/ps_applied.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/ps_bstr_decoding.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/ps_channel_filtering.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/ps_decode_bs_utils.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/ps_decorrelate.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/ps_fft_rx8.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/ps_hybrid_analysis.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/ps_hybrid_filter_bank_allocation.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/ps_hybrid_synthesis.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/ps_init_stereo_mixing.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/ps_pwr_transient_detection.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/ps_read_data.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/ps_stereo_processing.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/pulse_nc.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/pv_div.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/pv_log2.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/pv_normalize.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/pv_pow2.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/pv_sine.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/pv_sqrt.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp4audiodecoderconfig.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp4audiodecoderframe.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp4audiodecodergetmemrequirements.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp4audiodecoderinitlibrary.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp4audiodecoderresetbuffer.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/q_normalize.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/qmf_filterbank_coeff.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_aliasing_reduction.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_applied.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_code_book_envlevel.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_crc_check.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_create_limiter_bands.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_dec.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_decode_envelope.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_decode_huff_cw.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_downsample_lo_res.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_envelope_calc_tbl.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_envelope_unmapping.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_extract_extended_data.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_find_start_andstop_band.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_generate_high_freq.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_get_additional_data.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_get_cpe.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_get_dir_control_data.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_get_envelope.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_get_header_data.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_get_noise_floor_data.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_get_sce.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_inv_filt_levelemphasis.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_open.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_read_data.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_requantize_envelope_data.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_reset_dec.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_update_freq_scale.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/set_mc_info.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/sfb.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/shellsort.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/synthesis_sub_band.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/tns_ar_filter.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/tns_decode_coef.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/tns_inv_filter.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/trans4m_freq_2_time_fxp.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/trans4m_time_2_freq_fxp.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/unpack_idx.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/window_tables_fxp.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp4setaudioconfig.o \
 	$(PLATFORM_BUILD_DIRECTORY)-obj/getactualaacconfig.o \
	$(PLATFORM_BUILD_DIRECTORY)-obj/decwrapper.o

SOURCE_OBJS =

INCLUDES = \
	-I../include \
	-I../src \
	-I../util/getactualaacconfig/include \
	-I../../../../../oscl/oscl/config/linux \
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

DYNAMIC_LIBS =

.SUFFIXES: .cpp .c .s .S

.c.o: 
	$(CC) $(CCFLAGS) -o  $@ -c $<

.cpp.o: 
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o  $@ -c $<

.s.o: 
	$(CC) $(CCFLAGS) -o  $@ -c $<

.S.o: 
	$(CC) $(CCFLAGS) -o  $@ -c $<

all: $(PLATFORM_BUILD_DIRECTORY)-obj pv_aac_dec.a

$(PLATFORM_BUILD_DIRECTORY)-obj: 
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-lib || mkdir $(PLATFORM_BUILD_DIRECTORY)-lib
	
all_objects: $(OBJS)

pv_aac_dec.a: $(OBJS)
	## static library
	$(AR) cr  $(PLATFORM_BUILD_DIRECTORY)-lib/pv_aac_dec.a $(OBJS)
	$(RANLIB) $(PLATFORM_BUILD_DIRECTORY)-lib/pv_aac_dec.a
	## share library
	$(CC) $(LDFLAGS) -shared -o $(PLATFORM_BUILD_DIRECTORY)-lib/libpv_aac_dec.so $(OBJS) -lgcc

$(PLATFORM_BUILD_DIRECTORY)-obj/analysis_sub_band.o: ../src/analysis_sub_band.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/analysis_sub_band.o -c ../src/analysis_sub_band.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/apply_ms_synt.o: ../src/apply_ms_synt.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/apply_ms_synt.o -c ../src/apply_ms_synt.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/apply_tns.o: ../src/apply_tns.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/apply_tns.o -c ../src/apply_tns.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/buf_getbits.o: ../src/buf_getbits.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/buf_getbits.o -c ../src/buf_getbits.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/byte_align.o: ../src/byte_align.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/byte_align.o -c ../src/byte_align.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/calc_auto_corr.o: ../src/calc_auto_corr.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/calc_auto_corr.o -c ../src/calc_auto_corr.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/calc_gsfb_table.o: ../src/calc_gsfb_table.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/calc_gsfb_table.o -c ../src/calc_gsfb_table.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/calc_sbr_anafilterbank.o: ../src/calc_sbr_anafilterbank.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/calc_sbr_anafilterbank.o -c ../src/calc_sbr_anafilterbank.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/calc_sbr_envelope.o: ../src/calc_sbr_envelope.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/calc_sbr_envelope.o -c ../src/calc_sbr_envelope.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/calc_sbr_synfilterbank.o: ../src/calc_sbr_synfilterbank.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/calc_sbr_synfilterbank.o -c ../src/calc_sbr_synfilterbank.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/check_crc.o: ../src/check_crc.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/check_crc.o -c ../src/check_crc.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/dct16.o: ../src/dct16.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/dct16.o -c ../src/dct16.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/dct64.o: ../src/dct64.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/dct64.o -c ../src/dct64.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/decode_huff_cw_binary.o: ../src/decode_huff_cw_binary.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/decode_huff_cw_binary.o -c ../src/decode_huff_cw_binary.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/decode_noise_floorlevels.o: ../src/decode_noise_floorlevels.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/decode_noise_floorlevels.o -c ../src/decode_noise_floorlevels.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/decoder_aac.o: ../src/decoder_aac.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/decoder_aac.o -c ../src/decoder_aac.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/deinterleave.o: ../src/deinterleave.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/deinterleave.o -c ../src/deinterleave.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/digit_reversal_tables.o: ../src/digit_reversal_tables.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/digit_reversal_tables.o -c ../src/digit_reversal_tables.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/dst16.o: ../src/dst16.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/dst16.o -c ../src/dst16.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/dst32.o: ../src/dst32.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/dst32.o -c ../src/dst32.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/dst8.o: ../src/dst8.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/dst8.o -c ../src/dst8.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/esc_iquant_scaling.o: ../src/esc_iquant_scaling.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/esc_iquant_scaling.o -c ../src/esc_iquant_scaling.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/extractframeinfo.o: ../src/extractframeinfo.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/extractframeinfo.o -c ../src/extractframeinfo.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/fft_rx4_long.o: ../src/fft_rx4_long.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/fft_rx4_long.o -c ../src/fft_rx4_long.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/fft_rx4_short.o: ../src/fft_rx4_short.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/fft_rx4_short.o -c ../src/fft_rx4_short.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/fft_rx4_tables_fxp.o: ../src/fft_rx4_tables_fxp.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/fft_rx4_tables_fxp.o -c ../src/fft_rx4_tables_fxp.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/find_adts_syncword.o: ../src/find_adts_syncword.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/find_adts_syncword.o -c ../src/find_adts_syncword.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/fwd_long_complex_rot.o: ../src/fwd_long_complex_rot.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/fwd_long_complex_rot.o -c ../src/fwd_long_complex_rot.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/fwd_short_complex_rot.o: ../src/fwd_short_complex_rot.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/fwd_short_complex_rot.o -c ../src/fwd_short_complex_rot.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/gen_rand_vector.o: ../src/gen_rand_vector.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/gen_rand_vector.o -c ../src/gen_rand_vector.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/get_adif_header.o: ../src/get_adif_header.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/get_adif_header.o -c ../src/get_adif_header.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/get_adts_header.o: ../src/get_adts_header.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/get_adts_header.o -c ../src/get_adts_header.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/get_audio_specific_config.o: ../src/get_audio_specific_config.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/get_audio_specific_config.o -c ../src/get_audio_specific_config.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/get_dse.o: ../src/get_dse.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/get_dse.o -c ../src/get_dse.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/get_ele_list.o: ../src/get_ele_list.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/get_ele_list.o -c ../src/get_ele_list.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/get_ga_specific_config.o: ../src/get_ga_specific_config.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/get_ga_specific_config.o -c ../src/get_ga_specific_config.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/get_ics_info.o: ../src/get_ics_info.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/get_ics_info.o -c ../src/get_ics_info.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/get_prog_config.o: ../src/get_prog_config.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/get_prog_config.o -c ../src/get_prog_config.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/get_pulse_data.o: ../src/get_pulse_data.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/get_pulse_data.o -c ../src/get_pulse_data.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/get_sbr_bitstream.o: ../src/get_sbr_bitstream.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/get_sbr_bitstream.o -c ../src/get_sbr_bitstream.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/get_sbr_startfreq.o: ../src/get_sbr_startfreq.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/get_sbr_startfreq.o -c ../src/get_sbr_startfreq.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/get_sbr_stopfreq.o: ../src/get_sbr_stopfreq.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/get_sbr_stopfreq.o -c ../src/get_sbr_stopfreq.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/get_tns.o: ../src/get_tns.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/get_tns.o -c ../src/get_tns.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/getfill.o: ../src/getfill.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/getfill.o -c ../src/getfill.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/getgroup.o: ../src/getgroup.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/getgroup.o -c ../src/getgroup.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/getics.o: ../src/getics.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/getics.o -c ../src/getics.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/getmask.o: ../src/getmask.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/getmask.o -c ../src/getmask.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/hcbtables_binary.o: ../src/hcbtables_binary.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/hcbtables_binary.o -c ../src/hcbtables_binary.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/huffcb.o: ../src/huffcb.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/huffcb.o -c ../src/huffcb.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/huffdecode.o: ../src/huffdecode.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/huffdecode.o -c ../src/huffdecode.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/hufffac.o: ../src/hufffac.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/hufffac.o -c ../src/hufffac.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/huffspec_fxp.o: ../src/huffspec_fxp.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/huffspec_fxp.o -c ../src/huffspec_fxp.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/idct16.o: ../src/idct16.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/idct16.o -c ../src/idct16.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/idct32.o: ../src/idct32.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/idct32.o -c ../src/idct32.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/idct8.o: ../src/idct8.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/idct8.o -c ../src/idct8.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/imdct_fxp.o: ../src/imdct_fxp.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/imdct_fxp.o -c ../src/imdct_fxp.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/infoinit.o: ../src/infoinit.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/infoinit.o -c ../src/infoinit.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/init_sbr_dec.o: ../src/init_sbr_dec.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/init_sbr_dec.o -c ../src/init_sbr_dec.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/intensity_right.o: ../src/intensity_right.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/intensity_right.o -c ../src/intensity_right.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/inv_long_complex_rot.o: ../src/inv_long_complex_rot.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/inv_long_complex_rot.o -c ../src/inv_long_complex_rot.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/inv_short_complex_rot.o: ../src/inv_short_complex_rot.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/inv_short_complex_rot.o -c ../src/inv_short_complex_rot.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/iquant_table.o: ../src/iquant_table.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/iquant_table.o -c ../src/iquant_table.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/long_term_prediction.o: ../src/long_term_prediction.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/long_term_prediction.o -c ../src/long_term_prediction.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/long_term_synthesis.o: ../src/long_term_synthesis.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/long_term_synthesis.o -c ../src/long_term_synthesis.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/lt_decode.o: ../src/lt_decode.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/lt_decode.o -c ../src/lt_decode.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/mdct_fxp.o: ../src/mdct_fxp.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/mdct_fxp.o -c ../src/mdct_fxp.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/mdct_tables_fxp.o: ../src/mdct_tables_fxp.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/mdct_tables_fxp.o -c ../src/mdct_tables_fxp.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/mdst.o: ../src/mdst.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/mdst.o -c ../src/mdst.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/mix_radix_fft.o: ../src/mix_radix_fft.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/mix_radix_fft.o -c ../src/mix_radix_fft.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/ms_synt.o: ../src/ms_synt.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/ms_synt.o -c ../src/ms_synt.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pns_corr.o: ../src/pns_corr.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pns_corr.o -c ../src/pns_corr.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pns_intensity_right.o: ../src/pns_intensity_right.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pns_intensity_right.o -c ../src/pns_intensity_right.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pns_left.o: ../src/pns_left.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pns_left.o -c ../src/pns_left.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/ps_all_pass_filter_coeff.o: ../src/ps_all_pass_filter_coeff.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/ps_all_pass_filter_coeff.o -c ../src/ps_all_pass_filter_coeff.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/ps_all_pass_fract_delay_filter.o: ../src/ps_all_pass_fract_delay_filter.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/ps_all_pass_fract_delay_filter.o -c ../src/ps_all_pass_fract_delay_filter.cpp
	
$(PLATFORM_BUILD_DIRECTORY)-obj/ps_allocate_decoder.o: ../src/ps_allocate_decoder.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/ps_allocate_decoder.o -c ../src/ps_allocate_decoder.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/ps_applied.o: ../src/ps_applied.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/ps_applied.o -c ../src/ps_applied.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/ps_bstr_decoding.o: ../src/ps_bstr_decoding.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/ps_bstr_decoding.o -c ../src/ps_bstr_decoding.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/ps_channel_filtering.o: ../src/ps_channel_filtering.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/ps_channel_filtering.o -c ../src/ps_channel_filtering.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/ps_decode_bs_utils.o: ../src/ps_decode_bs_utils.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/ps_decode_bs_utils.o -c ../src/ps_decode_bs_utils.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/ps_decorrelate.o: ../src/ps_decorrelate.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/ps_decorrelate.o -c ../src/ps_decorrelate.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/ps_fft_rx8.o: ../src/ps_fft_rx8.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/ps_fft_rx8.o -c ../src/ps_fft_rx8.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/ps_hybrid_analysis.o: ../src/ps_hybrid_analysis.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/ps_hybrid_analysis.o -c ../src/ps_hybrid_analysis.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/ps_hybrid_filter_bank_allocation.o: ../src/ps_hybrid_filter_bank_allocation.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/ps_hybrid_filter_bank_allocation.o -c ../src/ps_hybrid_filter_bank_allocation.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/ps_hybrid_synthesis.o: ../src/ps_hybrid_synthesis.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/ps_hybrid_synthesis.o -c ../src/ps_hybrid_synthesis.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/ps_init_stereo_mixing.o: ../src/ps_init_stereo_mixing.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/ps_init_stereo_mixing.o -c ../src/ps_init_stereo_mixing.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/ps_pwr_transient_detection.o: ../src/ps_pwr_transient_detection.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/ps_pwr_transient_detection.o -c ../src/ps_pwr_transient_detection.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/ps_read_data.o: ../src/ps_read_data.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/ps_read_data.o -c ../src/ps_read_data.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/ps_stereo_processing.o: ../src/ps_stereo_processing.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/ps_stereo_processing.o -c ../src/ps_stereo_processing.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pulse_nc.o: ../src/pulse_nc.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pulse_nc.o -c ../src/pulse_nc.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pv_div.o: ../src/pv_div.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pv_div.o -c ../src/pv_div.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pv_log2.o: ../src/pv_log2.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pv_log2.o -c ../src/pv_log2.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pv_normalize.o: ../src/pv_normalize.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pv_normalize.o -c ../src/pv_normalize.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pv_pow2.o: ../src/pv_pow2.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pv_pow2.o -c ../src/pv_pow2.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pv_sine.o: ../src/pv_sine.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pv_sine.o -c ../src/pv_sine.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pv_sqrt.o: ../src/pv_sqrt.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pv_sqrt.o -c ../src/pv_sqrt.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp4audiodecoderconfig.o: ../src/pvmp4audiodecoderconfig.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp4audiodecoderconfig.o -c ../src/pvmp4audiodecoderconfig.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp4audiodecoderframe.o: ../src/pvmp4audiodecoderframe.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp4audiodecoderframe.o -c ../src/pvmp4audiodecoderframe.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp4audiodecodergetmemrequirements.o: ../src/pvmp4audiodecodergetmemrequirements.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp4audiodecodergetmemrequirements.o -c ../src/pvmp4audiodecodergetmemrequirements.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp4audiodecoderinitlibrary.o: ../src/pvmp4audiodecoderinitlibrary.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp4audiodecoderinitlibrary.o -c ../src/pvmp4audiodecoderinitlibrary.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp4audiodecoderresetbuffer.o: ../src/pvmp4audiodecoderresetbuffer.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp4audiodecoderresetbuffer.o -c ../src/pvmp4audiodecoderresetbuffer.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/q_normalize.o: ../src/q_normalize.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/q_normalize.o -c ../src/q_normalize.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/qmf_filterbank_coeff.o: ../src/qmf_filterbank_coeff.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/qmf_filterbank_coeff.o -c ../src/qmf_filterbank_coeff.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_aliasing_reduction.o: ../src/sbr_aliasing_reduction.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/sbr_aliasing_reduction.o -c ../src/sbr_aliasing_reduction.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_applied.o: ../src/sbr_applied.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/sbr_applied.o -c ../src/sbr_applied.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_code_book_envlevel.o: ../src/sbr_code_book_envlevel.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/sbr_code_book_envlevel.o -c ../src/sbr_code_book_envlevel.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_crc_check.o: ../src/sbr_crc_check.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/sbr_crc_check.o -c ../src/sbr_crc_check.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_create_limiter_bands.o: ../src/sbr_create_limiter_bands.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/sbr_create_limiter_bands.o -c ../src/sbr_create_limiter_bands.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_dec.o: ../src/sbr_dec.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/sbr_dec.o -c ../src/sbr_dec.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_decode_envelope.o: ../src/sbr_decode_envelope.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/sbr_decode_envelope.o -c ../src/sbr_decode_envelope.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_decode_huff_cw.o: ../src/sbr_decode_huff_cw.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/sbr_decode_huff_cw.o -c ../src/sbr_decode_huff_cw.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_downsample_lo_res.o: ../src/sbr_downsample_lo_res.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/sbr_downsample_lo_res.o -c ../src/sbr_downsample_lo_res.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_envelope_calc_tbl.o: ../src/sbr_envelope_calc_tbl.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/sbr_envelope_calc_tbl.o -c ../src/sbr_envelope_calc_tbl.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_envelope_unmapping.o: ../src/sbr_envelope_unmapping.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/sbr_envelope_unmapping.o -c ../src/sbr_envelope_unmapping.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_extract_extended_data.o: ../src/sbr_extract_extended_data.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/sbr_extract_extended_data.o -c ../src/sbr_extract_extended_data.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_find_start_andstop_band.o: ../src/sbr_find_start_andstop_band.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/sbr_find_start_andstop_band.o -c ../src/sbr_find_start_andstop_band.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_generate_high_freq.o: ../src/sbr_generate_high_freq.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/sbr_generate_high_freq.o -c ../src/sbr_generate_high_freq.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_get_additional_data.o: ../src/sbr_get_additional_data.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/sbr_get_additional_data.o -c ../src/sbr_get_additional_data.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_get_cpe.o: ../src/sbr_get_cpe.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/sbr_get_cpe.o -c ../src/sbr_get_cpe.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_get_dir_control_data.o: ../src/sbr_get_dir_control_data.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/sbr_get_dir_control_data.o -c ../src/sbr_get_dir_control_data.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_get_envelope.o: ../src/sbr_get_envelope.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/sbr_get_envelope.o -c ../src/sbr_get_envelope.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_get_header_data.o: ../src/sbr_get_header_data.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/sbr_get_header_data.o -c ../src/sbr_get_header_data.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_get_noise_floor_data.o: ../src/sbr_get_noise_floor_data.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/sbr_get_noise_floor_data.o -c ../src/sbr_get_noise_floor_data.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_get_sce.o: ../src/sbr_get_sce.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/sbr_get_sce.o -c ../src/sbr_get_sce.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_inv_filt_levelemphasis.o: ../src/sbr_inv_filt_levelemphasis.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/sbr_inv_filt_levelemphasis.o -c ../src/sbr_inv_filt_levelemphasis.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_open.o: ../src/sbr_open.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/sbr_open.o -c ../src/sbr_open.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_read_data.o: ../src/sbr_read_data.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/sbr_read_data.o -c ../src/sbr_read_data.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_requantize_envelope_data.o: ../src/sbr_requantize_envelope_data.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/sbr_requantize_envelope_data.o -c ../src/sbr_requantize_envelope_data.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_reset_dec.o: ../src/sbr_reset_dec.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/sbr_reset_dec.o -c ../src/sbr_reset_dec.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/sbr_update_freq_scale.o: ../src/sbr_update_freq_scale.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/sbr_update_freq_scale.o -c ../src/sbr_update_freq_scale.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/set_mc_info.o: ../src/set_mc_info.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/set_mc_info.o -c ../src/set_mc_info.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/sfb.o: ../src/sfb.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/sfb.o -c ../src/sfb.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/shellsort.o: ../src/shellsort.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/shellsort.o -c ../src/shellsort.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/synthesis_sub_band.o: ../src/synthesis_sub_band.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/synthesis_sub_band.o -c ../src/synthesis_sub_band.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/tns_ar_filter.o: ../src/tns_ar_filter.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/tns_ar_filter.o -c ../src/tns_ar_filter.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/tns_decode_coef.o: ../src/tns_decode_coef.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/tns_decode_coef.o -c ../src/tns_decode_coef.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/tns_inv_filter.o: ../src/tns_inv_filter.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/tns_inv_filter.o -c ../src/tns_inv_filter.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/trans4m_freq_2_time_fxp.o: ../src/trans4m_freq_2_time_fxp.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/trans4m_freq_2_time_fxp.o -c ../src/trans4m_freq_2_time_fxp.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/trans4m_time_2_freq_fxp.o: ../src/trans4m_time_2_freq_fxp.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/trans4m_time_2_freq_fxp.o -c ../src/trans4m_time_2_freq_fxp.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/unpack_idx.o: ../src/unpack_idx.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/unpack_idx.o -c ../src/unpack_idx.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/window_tables_fxp.o: ../src/window_tables_fxp.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/window_tables_fxp.o -c ../src/window_tables_fxp.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/pvmp4setaudioconfig.o: ../src/pvmp4setaudioconfig.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/pvmp4setaudioconfig.o -c ../src/pvmp4setaudioconfig.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/getactualaacconfig.o: ../util/getactualaacconfig/src/getactualaacconfig.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/getactualaacconfig.o -c ../util/getactualaacconfig/src/getactualaacconfig.cpp

$(PLATFORM_BUILD_DIRECTORY)-obj/decwrapper.o: ../src/decwrapper.cpp
	-@test -d $(PLATFORM_BUILD_DIRECTORY)-obj || mkdir $(PLATFORM_BUILD_DIRECTORY)-obj
	$(CXX) $(CXXFLAGS) -fPIC -DPIC -o $(PLATFORM_BUILD_DIRECTORY)-obj/decwrapper.o -c ../src/decwrapper.cpp

clean: 
	$(RM_DIR) \
	$(PLATFORM_BUILD_DIRECTORY)-lib \
	$(PLATFORM_BUILD_DIRECTORY)-obj
	
