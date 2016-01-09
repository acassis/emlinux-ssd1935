/* Use VPP & VPP_L driver */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
//#include <sys/mman.h>

#if 1
#define dbg printf
#else
#define dbg(x...)
#endif	
		

/* PROJECT SETTINGS */
#ifdef ARTEMIS
#warning "ARTEMIS"
#define DISP_VPP_BASE	0xd0008000
#define DISP_TVO_BASE	0xd0009000
#define DISP_TVE_BASE	0xd000a000
#define DISPLAY_TV
#define NSHAOWEI_INT_TVE	/* use external tve */
//#define USE_DYNAMIC_MEM	/* do not use hard-coded memory region, only applicable to tv display, not applicable to lcd display */
#define NEED_RESP		/* every command from ceva will return a dummy resp */
#ifndef USE_DYNAMIC_MEM
//#define DISP_TVO_BUF		0xe2a00000
#endif

#else	/* APHRODITE */
#warning "APHRODITE"
#define DISP_VPP_BASE	0x08008000
#define DISP_TVO_BASE	0x08009000
#define DISP_TVE_BASE	0x0800a000
#define NEED_RESP		/* every command from ceva will return a dummy resp */

//#define DISPLAY_TV
//#define TV_UNDERRUN_BUG

//#ifdef DISPLAY_TV
//#define TV_SCRIPT_ENABLE
//#endif
#endif

#include"vpp.h"

#ifdef DISPLAY_TV

#ifndef TV_SCRIPT_ENABLE
#include"tvo.h"
#ifndef NSHAOWEI_INT_TVE
#include"tve.h"
#endif
#endif

#else	/* DISPLAY_TV */

#ifdef USE_DYNAMIC_MEM
#undef USE_DYNAMIC_MEM
#endif

#endif	/* DISPLAY_TV */

#ifdef USE_DYNAMIC_MEM
#include "mem_alloc.h"
#else
#include "mapm.h"
#endif

#include "piu_types.h"
#include "piulib.h"
#include "piu_msg.h"
#include "disp.h"

/* piu cmd_id: always 2 bytes*/

/* cmd_id lower byte */
#define DISP_INIT	1
#define DISP_SET	2
#define DISP_GET	3
#define DISP_START	4
#define DISP_CFG	5
#define DISP_CLOSE	6
#if 0
#define DISP_STOP	7
#define DISP_OVERLAY	8		/* feature not implemented */
#endif

/* cmd_id higher byte */
#define DISP_IMAGE_PROPERTY	0		/* DISP_SET: input image size and stride; DISP_GET: output image size */
//#define DISP_IMAGE_STRIDE	1 << 8		/* DISP_SET: stride for Y, Cb/Cr */
#define DISP_BACKGROUND		2 << 8		/* SISP_SET: background color */
//#define DISP_BUF_ADDR		3 << 8		/* DISP_SET: dma ping-pong buffer address */
#define DISP_IMAGE_FORMAT	4 << 8		/* DISP_SET: image format (YUV444, RGB32, etc)*/
#define DISP_IMAGE_START	5 << 8		/* DISP_GET: image start offset */

/* piu_disp_cfg flag */

#define DISP_CFG_ALL		7 << 8		/* configure all registers except channel enable bit */
#define DISP_CFG_BUF		8 << 8		/* configure dma address */
#define DISP_CFG_WIDTH		9 << 8		/* configure input image width */
#define DISP_CFG_HEIGHT		10 << 8		/* configure input image height */
#define DISP_CFG_SCREEN_SZ	11 << 8		/* configure vpp output image size, implemented but not used yet */

#define DISP_U8_TO_U16(p)	*((uint16_t *)(p))
#define DISP_U8_TO_U32(p)	*((uint32_t *)(p))

/* display device settings */

#define DISP_TV_PAL	0
#define DISP_TV_NTSC	1
#define DISP_LCD	2

/* constants for display */
#define DISP_VPP_CH	0

#ifdef DISPLAY_TV
#define DISP_TVO_MODE	DISP_TV_NTSC
#else	/* DISPLAY_TV */
#define DISP_TVO_MODE	DISP_LCD
#endif


