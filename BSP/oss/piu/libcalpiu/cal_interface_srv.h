/**
* @file cal_interface_srv.h
* @brief Codec abstraction layer interface.
*
* File contains media components interface structs and enums.
* @author prashanthb@solomonsystech.com
*
*/
#ifndef CAL_INTERFACE_SRV_H
#define CAL_INTERFACE_SRV_H

/* enums */
/* enums related to mmct */
typedef enum
{
	MCU_DSP_COMMAND_ID_INVALID = -1,
	MCU_DSP_COMMAND_ID_ALSA_CPKT = 2,
	MCU_DSP_COMMAND_ID_ALSA_SPKT = 5,
	MCU_DSP_COMMAND_ID_MMCT = 6,
}MCU_DSP_COMMAND_ID_ENUM;

/* NOTE: add the respective codecs in between the range start and end enums 
   and update them respectively */
typedef enum
{
    CAL_MEDIACODEC_NONE = 0,
	CAL_MEDIA_AUD_RANGE_START = 1,
	CAL_AUD_AMR_NB = 1,
	CAL_AUD_AMR_WB = 2,
	CAL_AUD_AAC = 3,
	CAL_AUD_MP3 = 4,
	CAL_AUD_WMA = 5,
	CAL_AUD_FLAC = 6,
	CAL_AUD_VORBIS = 7,
	CAL_AUD_RMA = 8,
	CAL_MEDIA_AUD_RANGE_END = 8,
	CAL_MEDIA_IMG_RANGE_START = 9,
	CAL_IMG_JPEG = 9,
	CAL_IMG_BMP = 10,
	CAL_MEDIA_IMG_RANGE_END = 10,
	CAL_MEDIA_VID_RANGE_START = 11,
	CAL_VID_AVC = 11,
	CAL_VID_MPEG4 = 12,
	CAL_VID_MPEG4_ASP = 13,
	CAL_VID_RMV = 14,
	CAL_VID_WMV = 15,
	CAL_MEDIA_VID_RANGE_END = 15,
}CAL_MEDIA_CODEC_ENUM;

typedef enum
{
	CAL_MSG_MEDIA_RANGE_START = 0,
	CAL_MSG_INIT = 0,
	CAL_MSG_DECODE = 1,
	CAL_MSG_EXIT = 2,
	CAL_MSG_GETPARAMS = 3,
	CAL_MSG_SETPARAMS = 4,
	CAL_MSG_GETVERSION = 5,
	CAL_MSG_MEDIA_RANGE_END = 5,
	CAL_MSG_APB_RANGE_START = 6,
	CAL_MSG_APB_INIT = 6,
	CAL_MSG_APB_PAUSE = 7,
	CAL_MSG_APB_EXIT = 8,
	CAL_MSG_APB_CURRSTATE = 9,
	CAL_MSG_APB_RANGE_END = 9,
	CAL_MSG_PMU_RANGE_START = 10,
	CAL_MSG_PMU_CTRL_CMD = 10,
	CAL_MSG_PMU_RANGE_END = 11,
	CAL_MSG_MAX_IDS = 12,
}CAL_MSG_ID_ENUM;

typedef enum
{
	CAL_MSG_AUD_SPLCS_NONE = 0,
	CAL_MSG_AUD_SPLCS_DECODE_SEQHDR = 1,
	CAL_MSG_AUD_SPLCS_CLEAR_BUFF = 2,
	CAL_MSG_AUD_SPLCS_ENDOFSEQ = 3,
	CAL_MSG_AUD_SPLCS_PLC_ON = 4,
	CAL_MSG_AUD_SPLCS_PLC_OFF = 5,
}CAL_MSG_AUD_SPECIAL_CASE_ENUM;

typedef enum
{
	CAL_MSG_MEDIA_COMP_STATUS_INVALID = -2,
	CAL_MSG_MEDIA_COMP_STATUS_ERROR = -1,
	CAL_MSG_MEDIA_COMP_STATUS_OK = 0,
}CAL_MSG_MEDIA_COMP_STATUS_ENUM;

typedef enum
{
	CAL_MSG_AUD_DECODE_STATUS_EXTRADATAREQ = 1,
	CAL_MSG_AUD_DECODE_STATUS_ENDOFSTREAM = 3,

}CAL_MSG_AUD_DECODE_STATUS_ENUM;

typedef enum
{
	CAL_MSG_VID_SPLCS_NONE = 0,
	CAL_MSG_VID_SPLCS_DECODE_SEQHDR = 1,	
	CAL_MSG_VID_SPLCS_CLEAR_BUFF = 2,
	CAL_MSG_VID_SPLCS_ENDOFSEQ = 3,
	CAL_MSG_VID_SPLCS_SKIPFRAME_I = 4,
	CAL_MSG_VID_SPLCS_SKIPFRAME_IDR = 5,
	CAL_MSG_VID_SPLCS_SKIPFRAME_NEXT = 6,
}CAL_MSG_VID_SPECIAL_CASE_ENUM;


