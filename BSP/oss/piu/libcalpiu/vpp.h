/**
@file		vpp.h - header file for vpp.c
@author		shaowei@solomon-systech.com, kuganv@solomon-systech.com
@version	0.1
@date		16May07
@todo
*/

#ifndef _SHAOWEI_VPP_H
#define _SHAOWEI_VPP_H

#define VPP_F_ALL	~0x0
#define VPP_F_INADDR	1		/**< re-configure input image buffer address */
#define VPP_F_OUTADDR	1<<1		/**< re-configure output image buffer address */
#define VPP_F_ADDR	3
#define VPP_F_IN	1<<2		/**< re-configure input video only */
#define VPP_F_OUT	1<<3		/**< re-configure output video only */
#define VPP_F_OVL	1<<4		/**< re-configure overlay settings only */
#define VPP_F_SCA_MODE	1<<5		/**< re-configure scaler settings: bypass, lpf_linear, cubic, firmware-determined */
#define VPP_F_SCA_TB	1<<6		/**< re-configure scaler coefficients */
#define VPP_F_CSC_TB	1<<7		/**< re-configure csc transformation */
#define VPP_F_OVL_TB	1<<8		/**< re-configure overlay clut */


/* enumeration */

typedef enum
{
	VPP_ERR_NONE = 0,		/**< sucessful */
	VPP_ERR_HW = -1,		/**< hardware error */
	VPP_ERR_PARM = -2,		/**< parameter error */
	VPP_ERR_CFG = -3,		/**< configuration error */
	VPP_ERR_MEM = -4,		/**< memory allocation error */
	VPP_ERR_SUPP = -5		/**< not supported err */
}
vpp_err_e;

typedef enum
{
	VPP_PIC_YUV444P	= 0,		/**< YUV444 planar */
	VPP_PIC_YUV422P	= 1,		/**< YUV422 planar */
	VPP_PIC_YUV420P	= 2,		/**< YUV420 planar */
	VPP_PIC_YUV411P	= 3,		/**< YUV411 planar */
	VPP_PIC_YUV400P	= 4,		/**< YUV400 planar */
	VPP_PIC_YUV444I	= 5,		/**< YUV444 interleave */
	VPP_PIC_YUV422I_YUYV = 6,	/**< YUV422 interleave (YUYV) */
	VPP_PIC_YUV422I_UYVY = 7,	/**< YUV422 interleave (UYVY) */
	VPP_PIC_RGB32	= 8,		/**< RGB 32bpp */
	VPP_PIC_RGB16	= 0xb		/**< RGB 16bpp */
}
vpp_pic_e;


typedef enum
{
	VPP_DISPLAY_FIT	= 0,		/**< fit output to width and height */
	// the following two are not applicable to the case where DSP directly write to dma input buffer address
	VPP_DISPLAY_CROP = 1,		/**< crop the input to maintain the aspect ratio */
	VPP_DISPLAY_PAD	= 2		/**< pad the output to maintain the aspect ratio */
}
vpp_display_e;


typedef enum
{
	VPP_TRIG_LOOP	= 0,		/**< loop mode (continue to process next frame after completing the current frame as long as channel is enabled) */
	VPP_TRIG_ESYNC_B = 1,		/**< polling mode with ext_sync_b */
	VPP_TRIG_ESYNC	= 2,		/**< polling mode with ext_sync */
	VPP_TRIG_POLL	= 3		/**< polling mode (need to manually enable the channel for each frame to be processed) */
}
vpp_trigger_e;


typedef enum
{
	VPP_SCA_BYPASS	= 0,		/**< sca bypass */
	VPP_SCA_LPFL	= 1,		/**< sca lpf followed by linear interpolation (suitable for down sampling) */
	VPP_SCA_CUBIC	= 2,		/**< sca cubic filtering and interpolation (suitable for up sampling) */
	VPP_SCA_UNKNOWN	= 3		/**< unknown mode, determined by firmware */
}
vpp_scaler_e;


typedef enum
{
	VPP_MODE_SCA	= 0,		/**< SCA only */
	VPP_MODE_CSC	= 1, 		/**< CSC only */
	VPP_MODE_SCA_CSC = 2,		/**< chain mode: SCA->CSC */
	VPP_MODE_CSC_SCA = 3		/**< chain mode: CSC->SCA */
}
vpp_mode_e;


typedef enum
{
	VPP_ALPHA_0	= 0,		/**< 0 */
	VPP_ALPHA_125	= 1,		/**< 12.5% */
	VPP_ALPHA_250	= 2,		/**< 25% */
	VPP_ALPHA_375	= 3,		/**< 37.5% */
	VPP_ALPHA_500	= 4,		/**< 50% */
	VPP_ALPHA_625	= 5, 		/**< 62.5% */
	VPP_ALPHA_750	= 6,		/**< 75% */
	VPP_ALPHA_875	= 7,		/**< 87.5% */
	VPP_ALPHA_1000	= 8		/**< 100% */
}
vpp_opacity_e;


/* structure */

typedef struct
{
	uint16_t	x;		/**< x coordinate */
	uint16_t	y;		/**< y coordinate */
}
vpp_offset_t;

typedef struct
{
	uint16_t	r;		/**< Red, Cr */
	uint16_t	g;		/**< Green, Y */
	uint16_t	b;		/**< Blue, Cb */
}
vpp_coffset_t;


typedef struct
{
	uint16_t	w;		/**< width */
	uint16_t	h;		/**< height */
}
vpp_size_t;

