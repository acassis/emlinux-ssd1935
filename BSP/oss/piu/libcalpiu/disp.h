#ifndef _SHAOWEI_PIU_DISP_H
#define _SHAOWEI_PIU_DISP_H

#include "piu_types.h"


int disp_init(uint16_t w, uint16_t h, uint16_t stride, uint32_t vpp_out_buf[3], int cbits);
/**<
initialize display device parameters
@param[in]	w	display width (in pixel) along x coordinate
@param[in]	h	display height (in pixel) along y coordinate
@param[in]	stride	display buffer stride
@param[in]	vpp_out_buf[3]	vpp output buffer physical address: Y(RGB), U, V
@param[in]	cbits	display RGB bits, 8, 16 (default) or 32
return		0, success, others failed
*/

int disp_resize(uint16_t w, uint16_t h);
/**<
resize the display size; ONLY applicable when vpp is not started
@param[in]	w	display width (in pixel)
@param[in]	h	display hight (in pixel)
return		0, success, others failed
*/

int disp_adjust_color(int coef[], int offset[]);
/**<
adjust display color by changing csc parameters
@param[in]	coef	Color convertion matrix (9 elements)
@param[in]	offset	Color convertion offst (6 elements)
return		0, success, others failed
*/


void disp_exit(void);
/**<
close display unit; ONLY used when you enable initial screen clear
*/

void disp_handler(void *ctx, piu_msg_p msg);
/**<
piu callback for video display
@param[in]	ctx	context
@param[in]	msg	PIU message
*/

extern void disp_close(void);


#define DISP_VIDEO_MODE	0
#define DISP_IMAGE_MODE	1

extern uint16_t g_jpg_width;
extern uint16_t g_jpg_height;
extern uint16_t g_disp_mode;

#endif