typedef enum
{
	CAL_MSG_VID_DECODE_STATUS_WARNING_NO_KEYFRAME = -11,
	CAL_MSG_VID_DECODE_STATUS_NOHEADER = -10,
	CAL_MSG_VID_DECODE_STATUS_NOFRAME = -9,
	CAL_MSG_VID_DECODE_STATUS_WARNING = -8,
	CAL_MSG_VID_DECODE_STATUS_ERROR_UNSUPPORTED_PIXELFRMT = -7,
	CAL_MSG_VID_DECODE_STATUS_ERROR_HEADER = -6,
	CAL_MSG_VID_DECODE_STATUS_ERROR_FRAME = -5,
	CAL_MSG_VID_DECODE_STATUS_ERROR_UNSUPPORTED = -4,
	CAL_MSG_VID_DECODE_STATUS_ERROR_CRITICAL_UNSUPPORTED = -3,
	CAL_MSG_VID_DECODE_STATUS_EXTRADATAREQ = 1,
	CAL_MSG_VID_DECODE_STATUS_SKIP_FRAME = 7,
	CAL_MSG_VID_DECODE_STATUS_ENDOFSTREAM = 8,
}CAL_MSG_VID_DECODE_STATUS_ENUM;

typedef enum {
    CAL_MSG_VID_DECODE_PIX_FMT_NONE= -1,
    CAL_MSG_VID_DECODE_PIX_FMT_YUV420P,   ///< Planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
    CAL_MSG_VID_DECODE_PIX_FMT_YUYV422,   ///< Packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr
    CAL_MSG_VID_DECODE_PIX_FMT_RGB24,     ///< Packed RGB 8:8:8, 24bpp, RGBRGB...
    CAL_MSG_VID_DECODE_PIX_FMT_BGR24,     ///< Packed RGB 8:8:8, 24bpp, BGRBGR...
    CAL_MSG_VID_DECODE_PIX_FMT_YUV422P,   ///< Planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
    CAL_MSG_VID_DECODE_PIX_FMT_YUV444P,   ///< Planar YUV 4:4:4, 24bpp, (1 Cr & Cb sample per 1x1 Y samples)
    CAL_MSG_VID_DECODE_PIX_FMT_RGB32,     ///< Packed RGB 8:8:8, 32bpp, (msb)8A 8R 8G 8B(lsb), in cpu endianness
    CAL_MSG_VID_DECODE_PIX_FMT_YUV410P,   ///< Planar YUV 4:1:0,  9bpp, (1 Cr & Cb sample per 4x4 Y samples)
    CAL_MSG_VID_DECODE_PIX_FMT_YUV411P,   ///< Planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples)
    CAL_MSG_VID_DECODE_PIX_FMT_RGB565,    ///< Packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), in cpu endianness
    CAL_MSG_VID_DECODE_PIX_FMT_RGB555,    ///< Packed RGB 5:5:5, 16bpp, (msb)1A 5R 5G 5B(lsb), in cpu endianness most significant bit to 1
    CAL_MSG_VID_DECODE_PIX_FMT_GRAY8,     ///<        Y        ,  8bpp
    CAL_MSG_VID_DECODE_PIX_FMT_MONOWHITE, ///<        Y        ,  1bpp, 1 is white
    CAL_MSG_VID_DECODE_PIX_FMT_MONOBLACK, ///<        Y        ,  1bpp, 0 is black
    CAL_MSG_VID_DECODE_PIX_FMT_PAL8,      ///< 8 bit with PIX_FMT_RGB32 palette
    CAL_MSG_VID_DECODE_PIX_FMT_YUVJ420P,  ///< Planar YUV 4:2:0, 12bpp, full scale (jpeg)
    CAL_MSG_VID_DECODE_PIX_FMT_YUVJ422P,  ///< Planar YUV 4:2:2, 16bpp, full scale (jpeg)
    CAL_MSG_VID_DECODE_PIX_FMT_YUVJ444P,  ///< Planar YUV 4:4:4, 24bpp, full scale (jpeg)
    CAL_MSG_VID_DECODE_PIX_FMT_XVMC_MPEG2_MC,///< XVideo Motion Acceleration via common packet passing(xvmc_render.h)
    CAL_MSG_VID_DECODE_PIX_FMT_XVMC_MPEG2_IDCT,
    CAL_MSG_VID_DECODE_PIX_FMT_UYVY422,   ///< Packed YUV 4:2:2, 16bpp, Cb Y0 Cr Y1
    CAL_MSG_VID_DECODE_PIX_FMT_UYYVYY411, ///< Packed YUV 4:1:1, 12bpp, Cb Y0 Y1 Cr Y2 Y3
    CAL_MSG_VID_DECODE_PIX_FMT_BGR32,     ///< Packed RGB 8:8:8, 32bpp, (msb)8A 8B 8G 8R(lsb), in cpu endianness
    CAL_MSG_VID_DECODE_PIX_FMT_BGR565,    ///< Packed RGB 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), in cpu endianness
    CAL_MSG_VID_DECODE_PIX_FMT_BGR555,    ///< Packed RGB 5:5:5, 16bpp, (msb)1A 5B 5G 5R(lsb), in cpu endianness most significant bit to 1
    CAL_MSG_VID_DECODE_PIX_FMT_BGR8,      ///< Packed RGB 3:3:2,  8bpp, (msb)2B 3G 3R(lsb)
    CAL_MSG_VID_DECODE_PIX_FMT_BGR4,      ///< Packed RGB 1:2:1,  4bpp, (msb)1B 2G 1R(lsb)
    CAL_MSG_VID_DECODE_PIX_FMT_BGR4_BYTE, ///< Packed RGB 1:2:1,  8bpp, (msb)1B 2G 1R(lsb)
    CAL_MSG_VID_DECODE_PIX_FMT_RGB8,      ///< Packed RGB 3:3:2,  8bpp, (msb)2R 3G 3B(lsb)
    CAL_MSG_VID_DECODE_PIX_FMT_RGB4,      ///< Packed RGB 1:2:1,  4bpp, (msb)2R 3G 3B(lsb)
    CAL_MSG_VID_DECODE_PIX_FMT_RGB4_BYTE, ///< Packed RGB 1:2:1,  8bpp, (msb)2R 3G 3B(lsb)
    CAL_MSG_VID_DECODE_PIX_FMT_NV12,      ///< Planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 for UV
    CAL_MSG_VID_DECODE_PIX_FMT_NV21,      ///< as above, but U and V bytes are swapped
    CAL_MSG_VID_DECODE_PIX_FMT_RGB32_1,   ///< Packed RGB 8:8:8, 32bpp, (msb)8R 8G 8B 8A(lsb), in cpu endianness
    CAL_MSG_VID_DECODE_PIX_FMT_BGR32_1,   ///< Packed RGB 8:8:8, 32bpp, (msb)8B 8G 8R 8A(lsb), in cpu endianness
    CAL_MSG_VID_DECODE_PIX_FMT_NB,        ///< number of pixel formats, DO NOT USE THIS if you want to link with shared libav* because the number of formats might differ between versions
}CAL_MSG_VID_DECODE_PXLFRMT_ENUM;