volatile int disp_done = 0;		/* 1, display finish */

static const uint16_t tvo_pattern_size[3][2] = {{720, 576},	/* tv_pal */
						{720, 480}, 	/* tv_ntsc */
						{800, 480}};	/* lcd full screen */


static piu_msg_t disp_tx;
static piu_msg_t disp_rx;

static uint16_t fbp[3] = {0, 0, 0};

#ifdef BUFFER_INIT_CLEAR
// only used if you want to clear the screen at the beginning
static void *g_y = NULL;	/* virtual address */
static void *g_u = NULL;
static void *g_v = NULL;
static uint32_t	g_yuvsz[3] = {0, 0, 0};
#endif


static uint32_t vpp_output[3];	/* physical address of vpp out buffer */

static vpp_t	vpp;
static vpp_cfg_t	cfg;

// the complete JPG image size
uint16_t g_jpg_width = 0;
uint16_t g_jpg_height = 0;

uint16_t g_jpg_slices_height = 0;
uint32_t g_jpg_slices_index = 0;

uint16_t g_disp_mode = DISP_VIDEO_MODE;
uint16_t g_partial_disp_mode = 0;
uint16_t g_rgb_elem_size = 0;

/* width/height: vpp output image*/
static int piu_vpp_init(void)
{

	// init global variables
	g_partial_disp_mode = 0;
	g_jpg_slices_height = 0;
    g_jpg_slices_index = 0;

	/* Configure VPP */
	vpp.reg = (void *)DISP_VPP_BASE;
	
	cfg.input.pixel.w = 1;
	cfg.input.pixel.h = 1;
	cfg.input.format = VPP_PIC_YUV420P;
	cfg.trigger = VPP_TRIG_POLL;		/* polling mode, manual re-start */
	cfg.display = VPP_DISPLAY_FIT;	
	cfg.flag = VPP_F_IN | VPP_F_OUT;	// | VPP_F_ADDR;

	cfg.output.size.w = fbp[0];
	cfg.output.size.h = fbp[1];

	if ((cfg.output.format== VPP_PIC_RGB16) 
		|| (cfg.output.format == VPP_PIC_YUV422I_YUYV)
		|| (cfg.output.format == VPP_PIC_YUV422I_UYVY))
	{
		cfg.output.stride_y = fbp[2] * 2; //TVO_INP_WIDTH;
	}
	else if (cfg.output.format == VPP_PIC_RGB32)
	{
		cfg.output.stride_y = fbp[2] * 4; //TVO_INP_WIDTH;
		
	}
	else 
	{
		cfg.output.stride_y = fbp[2]; //TVO_INP_WIDTH;
		cfg.output.stride_cb = fbp[2]; //TVO_INP_WIDTH;
		cfg.output.stride_cr = fbp[2]; //TVO_INP_WIDTH;
	}
	cfg.output.pixel.w = 1;
	cfg.output.pixel.h = 1;

	cfg.output.ybuf[0] =  vpp_output[0];
	cfg.output.cbbuf[0] =  vpp_output[1];
	cfg.output.crbuf[0] =  vpp_output[2];

	// black out the display for jpeg display
	if(g_disp_mode == DISP_IMAGE_MODE)
	{
		void *addr = 0;	
		int size = fbp[0]*fbp[1]*g_rgb_elem_size;
		int index = 0;
#if 0		
		// black out the last 10 line
		if(fbp[1] > 10)
		{
			index = (fbp[1] - 10)* fbp[0]*g_rgb_elem_size;
			size = fbp[0]*10*g_rgb_elem_size;
		}
#endif
		dbg("DISP: disp buffer mapm %x, size %d\n",vpp_output[0]+index, size);
		addr = mapm(vpp_output[0]+index, size);
		
	    if(addr != 0)
		{	
			// clear display 
			/* clear vpp output buffer (framebuffer) to black */	
			memset((uint8_t *)addr, 0, size);
			unmapm(addr, size);
		}
		else
			printf("failed to mapm %x, size %d\n",vpp_output[0], size); 
	}
	return vpp_init(&vpp);
}