#pragma pack(1)
typedef struct
{
	uint8_t	b;			/**< BLUE/Cb value */
	uint8_t	g;			/**< GREEN/Y value */
	uint8_t	r;			/**< RED/Cr value */
	uint8_t	a;			/**< alpha (opacity) */
}
vpp_clut_t;
#pragma pack()


typedef struct
{
	vpp_size_t	size;		/**< video picture size in pixel*/
	uint16_t	stride_y;	/**< page width for Y in bytes, must in multiple of 8-bytes, 0 means same as width */
	uint16_t	stride_cb;	/**< page width for Cb in bytes, must in multiple of 8-bytes, 0 means same as width */
	uint16_t	stride_cr;	/**< page width for Cr in bytes, must in multiple of 8-bytes, 0 means same as width */
	vpp_pic_e	format;		/**< picture format */
	vpp_size_t	pixel;		/**< pixel aspect ratio */
	uint32_t	ybuf[2];	/**< physical address for Y's window start address (ping-pong0~1) */
	uint32_t	cbbuf[2];	/**< physical address for Cb's window start address (ping-pong0~1) */
	uint32_t	crbuf[2];	/**< physical address for Cr's window start address (ping-pong0~1) */
}
vpp_video_t;



typedef struct
{
	uint8_t		en;		/**< 1 enabled, 0 disabled */
	vpp_offset_t	pos;		/**< position of overlay window */
	vpp_size_t	size;		/**< size of overlay window */
	uint32_t	addr;		/**< address of overlay bitmem */
	uint32_t	stride;		/**< page width for overlay bitmem, must be equal for ch0 and ch1 */
	vpp_clut_t	*clut;		/**< color lookup table */
}
vpp_ovl_cfg_t;


typedef struct
{
	vpp_coffset_t	offset[2];	/**< csc in_offset */
	void		*coef;		/**< csc coefficients */
}
vpp_csc_cfg_t;

typedef struct
{
	uint32_t	select;		/**< external sync source 0~7 */
	uint32_t	delay;		/**< external sync delay */
}
vpp_sync_t;


typedef struct
{
	uint32_t	flag;		/**< VPP_F_XXX */
	vpp_trigger_e	trigger;	/**< processing trigger mode */
	vpp_sync_t	sync;
	vpp_display_e	display;	/**< display format: fit, crop, pad*/
	vpp_video_t	input;		/**< input video for a channel */
	vpp_video_t	output;		/**< output video for this channel */
	vpp_ovl_cfg_t	overlay;	/**< overlay setting for this channel */
}
vpp_cfg_t, *vpp_cfg_p;


typedef struct
{
	void		*reg;		/**< base address of registers */	
/* private */
	uint16_t	lmem_sz;	/**< line memory size (RO) */
	uint8_t		no_of_channels;	/**< number of channel supported */
	uint8_t		features;	/**< bit 0: support curbic interpolation, 
					bit1: support programmable CSC; bit 2: support overlay in scaler */
/* user usage */
//	uint8_t		csc_T[2];	/**< */
//	uint8_t		ovl_clut[2];	/**< */
//	uint8_t		sca_cubic4[2];	/**< sca horizontal/vertical 4-tap cubic coefficients for two channels */
//	uint8_t 	sca_lpf7[2];	/**< sca horizontal 7-tap lpf coefficients for two channels */
	void		*private[2];
	uint8_t		start_once;
}
vpp_t, *vpp_p;


vpp_err_e vpp_init(vpp_p t);
/**<
initialize vpp
*/

vpp_err_e vpp_start(vpp_p t, uint8_t channel);
/**<
start vpp processing, call after vpp_cfg, vpp_lpf or vpp_cubic 
*/

vpp_err_e vpp_stop(vpp_p t, uint8_t channel);
/**<
stop/suspend vpp processing; only applicable to situation that vpp trigger mode is not polling
*/

vpp_err_e vpp_cfg(vpp_p t, vpp_cfg_p cfg, uint8_t channel);
/**<
configure vpp channel, except settings for sca lpf and cubic
*/

uint32_t vpp_status(vpp_p t);
/**<
return vpp status register value
*/

vpp_err_e vpp_exit(vpp_p t);
/**<
module exit, free internal memory
*/

vpp_err_e vpp_lpf_T_cfg(vpp_p t, const int *y_lpf, const int *c_lpf, uint8_t channel);
/**<
configure horizontal Y/C lpf coefficients
*/

vpp_err_e vpp_cubic_T_cfg(vpp_p t, const int *hor_cubic, const int *ver_cubic);
/**<
configure horizontal/vertical cubic cofficients
*/

vpp_err_e vpp_overlay_T_cfg(vpp_p t, vpp_clut_t *clut, uint8_t channel);
/**<
configure overlay color lookup table
*/

vpp_err_e vpp_csc_T_cfg(vpp_p t, const int *coef, const int *offset, uint8_t channel);
/**<
configure csc coefficent matrix and in/out offset
@param[in]	t		vpp context
@param[in]	coef		transformation matrix, {{c00,c01,c02}, {c10, c11, c12}, {c20, c21, c22}}
@param[in]	offset		offset vectors, {{x,y,z}, {X, Y, Z}}; x,y,z, IN offset; X,Y,Z, OUT offset
@param[in]	channel		CSC channel
*/

#ifdef VPP_PIU_ENABLE
void vpp_dma_shortcut(vpp_p t, uint32_t y, uint32_t cb, uint32_t cr, uint8_t channel);
#endif


#endif