typedef enum 
{
	CAL_MSG_VID_VIDEOFORMAT_COMPONENT	= 0,
	CAL_MSG_VID_VIDEOFORMAT_PAL			= 1,
	CAL_MSG_VID_VIDEOFORMAT_NTSC		= 2,
	CAL_MSG_VID_VIDEOFORMAT_SECAM		= 3,
	CAL_MSG_VID_VIDEOFORMAT_MAC			= 4,
	CAL_MSG_VID_VIDEOFORMAT_UNSPECIFIED = 5
}CAL_MSG_VID_VIDEOFORMAT_ENUM;


typedef enum
{
	CAL_MSG_AUD_MODE_STEREO = 0,
	CAL_MSG_AUD_MODE_MONO = 1,
}CAL_MSG_AUD_MODE_ENUM;

/* enums for audio play back */

typedef enum
{
	CAL_APBMSG_PACKED = 0,
	CAL_APBMSG_UNPACKED = 1
} CAL_APBMSG_PACKED_FLAG;

typedef enum
{
	CAL_APBMSG_AULAW_NONE = 0,
	CAL_APBMSG_ALAW = 1,
	CAL_APBMSG_ULAW = 2,
} CAL_APBMSG_AULAW_FLAG;

typedef enum
{
	CAL_APBMSG_PHASE_SINGLE = 0,
	CAL_APBMSG_PHASE_DUAL = 2,
} CAL_APBMSG_PHASE_FLAG;

typedef enum
{
	CAL_APBMSG_LITTLE_ENDIAN = 0,
	CAL_APBMSG_BIG_ENDIAN = 1,
} CAL_APBMSG_ENDIAN_FLAG;

typedef enum
{
	CAL_APBMSG_INVALID = -3,
	CAL_APBMSG_HARDWARE_ERROR = -2,
	CAL_APBMSG_STATUS_ERROR = -1,
	CAL_APBMSG_STATUS_OK = 0,
} CAL_APBMSG_STATUS;

typedef enum
{
	CAL_MSG_PMU_SLEEP = 0,
	CAL_MSG_PMU_CHGCLK_HIGHEFF = 1,
	CAL_MSG_PMU_CHGCLK_MIDEFF = 2,
	CAL_MSG_PMU_CHGCLK_LOWEFF = 3,
}CAL_MSG_PMU_CTRL_CMD_ENUM;

typedef enum
{
	CAL_PMUMSG_STATUS_DSPPROC = -3,
	CAL_PMUMSG_STATUS_ERROR_PARAMS = -2,
	CAL_PMUMSG_STATUS_WARNING = -1,
	CAL_PMUMSG_STATUS_OK = 0,
} CAL_PMUMSG_STATUS_ENUM;


/* structure definitions */

/******************************************************************************/
/* Audio related structures                                                   */
/******************************************************************************/
/**
* @struct cal_mcumsg_aud_init
* @brief Audio init message structure from mcu
*
* Structure used by MCU while sending audio init message to DSP
*/
typedef struct {
	/** @var <size Size of this structure> */
	U32		size;
	/** @var <msg_id id which indicates init, decode, stop etc> */
	S8 		msg_id;
	/** @var <media_codec defines media and codec type> */
	S32 	media_codec;
	/** @var <codec_instance specifies instance of a codec> */
	U8 		codec_instance;
}cal_mcumsg_aud_init, *p_cal_mcumsg_aud_init;

/**
* @struct cal_dspmsg_aud_init
* @brief Audio init message structure from dsp
*
* Structure used by DSP while sending audio init message to MCU
*/
typedef struct {
	/** @var <size Size of this structure> */
	U32		size;
	/** @var <msg_id id which says init, decode, stop etc> */
	S8 		msg_id;
	/** @var <status status of init on DSP > */
	S32 	status;
	/** @var <media_codec media component type> */
	S32 	media_codec;
	/** @var <codec_instance instance of a perticular media component> */
	U8 		codec_instance;
	/** @var <info additional info that a codec needs to communicate> */
	S32 	info;
}cal_dspmsg_aud_init, *p_cal_dspmsg_aud_init;