static int piu_vpp_cfg(uint16_t flag, void *ctx)
{
	int ret = 0;
	switch (flag)
	{
		case DISP_CFG_ALL:
		{ /* vpp_t must be ready */
        		cfg.input.ybuf[0] = 1;
        		cfg.input.cbbuf[0] = 1;
        		cfg.input.crbuf[0] = 1;
				if((g_disp_mode == DISP_IMAGE_MODE) &&(g_partial_disp_mode == 1))
				{
					//update VPP output height
					cfg.output.size.h = g_jpg_slices_height;	
				}
			cfg.flag = VPP_F_IN | VPP_F_OUT | VPP_F_ADDR;
dbg("DISP: rx DISP_CFG_ALL\n");
			break;
		}
#if 0
		case DISP_CFG_BUF:
		{
			uint32_t *addr = (uint32_t *)ctx;
			
        		vpp.input[DISP_VPP_CH].ybuf[0] = addr[0];
        		vpp.input[DISP_VPP_CH].cbbuf[0] = addr[1];
        		vpp.input[DISP_VPP_CH].crbuf[0] = addr[2];
			vpp.flag[DISP_VPP_CH] = VPP_F_INADDR | VPP_F_CH_EN;
dbg("???rx DISP_CFG_BUF: (%x, %x, %x)\n", addr[0], addr[1], addr[2]);
			break;
		}
#endif
		case DISP_CFG_WIDTH:
		{
			cfg.input.size.w = *((uint16_t *)ctx);
			cfg.flag = VPP_F_IN;
dbg("DISP: rx DISP_CFG_WIDTH: %d\n", cfg.input.size.w);
			break;
		}
		case DISP_CFG_HEIGHT:
		{
			cfg.input.size.h = *((uint16_t *)ctx);
			cfg.flag = VPP_F_IN;
dbg("DISP: rx DISP_CFG_HEIGHT: %d\n", cfg.input.size.h);
			break;
		}
		case DISP_CFG_SCREEN_SZ:
		{
			uint16_t *size = (uint16_t *)ctx;
			cfg.output.size.w = size[0];	
			cfg.output.size.h = size[1];		
			cfg.flag = VPP_F_OUT;
dbg("DISP: rx DISP_CFG_SCREEN_SZ: (%d, %d)\n", size[0], size[1]);
			break;	
		}
		default:
dbg("rx Unknown DISP_CFG\n");
			return -6;
	}
dbg("DISP: flag=0x%x, IN w=%d h=%d stride=[%d %d %d]\n\tOUT w=%d, h=%d, stride=[%d %d %d]\n", cfg.flag, cfg.input.size.w, cfg.input.size.h, cfg.input.stride_y, cfg.input.stride_cb, cfg.input.stride_cr, cfg.output.size.w, cfg.output.size.h, cfg.output.stride_y, cfg.output.stride_cb, cfg.output.stride_cr);
	ret = vpp_cfg(&vpp, &cfg, DISP_VPP_CH);
	if (ret)
	{
		printf("vpp_cfg failed\n");
		goto l_vpp_cfg_exit;
	}
l_vpp_cfg_exit:
	return ret;
}


