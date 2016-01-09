#ifndef CAL_INTERFACE_BUFFDCPT_H
#define CAL_INTERFACE_BUFFDCPT_H


/**
* @struct cal_mcu_buff_dcpt
* @brief buffer descriptor structure definition
*
* Structure used by sample application on MCU for data buffers
*/
typedef struct
{
	/** @var <media codec type, cross check in DSP CAL> */
	S32  media_codec;
	/** @var <Buffer physical address> */
	U32  buff_addr;
	/** @var <data buffer size> */
	U32  buff_size;
	/** @var <buffer alignment> */
	U8   buff_align;
	/** @var <physical address of next buff descriptor > */
	U32  buff_next_addr;
	/** @var <additional info parameter> */
	S32  info;
}cal_mcu_buff_dcpt;


#endif /*CAL_INTERFACE_BUFFDCPT_H*/

/* nothing beyond this */