/**
* @struct cal_mcumsg_aud_decode
* @brief Audio decode message structure from mcu
*
* Structure used by MCU while sending audio init message to DSP
*/
typedef struct {
	/** @var <size Size of this structure> */
	U32		size;
	/** @var <mdg_id id which indicates init/decode/exit etc> */
	S8 		msg_id;
	/** @var <media_codec defines media and codec type> */
	S32 	media_codec;
	/** @var <codec_instance specifies instance of a codec > */
	U8 		codec_instance;
	/** @var <inputbuff_addr address of input bit stream> */
	U32 	inputbuff_addr;
	/** @var <inputbuff_size size of input bitstream> */
	S32 	inputbuff_size;
	/** @var <outputbuff_addr address of output data> */
	U32 	outputbuff_addr;
	/** @var <outputbuff_size size of output data> */
	S32 	outputbuff_size;
	/** @var <block_align block align for audio data> */
	U16		block_align;
	/** @var <num_frames number of frames to decode, if -1 then this feild wont to considered> */
	S32 	num_frames;
	/** @var <pv_xtnd pointer to component specific struct> */
	void	*pv_xtnd;
	/** @var <special_case special case signalling flag> */
	S8 		special_case;
	/** @var <info additional info that a codec needs to communicate> */
	S32 	info;
}cal_mcumsg_aud_decode, *p_cal_mcumsg_aud_decode;

/**
* @struct cal_dspmsg_aud_decode
* @brief Audio decode message structure from dsp
*
* Structure used by DSP while sending audio init message to MCU
*/
typedef struct{
	/** @var <size Size of this structure> */
	U32		size;
	/** @var <msg_id id which says init, decode, stop etc> */
	S8 		msg_id;
	/** @var <status status of init on DSP > */
	S32 	status;
	/** @var <media_codec media component type> */
	S32 	media_codec;
	/** @var <codec_instance instance of a perticular media component> */
	U8 		codec_instance;
	/** @var <consumed_length_inp inp buff length consumed by decoder> */
	S32 	consumed_length_inp; 
	/** @var <consumed_length_out put buff length consumed by decoder> */
	S32 	consumed_length_out; 
	/** @var <timebase delta time from bitstream> */
	S32 	timebase; 	
	/** @var <frame_num> frame number decoded*/
	S32 	frame_num; 	
	/** @var <pv_aud_xtnd pointer to component specific struct> */
	void	*pv_aud_xtnd;	
	/** @var <info additional info that a codec needs to communicate> */
	S32 	info; 		
}cal_dspmsg_aud_decode, *p_cal_dspmsg_aud_decode;

/**
* @struct cal_mcumsg_aud_exit
* @brief Audio exit message structure from mcu
*
* Structure used by MCU while sending audio exit message to DSP
*/
typedef struct{
	/** @var <size Size of this structure> */
	U32		size;
	/** @var <mdg_id id which indicates init/decode/exit etc> */
	S8 		msg_id;
	/** @var <media_codec defines media and codec type> */
	S32 	media_codec;
	/** @var <codec_instance specifies instance of a codec > */
	U8 		codec_instance;
}cal_mcumsg_aud_exit, *p_cal_mcumsg_aud_exit;

/**
* @struct cal_dspmsg_aud_exit
* @brief Audio exit message structure from dsp
*
* Structure used by DSP while sending audio exit message to MCU
*/
typedef struct{
	/** @var <size Size of this structure> */
	U32		size;
	/** @var <msg_id id which says init, decode, stop etc> */
	S8 		msg_id;
	/** @var <status status of init on DSP > */
	S32 	status;
	/** @var <media_codec media component type> */
	S32 	media_codec;
	/** @var <codec_instance instance of a perticular media component> */
	U8 		codec_instance;
	/** @var <info additional info that a codec needs to communicate> */
	S32 	info;
}cal_dspmsg_aud_exit, *p_cal_dspmsg_aud_exit;


/**
* @struct cal_mcumsg_aud_getparam
* @brief Audio get param message structure from mcu
*
* Structure used by MCU while sending audio get param message to DSP
*/
typedef struct{
	/** @var <size Size of this structure> */
	U32		size;
	/** @var <mdg_id id which indicates init/decode/exit etc> */
	S8 		msg_id;
	/** @var <media_codec defines media and codec type> */
	S32 	media_codec;
	/** @var <codec_instance specifies instance of a codec > */
	U8 		codec_instance;
    /** @var <info additional info that a codec needs to communicate> */
	S32 	info;
}cal_mcumsg_aud_getparam, *p_cal_mcumsg_aud_getparam;