/* display piu isr handler */
void disp_handler(void *ctx, piu_msg_p msg)
{
	uint16_t cmd_id;

	memcpy(&disp_rx, msg, sizeof(piu_msg_t));
		
	cmd_id = DISP_U8_TO_U16(disp_rx.p);
	dbg("V %x\n", cmd_id);
	
	if ((cmd_id & 0xFF) == DISP_GET)
	{
		switch (cmd_id&0xFF00)
		{
			case DISP_IMAGE_PROPERTY:
			{
				disp_tx.len = 6;
				DISP_U8_TO_U16(disp_tx.p) = cmd_id;
				DISP_U8_TO_U16(disp_tx.p+2) = cfg.output.size.w;
				DISP_U8_TO_U16(disp_tx.p+4) = cfg.output.size.h;
dbg("DISP: rx DISP_GET_IMAGE_PROPERTY ->tx [%d,%d] \n", cfg.output.size.w, cfg.output.size.h);
				break;
			}
			case DISP_IMAGE_START:
			{
				disp_tx.len = 6;
				DISP_U8_TO_U16(disp_tx.p) = cmd_id;
				DISP_U8_TO_U16(disp_tx.p+2) = 0;	//offset[0];
				DISP_U8_TO_U16(disp_tx.p+4) = 0;	//offset[1];
dbg("DISP: rx DISP_GET_IMAGE_START -> tx [%d,%d] \n", 0,0);             //offset[0], offset[1]);
				break;
			}
			default:
			{
				printf("rx Unkwown DISP_GET\n");
				return;
			}
			piu_tx(PIU_DISP_QID, &disp_tx);
		}
	}
	else if (cmd_id == (DISP_CFG | DISP_CFG_BUF))
	{
		// DO NOT send back response because ceva can't receive it due to nested interrupt
		uint32_t *addr = (uint32_t *)(disp_rx.p+4);
		
        	cfg.input.ybuf[0] = addr[0];
        	cfg.input.cbbuf[0] = addr[1];
        	cfg.input.crbuf[0] = addr[2];
		// check if there is partial display mode or not
		if((g_disp_mode == DISP_IMAGE_MODE) &&(g_partial_disp_mode == 1))
		{
			//update VPP output buffer pointor           
			cfg.output.ybuf[0] = vpp_output[0] + g_jpg_slices_index;
			cfg.output.cbbuf[0] = 0;
			cfg.output.crbuf[0] = 0;
			g_jpg_slices_index += g_jpg_slices_height*g_rgb_elem_size*cfg.output.size.w;
			dbg("DISP: output ybuf: %x, buf index: %x\n", cfg.output.ybuf[0], g_jpg_slices_index);
		}

		cfg.flag = VPP_F_ADDR;
dbg("DISP: rx DISP_CFG_BUF: (%x, %x, %x)\n", addr[0], addr[1], addr[2]);
		if (vpp_cfg(&vpp, &cfg, DISP_VPP_CH))
		{
			printf("DISP: VPP_CFG failed\n");
			return;
		}
dbg("DISP: DISP_START\n");
		if (vpp_start(&vpp, DISP_VPP_CH))
		{
			printf("DISP: VPP_START failed\n");
			return;
		}
dbg("DISP: DISP_START done\n");
	}
	else
	{
	 	if ((cmd_id & 0xFF) == DISP_CFG)
		{
			if (piu_vpp_cfg(cmd_id&0xFF00, (char *)disp_rx.p + 4))
			{
				printf("DISP: VPP_CFG failed\n");
			}
		}
		else if ((cmd_id & 0xFF) == DISP_SET)
		{
			switch (cmd_id&0xFF00)
			{
				case DISP_IMAGE_PROPERTY:
				{
					uint16_t *p = (uint16_t *)(disp_rx.p+2);
					cfg.input.size.w = p[0];
					cfg.input.size.h = p[1];
					cfg.input.stride_y = p[2];
					cfg.input.stride_cb = p[3];
					cfg.input.stride_cr = p[3];
					
					// check for partial image display mode
					if((g_disp_mode == DISP_IMAGE_MODE) && (cfg.input.size.h < g_jpg_height))
					{
						g_partial_disp_mode = 1;
						g_jpg_slices_height = (fbp[1]*cfg.input.size.h)/g_jpg_height;
                        dbg("DISP: partial input height = %d, output height = %d, orig image height = %d\n",cfg.input.size.h,g_jpg_slices_height,g_jpg_height);						
					}	

					//clean 080104

					if (disp_rx.len >= 12)  //added 2 bytes for yuv format
					{	
						switch (p[4])
						{
							case 0: cfg.input.format = VPP_PIC_YUV444P;break;
							case 1: cfg.input.format = VPP_PIC_YUV422P;break;
							case 2: cfg.input.format = VPP_PIC_YUV420P;break;
							case 3: cfg.input.format = VPP_PIC_YUV411P;break;
							case 4: cfg.input.format = VPP_PIC_YUV400P;break;
							default:printf("error yuv format\n");
						}
						dbg("DISP: output format = %d\n", cfg.input.format);
					}
					if (disp_rx.len == 16)  
					{						
						cfg.input.pixel.w = p[5];
						cfg.input.pixel.h = p[6];
						if (p[5] == 0 || p[6] == 0)
						{
							cfg.input.pixel.w = 1;
							cfg.input.pixel.h = 1;
						}
					}

					//endclean

					cfg.flag |= VPP_F_IN;
dbg("DISP: rx DISP_SET_IMAGE_PROPERTY: size=[%d,%d], stride=[%d %d]\n", p[0], p[1], p[2], p[3]);
#ifdef TV_UNDERRUN_BUG
{
	void *stat = mapm(0x8009008, 36);
	uint32_t val= 0;
	if (stat == (void *)-1)
	{
                printf("mapm fail\n");
		return;
	}
	val = *((uint32_t *)stat + 8);
	printf("image un=%x\n", val);
	if (val == 3)
 	{	
		int i= 500;
		
		*(uint32_t *)stat = 3;
		while (i--);
		*((uint32_t *)stat + 1) = 0x38004F;
		*((uint32_t *)stat + 5) = 0x93e00000;
		*((uint32_t *)stat + 6) = 0x93e54600;
		*((uint32_t *)stat + 7) = 0x93e7e900;
		*(uint32_t *)stat = 5;
		printf("image un=%x\n", *((uint32_t *)stat + 8) );
	}
	unmapm(stat, 36);
}
#endif
					break;
				}
				default:
				{
					printf("DISP: rx Unknown DISP_SET\n");
					return;
				}
			}
		}	
		else
		{
			switch (cmd_id)
			{
				case DISP_INIT:
				{
dbg("DISP: rx DISP_INIT\n");
#if 0
					if (disp_init())
					{
						printf("DISP_INIT failed\n");
						return;
					}
#endif
					if (piu_vpp_init())
					{
						printf("DISP: VPP_INIT failed\n");
						return;
					}
dbg("DISP: VPP OPEN\n");

					disp_tx.len = 6;
					DISP_U8_TO_U16(disp_tx.p) = cmd_id;
					DISP_U8_TO_U16(disp_tx.p+2) = cfg.output.size.w;
					DISP_U8_TO_U16(disp_tx.p+4) = cfg.output.size.h;
					piu_tx(PIU_DISP_QID, &disp_tx);
					return;

				}
				case DISP_START:
				{
dbg("DISP: rx DISP_START\n");
					if (vpp_start(&vpp, DISP_VPP_CH))
					{
						printf("DISP: VPP_START failed\n");
						return;
					}
					break;
				}			
				case DISP_CLOSE:
				{
					g_disp_mode = DISP_VIDEO_MODE;
					g_partial_disp_mode = 0;
					g_jpg_slices_index = 0;
					//vpp_exit(&vpp);
					disp_done = 1;
#ifdef TV_UNDERRUN_BUG
{
	void *stat = mapm(0x8009028, 4);
	uint32_t val= 0;
	if (stat == (void *)-1)
	{
                printf("mapm fail\n");
		return;
	}
	val = *(uint32_t *)stat;
	printf("close un=%x\n", val);
	if (val == 3)
	{
		*(uint32_t *)stat = 3;
	}
	unmapm(stat, 4);
}
#endif
printf("DISP: VPP CLOSE\n");
#if 0
					disp_exit();
printf("???disp_exit() done???\n");
#endif
					break;
				}
				default:
				{
					printf("Unkwown DISP command 0x%x\n", cmd_id);
					return;
				}
			}
		}
#ifdef NEED_RESP
printf("\n%x\n", cmd_id);
		disp_tx.len = 2;
		DISP_U8_TO_U16(disp_tx.p) = cmd_id;
		piu_tx(PIU_DISP_QID, &disp_tx);
#endif
	}
}