/**
* @struct cal_dspmsg_aud_getparam
* @brief Audio get param message structure from dsp
*
* Structure used by DSP while sending audio get param message to MCU
*/
typedef struct{
	/** @var <size Size of this structure> */
	U32		size;
	/** @var <msg_id id which says init, decode, stop etc> */
	S8 		msg_id;
	/** @var <status status of init on DSP > */
	S32 	status;
	/** @var <media_codec media component type> */
	S32 	media_codec;
	/** @var <codec_instance instance of a perticular media component> */
	U8 		codec_instance;
    /** @var <curr_buff_posn length consumed by decoder from starting of the buff> */
	S32 	curr_buff_posn; 
    /** @var <bit_rate bit rate of the audio stream> */
	S32 	bit_rate; 	
    /** @var <timebase delta time from bitstream> */
	S32 	timebase; 	
    /** @var <sampling_rate sampling rate> */
	S32 	sampling_rate; 	
    /** @var <num_channels number of channels> */
	S8 		num_channels; 
	/** @var <num_frames_decoded number of frames decoded> */
	S32 	num_frames_decoded; 	
	/** @var <frame_size defines number of bytes for one decode frame> */
	S32		frame_size;	
	/** @var <mode stream audio mode> */
	S8		mode;		
	/** @var <layer stream audio layer> */
	S8		layer;		
	/** @var <mode_ext stream mode extension> */
	S8		mode_ext;	
	/** @var <stream_ext stream extention> */
	S8		stream_ext;	
	/** @var <pv_aud_xtnd pointer to extended audio codec structure> */
	void	*pv_aud_xtnd;	
    /** @var <info additional info that a codec needs to communicate> */
	S32 	info; 		
}cal_dspmsg_aud_getparam, *p_cal_dspmsg_aud_getparam;


/**
*
* Structure used in cal_mcumsg_aud_setparam to wma specific param
*/
typedef struct{
	/** @var <encodeopt wma specific param>*/
	S16 wma_encodeoption_s16;
	/** @var <nversion wma specific param>*/
	S16 wma_nversion_s16;	
	/** @var <nsuperblockalignwma specific param>*/ 
	S32 wma_nsuperblockalign_s32;	
	/** @var <samplesperblock wma specific param>*/ 
	S32 wma_samplesperblock_s32;
	/** @var <AvgBytesPerSec wma specific param>*/	
	U32 wma_nAvgBytesPerSec_u32;
}cal_mcumsg_aud_setparam_wma_specific;


/**
* @struct cal_mcumsg_aud_setparam
* @brief Audio set param message structure from MCU
*
* Structure used by MCU while sending audio set param message to DSP
*/
typedef struct{
	/** @var <size Size of this structure> */
	U32		size;
	/** @var <mdg_id id which indicates init/decode/exit etc> */
	S8 		msg_id;
	/** @var <media_codec defines media and codec type> */
	S32 	media_codec;
	/** @var <codec_instance specifies instance of a codec > */
	U8 		codec_instance;
    	/** @var <bit_rate bit rate of the audio stream> */		
	S32 	bit_rate;
    	/** @var <num_channels number of channels> */
	S8 		num_channels;
	/** @var <sampling_rate sampling rate> */
	S32 	sampling_rate;		
	/** @var <mode stream audio mode> */
	S8		mode;
	/** @var <layer stream audio layer> */
	S8		layer;
	/** @var <mode_ext stream mode extension> */
	S8		mode_ext;
	/** @var <stream_ext stream extension> */
	S8		stream_ext;
	/** @var <wma specific parameters > */	
	cal_mcumsg_aud_setparam_wma_specific wma_specific;
	/** @var < used for clear input buffer, EOF in case of end, 
        skip frames if required sync etc> */
	S8 	    special_case;
	/** @var <frame_size defines number of bytes for one decode frame> */
	S32		frame_size;	
	/** @var <pv_aud_xtnd pointer to extended audio codec structure> */
	void	*pv_aud_xtnd;
    	/** @var <info additional info that a codec needs to communicate> */
	S32 	info;
}cal_mcumsg_aud_setparam, *p_cal_mcumsg_aud_setparam;

/**
* @struct cal_dspmsg_aud_setparam
* @brief Audio set param message structure from dsp
*
* Structure used by DSP while sending audio set param message to MCU
*/
typedef struct{
	/** @var <size Size of this structure> */
	U32		size;
	/** @var <msg_id id which says init, decode, stop etc> */
	S8 		msg_id;
	/** @var <status status of init on DSP > */
	S32 	status;
	/** @var <media_codec media component type> */
	S32 	media_codec;
	/** @var <codec_instance instance of a perticular media component> */
	U8 		codec_instance;
    /** @var <info additional info that a codec needs to communicate> */
	S32 	info;
}cal_dspmsg_aud_setparam, *p_cal_dspmsg_aud_setparam;

/**
* @struct cal_dspmsg_aud_getver
* @brief Audio get version message structure from MCU
*
* Structure used by MCU while sending audio get version message to DSP
*/
typedef struct{
	/** @var <size Size of this structure> */
	U32		size;
	/** @var <mdg_id id which indicates init/decode/exit etc> */
	S8 		msg_id;
	/** @var <media_codec defines media and codec type> */
	S32 	media_codec;
	/** @var <codec_instance specifies instance of a codec > */
	U8 		codec_instance;
}cal_mcumsg_aud_getver, *p_cal_mcumsg_aud_getver;

/**
* @struct cal_dspmsg_aud_getver
* @brief Audio get version message structure from dsp
*
* Structure used by DSP while sending audio get version message to MCU
*/
typedef struct{
	/** @var <size Size of this structure> */
	U32		size;
	/** @var <msg_id id which says init, decode, stop etc> */
	S8 		msg_id;
	/** @var <status status of init on DSP > */
	S32 	status;
	/** @var <media_codec media component type> */
	S32 	media_codec;
	/** @var <codec_instance instance of a perticular media component> */
	U8 		codec_instance;
	/** @var <ver_major version MSB> */
	U16		ver_major;
	/** @var <ver_minor version LSB> */
	U16		ver_minor;
	/** @var <ver_year year of modification> */
	U16		ver_year;
	/** @var <ver_date date of modification> */
	U16		ver_date;
    /** @var <info > additional info that a codec needs to communicate*/
	S32 	info;
}cal_dspmsg_aud_getver, *p_cal_dspmsg_aud_getver;