int disp_resize(uint16_t w, uint16_t h)
{
	int ret = 0;
	fbp[0] = w;
	fbp[1] = h;
	cfg.output.size.w = fbp[0];
	cfg.output.size.h = fbp[1];
	cfg.flag = VPP_F_IN;
	ret = vpp_cfg(&vpp, &cfg, DISP_VPP_CH);
	if (ret)
	{
		return ret;
	}
	return vpp_start(&vpp, DISP_VPP_CH);
}


int disp_init(uint16_t w, uint16_t h, uint16_t stride, uint32_t vpp_out_buf[3], int fmt)
{
	uint32_t sz;
	
	if (!stride)
	{
		printf("Framebuffer stride must be larger than 0\n");	
		return -1;
	}

	fbp[0] = w;		/* width of region of display (ROD */
	fbp[1] = h;		/* height of ROD */
	fbp[2] = stride;	/* stride of framebuffer */
	vpp_output[0] = vpp_out_buf[0];/* Y starting address */
	vpp_output[1] = vpp_out_buf[1];/* U starting address */
	vpp_output[2] = vpp_out_buf[2];/* V starting address */
	sz = fbp[1]*fbp[2];
	disp_done = 0;

	g_rgb_elem_size = fmt/8;

	/* tvo, tve init & cfg*/
	memset(&disp_tx, 0, sizeof(piu_msg_t));
	memset(&disp_rx, 0, sizeof(piu_msg_t));
	memset(&vpp, 0, sizeof(vpp_t));
	memset(&cfg, 0, sizeof(vpp_cfg_t));

  	/* In this task, disp_tx always to be response to ceva, and disp_rx always contains cmd from ceva */
	disp_tx.type = PIU_REP;
	disp_rx.type = PIU_CMD;
#if DISP_TVO_MODE == DISP_LCD
	switch (fmt)
	{
		case 8:
			cfg.output.format =  VPP_PIC_YUV420P;
#ifdef BUFFER_INIT_CLEAR
			g_yuvsz[0] = sz;
			g_yuvsz[1] = sz/2;
			g_yuvsz[2] = sz/2;
#endif
			break;
		case 16:
			cfg.output.format =  VPP_PIC_RGB16;
#ifdef BUFFER_INIT_CLEAR
			g_yuvsz[0] = 2*sz;
			g_yuvsz[1] = 0;
			g_yuvsz[2] = 0;
#endif
			break;
		case 32:
			cfg.output.format =  VPP_PIC_RGB32;
#ifdef BUFFER_INIT_CLEAR
			g_yuvsz[0] = 4*sz;
			g_yuvsz[1] = 0;
			g_yuvsz[2] = 0;
#endif
			break;
		default:
			printf("Output color format error, choose only 8, 16, 32\n");	
			return -2;
	}
#else
	cfg.output.format =  VPP_PIC_YUV422P;
#ifdef BUFFER_INIT_CLEAR
	g_yuvsz[0] = sz;
	g_yuvsz[1] = sz/2;
	g_yuvsz[2] = sz;
#endif
#endif

#ifdef BUFFER_INIT_CLEAR
	g_y = mapm(vpp_output[0], g_yuvsz[0]);
	if (g_yuvsz[1])
	{
		g_u = mapm(vpp_output[1], g_yuvsz[1]);
	}
	if (g_yuvsz[2])
	{
		g_v = mapm(vpp_output[2], g_yuvsz[2]);
	}
	if ((g_y == (void *)-1) || (g_u == (void *)-1) || (g_y == (void *)-1))
	{
                printf("mapm fail\n");
		ret = -3;
		goto l_init_exit;
	}
	// clear display 
	/* clear vpp output buffer (framebuffer) to black */	
	{
		int i=0;
		for (i=0; i<fbp[1]; i++)
		{
			memset((uint8_t *)(g_y + i*w), 0, w);
		}
	}
dbg("DISP: black background\n");
#endif

printf("lcd init done\n");
	return 0;

#ifdef BUFFER_INIT_CLEAR
l_init_exit:
	unmapm(g_y, g_yuvsz[0]);
	if (g_yuvsz[1])
	{
		unmapm(g_u, g_yuvsz[1]);
	}
	if (g_yuvsz[2])
	{
		unmapm(g_v, g_yuvsz[2]);
	}

printf("DISP: INIT failed\n");
	return ret;
#endif
}


void disp_exit()
{
printf("DISP: EXIT\n");
#ifdef BUFFER_INIT_CLEAR
	unmapm(g_y, g_yuvsz[0]);
	if (g_yuvsz[1])
	{
		unmapm(g_u, g_yuvsz[1]);
	}
	if (g_yuvsz[2])
	{
		unmapm(g_v, g_yuvsz[2]);
	}
#endif
}