/******************************************************************************/
/* video related structures                                                   */
/******************************************************************************/

typedef struct{
	/** @var <size Size of this structure> */
	U32		size;
	/** @var <mdg_id id which indicates init/decode/exit etc> */
	S8 		msg_id;
	/** @var <media_codec defines media and codec type> */
	S32 		media_codec;
	/** @var <codec_instance specifies instance of a codec > */
	U8 		codec_instance;
	/** @var <video decoder reference buffer address> */
	U32 refbuff_addr;
	/** @var <video decoder reference buffer size> */
	S32 refbuff_size;
}cal_mcumsg_vid_init, *p_cal_mcumsg_vid_init;

typedef struct{
	/** @var <size Size of this structure> */
	U32		size;
	/** @var <msg_id id which says init, decode, stop etc> */
	S8 		msg_id;
	/** @var <status status of init on DSP > */
	S32 	status;
	/** @var <media_codec media component type> */
	S32 		media_codec;
	/** @var <codec_instance instance of a perticular media component> */
	U8 		codec_instance;
    /** @var <info > additional info that a codec needs to communicate*/
	S32 info; 			/* additional info that a codec needs to communicate */
	/** @var <video decoder reference buffer address> */
	U32 refbuff_addr;
	/** @var <video decoder reference buffer size> */
	S32 refbuff_size;
}cal_dspmsg_vid_init, *p_cal_dspmsg_vid_init;

typedef struct{
	/** @var <size Size of this structure> */
	U32		size;
	/** @var <mdg_id id which indicates init/decode/exit etc> */
	S8 		msg_id;
	/** @var <media_codec defines media and codec type> */
	S32 		media_codec;
	/** @var <codec_instance specifies instance of a codec > */
	U8 		codec_instance;
	/** @var <special_case special case signalling flag> */
	S8 	special_case; 		
	/** @var <input bitstream buffer address> */
	S32 inputbuff_addr; 
	/** @var <input buffer size> */	
	S32 inputbuff_size; 	
#if 0
	S32 outputbuff_addr; 	/* address of output data */
	S32 outputbuff_size; 	/* size of output data */
#else
	/** @var <address of output data, in case of packed frame, 
	                           this indicates starting point of the packed frame> */
	S32 outputbuff_addr_Y; 	
	/** @var <address of output U data > */	
	S32 outputbuff_addr_U; 	
	/** @var <address of output V data> */	
	S32 outputbuff_addr_V; 
	/** @var <stride of Y buffer, in case of packed frame, 
	                           this defines stride of the packed frame> */	
	S16 outbuff_Y_stride;  	
	/** @var <stride of U buffer> */	
	S16 outbuff_U_stride;  	
	/** @var <stride of V buffer> */	
	S16 outbuff_V_stride;  	
#endif
	/** @var <delta time from bitstream> */	
	S32 timebase; 
	/** @var <not supported right now> */	
	S16 hurry_up; 		
    /** @var <info > additional info that a codec needs to communicate*/
	S32 info;
}cal_mcumsg_vid_decode, *p_cal_mcumsg_vid_decode;

typedef struct{
	/** @var <size Size of this structure> */
	U32		size;
	/** @var <msg_id id which says init, decode, stop etc> */
	S8 		msg_id;
	/** @var <status status of init on DSP > */
	S32 	status;
	/** @var <media_codec media component type> */
	S32 		media_codec;
	/** @var <codec_instance instance of a perticular media component> */
	U8 		codec_instance;
	/** @var < address of output data, in case of packed frame, 
	                           this indicates starting point of the packed frame> */	
	S32 outputbuff_addr_Y; 
	/** @var <address of output data> */	
	S32 outputbuff_addr_U; 	
	/** @var <address of output data> */	
	S32 outputbuff_addr_V; 	
	/** @var <stride of Y buffer, in case of packed frame, 
	                           this defines stride of the packed frame> */	
	S16 outbuff_Y_stride;  	
	/** @var <stride of U buffer> */	
	S16 outbuff_U_stride;  	
	/** @var <stride of V buffer> */	
	S16 outbuff_V_stride;  
	/** @var <input buffer consumed> */	
	S32 consumed_length; 	
	/** @var <to specify either encountered seqhdr or skip frame> */	
	S8 	special_case; 		
	/** @var <delta time from bitstream in time units> */	
	S32 timebase;	 	
	/** @var <frame number maintained by codec> */	
	S32 frame_num;	
	/** @var <coded frame type, I/P/B etc> */	
	S16 coded_frame;  	
    /** @var <info > additional info that a codec needs to communicate*/
	S32 info;
}cal_dspmsg_vid_decode, *p_cal_dspmsg_vid_decode;


typedef struct{
	/** @var <size Size of this structure> */
	U32		size;
	/** @var <mdg_id id which indicates init/decode/exit etc> */
	S8 		msg_id;
	/** @var <media_codec defines media and codec type> */
	S32 		media_codec;
	/** @var <codec_instance specifies instance of a codec > */
	U8 		codec_instance;
	/** @var <additional info that a codec needs to communicate> */	
	S32 info; 		
}cal_mcumsg_vid_getparams, *p_cal_mcumsg_vid_getparams;

typedef struct{
	/** @var <size Size of this structure> */
	U32		size;
	/** @var <msg_id id which says init, decode, stop etc> */
	S8 		msg_id;
	/** @var <status status of init on DSP > */
	S32 	status;
	/** @var <media_codec media component type> */
	S32 		media_codec;
	/** @var <codec_instance instance of a perticular media component> */
	U8 		codec_instance;
	/** @var <time units, resolution> */	
	S32 timebase_units;		
	/** @var <current frame time in units, delta time, 
	                           increment for every decode> */	
	S32 timebase;	 	
	/** @var <width of the picture> */	
	S16 width; 			
	/** @var <height of the picture> */	
	S16 height; 		
	/** @var <format that needs to be supported> */	
	S8 	pixel_format; 	
	/** @var <frame number maintained by codec> */	
	S32 frame_num;	
	/** @var <profile of the stream> */	
	S16 profile;	
	/** @var <level of the stream> */	
	S16 level;			
	/** @var <output video format> */	
	S8  video_format;  
	
#if 0 /* will enale this after increasing the shared mem buffer size from 64 to 128 */	
	/** @var <color space> */	
	S32 color_primaries[8];  
	/** @var <transfer characteristics> */	
	S32 transfer_char;  	 
	/** @var <aspect ratio width value> */	
	S16 aspect_width;    
	/** @var <aspect ratio height value> */	
	S16 aspect_height;  	 
	/** @var <color space conversion coeffs> */	
	S32	conv_coeffs[3][4]; 
#endif	
	
    /** @var <info > additional info that a codec needs to communicate*/
	S32 info;
}cal_dspmsg_vid_getparams, *p_cal_dspmsg_vid_getparams;

typedef struct{
	/** @var <size Size of this structure> */
	U32		size;
	/** @var <mdg_id id which indicates init/decode/exit etc> */
	S8 		msg_id;
	/** @var <media_codec defines media and codec type> */
	S32 		media_codec;
	/** @var <codec_instance specifies instance of a codec > */
	U8 		codec_instance;
	/** @var <used for clear input buffer, EOF in case of end, 
	                       skip frame for video sync etc> */	
	S8 	special_case; 	
	/** @var <change the current time to this value> */	
	S32 time_startval; 
	/** @var <width of the picture> */	
	S16 width; 		
	/** @var <height of the picture> */	
	S16 height; 		
	/** @var <format that needs to be supported> */	
	S8 	pixel_format; 
	/** @var <error concealment flag to enable concealment> */	
	S8  error_conceal; 	
	/** @var < aspect ratio width of the picture > */	
	S16 aspect_width; 	
	/** @var <aspect ratio height of the picture> */	
	S16 aspect_height; 	
	/** @var <media standard profile> */	
	S16 profile; 		
	/** @var <media standard level> */	
	S16 level; 			
    /** @var <info > additional info that a codec needs to communicate*/
	S32 info;
}cal_mcumsg_vid_setparams, *p_cal_mcumsg_vid_setparams;

typedef struct{
	/** @var <size Size of this structure> */
	U32		size;
	/** @var <msg_id id which says init, decode, stop etc> */
	S8 		msg_id;
	/** @var <status status of init on DSP > */
	S32 	status;
	/** @var <media_codec media component type> */
	S32 		media_codec;
	/** @var <codec_instance instance of a perticular media component> */
	U8 		codec_instance;
	S32 info;			/* additional info that a codec needs to communicate */
}cal_dspmsg_vid_setparams, *p_cal_dspmsg_vid_setparams;

typedef struct{
	/** @var <size Size of this structure> */
	U32		size;
	/** @var <mdg_id id which indicates init/decode/exit etc> */
	S8 		msg_id;
	/** @var <media_codec defines media and codec type> */
	S32 		media_codec;
	/** @var <codec_instance specifies instance of a codec > */
	U8 		codec_instance;
}cal_mcumsg_vid_exit, *p_cal_mcumsg_vid_exit;

/**
* @struct cal_dspmsg_vid_getver
* @brief video get version message structure from MCU
*
* Structure used by MCU while sending video get version message to DSP
*/
typedef struct{
	/** @var <size Size of this structure> */
	U32		size;
	/** @var <msg_id id which says init, decode, stop etc> */
	S8 		msg_id;
	/** @var <status status of init on DSP > */
	S32 	status;
	/** @var <media_codec media component type> */
	S32 		media_codec;
	/** @var <codec_instance instance of a perticular media component> */
	U8 		codec_instance;
	S32 info;			/* additional info that a codec needs to communicate */
}cal_dspmsg_vid_exit, *p_cal_dspmsg_vid_exit;

/**
* @struct cal_dspmsg_vid_getver
* @brief video get version message structure from MCU
*
* Structure used by MCU while sending video get version message to DSP
*/
typedef struct{
	/** @var <size Size of this structure> */
	U32		size;
	/** @var <mdg_id id which indicates init/decode/exit etc> */
	S8 		msg_id;
	/** @var <media_codec defines media and codec type> */
	S32 		media_codec;
	/** @var <codec_instance specifies instance of a codec > */
	U8 		codec_instance;
}cal_mcumsg_vid_getver, *p_cal_mcumsg_vid_getver;

/**
* @struct cal_dspmsg_vid_getver
* @brief video get version message structure from dsp
*
* Structure used by DSP while sending video get version message to MCU
*/
typedef struct{
	/** @var <size Size of this structure> */
	U32		size;
	/** @var <msg_id id which says init, decode, stop etc> */
	S8 		msg_id;
	/** @var <status status of init on DSP > */
	S32 	status;
	/** @var <media_codec media component type> */
	S32 		media_codec;
	/** @var <codec_instance instance of a perticular media component> */
	U8 		codec_instance;
	/** @var <ver_major version MSB> */
	U16		ver_major;
	/** @var <ver_minor version LSB> */
	U16		ver_minor;
	/** @var <ver_year year of modification> */
	U16		ver_year;
	/** @var <ver_date date of modification> */
	U16		ver_date;
    /** @var <info > additional info that a codec needs to communicate*/
	S32 	info;
}cal_dspmsg_vid_getver, *p_cal_dspmsg_vid_getver;


/******************************************************************************/
/* audio playback definitions                                                 */
/******************************************************************************/
typedef struct
{
   U32  size; 			/* size of this structure */
   S8   msg_id; 		/* id which says start, reset, play, stop */
   S8   apb_instance;  /* specifies instance of audio play back */
   U32  buffer_addr; 	/* address of the APB buffer */
   U32  buff_size;       /* APB buffer size (must be multiple of frame size) */
   U32  buff_offset;    /* offset in the input buffer from which audio has to be played */
   U32  frm_len_synch;  /* number of PCM samples after which synch signal has to be sent */
   U32  curr_timestamp; /* current timestamp in terms of milliseconds */
   
   /* hardware specific params */
   S32  sampling_frquency;/* sampling frequency */
   S8   packed_flag;  	/* flag to indicate packed or unpacked */
   S8   au_flag; 		/* flag to indicate a law or mu law */
   S8   out_phase; 	    /* defines single or dual phase */
   S8   element_size; 	/* indicates each audio element size */
   S8   endian; 		/* define msb or lsb first */
   S32  info; 			/* additional info that a codec needs to communicate */	
}cal_mcumsg_apb_start;

typedef struct
{
   U32	size; 			/* size of this structure */
   S8   msg_id; 		/* id which says start, reset, play, stop */
   S8   apb_instance;  	/* specifies instance of audio play back */
   S32  status;         /* status of the message*/
   S32  info; 			/* additional info that a codec needs to communicate */	
}cal_dspmsg_apb_start;

/* note: reset should change any harware level param */
typedef struct
{
   U32  size; 			/* size of this structure */
   S8   msg_id; 		/* id which says start, reset, play, stop */
   S8   apb_instance;  /* specifies instance of audio play back */
   S8   special_flag;  /* special flag, not used now */ 
   S32  info; 			/* additional info that a codec needs to communicate */	
}cal_mcumsg_apb_pause;

typedef struct
{
   U32	size; 			/* size of this structure */
   S8   msg_id; 		/* id which says start, reset, play, stop */
   S8   apb_instance;  	/* specifies instance of audio play back */
   S32  status;         /* status of the message*/
   S32  info; 			/* additional info that a codec needs to communicate */	
}cal_dspmsg_apb_pause;

typedef struct
{
   U32	size; 			/* size of this structure */
   S8   msg_id; 		/* id which says start, reset, play, stop */
   S8   apb_instance;  	/* specifies instance of audio play back */
   S32  info; 			/* additional info that a codec needs to communicate */	
}cal_mcumsg_apb_stop;

typedef struct
{
   U32	size; 			/* size of this structure */
   S8   msg_id; 		/* id which says start, reset, play, stop */
   S8   apb_instance;  	/* specifies instance of audio play back */
   S32  status;         /* status of the message*/
   S32  info; 			/* additional info that a codec needs to communicate */	
}cal_dspmsg_apb_stop;

typedef struct
{
   U32	size; 			/* size of this structure */
   S8   msg_id; 		/* id which says start, reset, play, stop */
   S8   apb_instance;  	/* specifies instance of audio play back */
   S32  status;         /* status of the message*/
   U32  curr_timestamp; /* current time in the audio play back */
   U32  buff_posn;      /* APB buffer current position */
   U32  buff_size_consumed; /* APB buffer size consumed */
   S32  info; 			/* additional info that a codec needs to communicate */	
}cal_dspmsg_apb_currstate;

/******************************************************************************/
/* power management unit structure definitions                                */
/******************************************************************************/

typedef struct
{
	/** @var <size Size of this structure> */
	U32 size;
	/** @var <msg_id specifies PMU msg id> */
	S8  msg_id;
	/** @var <pmu_ctrl_cmd specifies the control command> */
	S8  pmu_ctrl_cmd;
	/** @var <pmu_freq intended clock of XS1200 master clock > */
	U32 pmu_freq;
	/** @var <pmu_clk_ratios specifies clk ratio of cxclk:xhclk:xpclk> */
	S8  pmu_clk_ratios;
    /** @var <info > additional info that MCU app needs to communicate*/
	S32 	info;
}cal_mcumsg_pmu_ctrl;

typedef struct
{
	/** @var <size Size of this structure> */
	U32 size;
	/** @var <msg_id specifies the control command> */
	S8  msg_id;
	/** @var <status status of MPU command > */
	S32 	status;
    /** @var <info > additional info that DSP app needs to communicate*/
	S32 	info;	
}cal_dspmsg_pmu_ctrl;

#endif /*#ifndef CAL_INTERFACE_SRV_H */

/* nothing beyond this */


