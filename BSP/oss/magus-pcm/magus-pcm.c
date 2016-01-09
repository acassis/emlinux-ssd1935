/*
 * linux/sound/soc/magus/magus-pcm.c -- ALSA PCM interface for the Magus chip
 *
 * Author:	JF Liu
 * Created:	Nov 30, 2007
 * Copyright:	(C) 2007 Solomon Systech
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>

#include <asm/uaccess.h>
#include <asm/io.h>

#include <sound/driver.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>

#include "magus-pcm.h"

/* 
   CAL audio playback enable flag. When enabled - uses CAL messaging structures 
   , when disabled - uses player related structures for audio play back 
*/
#define CALARM_APB_ENABLE

#ifdef CALARM_APB_ENABLE
/* CAL integration */
#include "import/cal_interface_types.h"
#include "import/cal_interface_srv.h"
#endif /*CALARM_APB_ENABLE*/


// external function prototypes
typedef void (*alsa_cb)(void*);
extern void alsa_cb_register(alsa_cb cb);
extern int piu_tx(uint32_t msg, piu_msg_p body);
extern void alsa_piu_enable(int flag);

/* enable this flag for a timer-interrupt based PCM device test */
//#define DUMMY_TIMER_TEST

/* the flag for debugging, it will print out all the dbg messages */
#define MAGUS_PCM_DEBUG 0

#if MAGUS_PCM_DEBUG
#define dbg(format, arg...) printk(KERN_ERR"magus-pcm.c %s()"format,__FUNCTION__, ## arg)
#else
#define dbg(format, arg...)
#endif

#define PCM_ERR(format, arg...) \
	printk(KERN_ERR"magus-pcm.c %s() "format,__FUNCTION__, ## arg)

/* the base physical address of DSP Tx/Rx register map */
static unsigned int magus_dsp_tdm_base_addr;

/* the static pointer to store the runtime data */
static struct magus_runtime_data *g_prtd = NULL; 



static const struct snd_pcm_hardware magus_pcm_hardware = {
	.info			= SNDRV_PCM_INFO_MMAP |
				  SNDRV_PCM_INFO_MMAP_VALID |
				  SNDRV_PCM_INFO_INTERLEAVED |
				  SNDRV_PCM_INFO_PAUSE |
				  SNDRV_PCM_INFO_RESUME,
	.formats		= SNDRV_PCM_FMTBIT_S16_LE |
					SNDRV_PCM_FMTBIT_S24_LE |
					SNDRV_PCM_FMTBIT_S32_LE,
	.period_bytes_min	= 128,
	.period_bytes_max	= 8192,
	.periods_min		= 2,
	.periods_max		= 1024,
	.buffer_bytes_max	= 256 * 1024,
	.fifo_size		= 32,
};

struct magus_runtime_data {
	spinlock_t lock;
#ifdef DUMMY_TIMER_TEST	
	struct timer_list timer;
	unsigned int pcm_bps;		/* bytes per second */
	unsigned int pcm_jiffie;	/* bytes per one jiffie */
#endif
	unsigned int pcm_size;
	unsigned int pcm_count;
	unsigned int pcm_irq_pos;	/* IRQ position */
	unsigned int pcm_buf_pos;	/* position in buffer */
	struct snd_pcm_substream *substream;
};

/* local function prototype */
static inline int magus_pcm_tdm_config(struct snd_pcm_substream *substream, uint8_t flag);
static inline int magus_pcm_tdm_sync(struct snd_pcm_substream *substream);

#ifdef DUMMY_TIMER_TEST	
static inline void magus_pcm_timer_start(struct magus_runtime_data *prtd)
{
	prtd->timer.expires = 1 + jiffies;
	add_timer(&prtd->timer);
}

static inline void magus_pcm_timer_stop(struct magus_runtime_data *prtd)
{
	del_timer(&prtd->timer);
}

static void magus_pcm_timer_function(unsigned long data)
{
	struct magus_runtime_data *prtd = (struct magus_runtime_data *)data;
	unsigned long flags;
	dbg("enter magus_pcm_timer_function func\n");
	spin_lock_irqsave(&prtd->lock, flags);
	prtd->timer.expires = 1 + jiffies;
	add_timer(&prtd->timer);
	prtd->pcm_irq_pos += prtd->pcm_jiffie;
	prtd->pcm_buf_pos += prtd->pcm_jiffie;
	prtd->pcm_buf_pos %= prtd->pcm_size;
	if (prtd->pcm_irq_pos >= prtd->pcm_count) {
		prtd->pcm_irq_pos %= prtd->pcm_count;
		spin_unlock_irqrestore(&prtd->lock, flags);
		snd_pcm_period_elapsed(prtd->substream);
		dbg("sent snd_pcm_period_elapsed\n");
	} else
		spin_unlock_irqrestore(&prtd->lock, flags);
}

#endif
/* the callback function serves as the PCM interface interrupt service routine */
int magus_pcm_apb_state = 0;
#ifdef CALARM_APB_ENABLE
///#define DEBUG_PCM_COUNT
#ifdef DEBUG_PCM_COUNT
static volatile unsigned int count = 0;
static volatile unsigned int SampleOfCnt = 0;
static volatile int PValue = 0;
static volatile unsigned int PreDiff = 0;
#endif
/* the callback function serves as the PCM interface interrupt service routine */
void magus_pcm_callback(void* data)
{
	//unsigned long flags;
	struct magus_runtime_data *prtd = g_prtd;
	uint32_t position;
	uint32_t                        size;
	cal_dspmsg_apb_currstate        *ps_dspapb_currstat;
	cal_dspmsg_apb_start            *ps_dspapb_start;
	
#ifdef DEBUG_PCM_COUNT
	snd_pcm_uframes_t appl_ptr, appl_ofs;
	struct snd_pcm_runtime *runtime;
	int diff = 0;
#endif
	
	/* initialize */
	ps_dspapb_start = (cal_dspmsg_apb_start *)data;
	/**************************************************************************/
	/* check for the status of current status message from dsp, if ok then add*/
	/* the size consumed to the curr position of PCM buffer pointer           */
	/**************************************************************************/
	if(ps_dspapb_start->status != CAL_MSG_MEDIA_COMP_STATUS_OK)
	{
		///*((unsigned int *)(magus_dsp_tdm_base_addr+4)) = 0;
		magus_audio_data_finish_msg_t* dsp_msg = (magus_audio_data_finish_msg_t*)data;
		printk(" ??? dsp_msg->msgid:0x%02x\n", dsp_msg->msgid);
		printk("Error: invalid dsp msg id = %d\n", (int)ps_dspapb_start->status);
#ifdef DEBUG_PCM_COUNT
		SampleOfCnt = 0;
		count = 0;
#endif
		magus_pcm_apb_state = 0;
		return;
	}
	/* if current status message from dsp then increment the pcm buffer */
	if(CAL_MSG_APB_CURRSTATE == ps_dspapb_start->msg_id)
	{
		ps_dspapb_currstat = (cal_dspmsg_apb_currstate *)data;
		position = ps_dspapb_currstat->buff_posn;
		size     = ps_dspapb_currstat->buff_size_consumed;
		dbg("DSP audio frame msg rxed with data size %d, position 0x%x\n", size, position);
		/* chen kai 20090806 */
		/* re-caculate the size info based on position */
		size = (position >= prtd->pcm_buf_pos) ? \
			   (position - prtd->pcm_buf_pos) : \
			   (prtd->pcm_size - (prtd->pcm_buf_pos - position));
#ifdef DEBUG_PCM_COUNT
		count++;
		///if (count < 40)
		///	printk("DSP audio frame msg rxed with data size %d, position 0x%x\n", size, position);
		SampleOfCnt += (size/4);
		if (count != 0 && (count%10) == 0)
		{
			///printk("DSP audio frame msg rxed with data size %d, position 0x%x\n", size, position);
			runtime = prtd->substream->runtime;
			appl_ptr = runtime->control->appl_ptr;
			appl_ofs = appl_ptr % runtime->buffer_size;
			diff = appl_ptr - SampleOfCnt;
			
			///hwbuf = runtime->dma_area + frames_to_bytes(runtime, appl_ofs);
			///printk("dma_area : 0x%08x, offset:0x%08x\n", runtime->dma_area, frames_to_bytes(runtime, appl_ofs));
			///printk("app info -> appl_ptr:%d, appl_ofs:%d, SampleOfCnt:%d, diff:%d\n", 
			///		appl_ptr, appl_ofs, SampleOfCnt, (appl_ptr - SampleOfCnt));
			printk("appl_ptr:%d,SampleOfCnt:%d,diff:%d\n", appl_ptr, SampleOfCnt, diff);
#if 0
			if (diff > 20480 || diff < 0)
			{
				// reset PValue and PreDiff
				PValue = 0;
				PreDiff = 0;
			}
			else
			{
				// first time
				if (PreDiff == 0) PValue++;
				// non-first time, compare first
				else if (PreDiff > diff) PValue++;
				else if (PreDiff < diff) PValue--;
				if (diff > 0) PreDiff = diff;
				
				// stop dsp to send pcm data to i2s
				if (PValue >= 5 && diff <= 10240)
				{
					printk("stop dsp sending pcm data to i2s\n");
					*((unsigned int *)(magus_dsp_tdm_base_addr+4)) = 0;
				}
			}
#endif
		}
#endif
		//spin_lock_irqsave(&prtd->lock, flags);
		//printk("buffer addr and cinsumed size, buffer position = %x %d %x %d\n", position,size,prtd->pcm_buf_pos,ps_dspapb_currstat->info);
		
		prtd->pcm_irq_pos += size;
		prtd->pcm_buf_pos += size;
		prtd->pcm_buf_pos %= prtd->pcm_size;
		if (prtd->pcm_irq_pos >= prtd->pcm_count) {
			prtd->pcm_irq_pos %= prtd->pcm_count;
			//spin_unlock_irqrestore(&prtd->lock, flags);
			//dbg("period elapsed \n");
			snd_pcm_period_elapsed(prtd->substream);
			magus_pcm_apb_state = 1;
		} 
		//spin_unlock_irqrestore(&prtd->lock, flags);
	}
	else if(CAL_MSG_APB_INIT == ps_dspapb_start->msg_id)
	{
		magus_pcm_apb_state = 0;
		printk(" dma buffer addr during init = %x \n", (unsigned int)ps_dspapb_start->info);
	}
	else if(CAL_MSG_APB_EXIT == ps_dspapb_start->msg_id)
	{
		/* disable DSP BTDMP by clearing its TDM EN Register */
		*((unsigned int *)(magus_dsp_tdm_base_addr+4)) = 0;
		printk("Disabled BTDMP \n");
		magus_pcm_apb_state = 0;
	}
}
#else /* #ifdef CALARM_APB_ENABLE */
void magus_pcm_callback(void* data)
{
  magus_audio_data_finish_msg_t* dsp_msg = (magus_audio_data_finish_msg_t*)data;
  unsigned long flags;
  struct magus_runtime_data *prtd = g_prtd;
  uint32_t position = dsp_msg->buf_position;
  uint32_t size = dsp_msg->filled_size;
  
  if(dsp_msg->msgid != MAGUS_TDM_FRAME_DONE_MSG_TYPE)
  {
  	printk("Error: invalid dsp msg id = %d\n", dsp_msg->msgid);
  	return;
  }
  
  //dbg("DSP audio frame msg rxed with data size %d, position 0x%x\n", size, position);
  
  //spin_lock_irqsave(&prtd->lock, flags);
  prtd->pcm_irq_pos += size;
  prtd->pcm_buf_pos += size;
  prtd->pcm_buf_pos %= prtd->pcm_size;
  if (prtd->pcm_irq_pos >= prtd->pcm_count) {
	  prtd->pcm_irq_pos %= prtd->pcm_count;
	  //spin_unlock_irqrestore(&prtd->lock, flags);
	  //dbg("period elapsed \n");
	  snd_pcm_period_elapsed(prtd->substream);
  } 
	//spin_unlock_irqrestore(&prtd->lock, flags);

}
#endif /* #ifdef CALARM_APB_ENABLE */

static int magus_pcm_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params)
{
	//struct snd_pcm_runtime *runtime = substream->runtime;
	//struct magus_runtime_data *prtd = runtime->private_data;
  	int ret;
		  
	dbg("enter magus_pcm_hw_params func \n");
	
	ret = snd_pcm_lib_malloc_pages(substream,
				 params_buffer_bytes(params));
	if(ret < 0)
	{
		printk("Error: snd_pcm_lib_malloc_pages failed\n");
		return ret;
	}
	
	dbg("exit magus_pcm_hw_params func OK\n");
	
	return ret;
}

static int magus_pcm_hw_free(struct snd_pcm_substream *substream)
{
	//struct magus_runtime_data *prtd = substream->runtime->private_data;
	int ret;
	
	dbg("enter magus_pcm_hw_free func\n");
	
	ret = snd_pcm_lib_free_pages(substream);
	snd_pcm_set_runtime_buffer(substream, NULL);
	
	dbg("exit magus_pcm_hw_free func %d\n", ret);
	return ret;
}

static int magus_pcm_prepare(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct magus_runtime_data *prtd = runtime->private_data;
	
	dbg("enter magus_pcm_prepare func\n");
	
#ifdef DUMMY_TIMER_TEST
	unsigned int bps;
	bps = runtime->rate * runtime->channels;
	bps *= snd_pcm_format_width(runtime->format);
	bps /= 8;
	if (bps <= 0)
		return -EINVAL;
		
	prtd->pcm_bps = bps;
	prtd->pcm_jiffie = bps / HZ;
#endif	
	prtd->pcm_size = snd_pcm_lib_buffer_bytes(substream);
	prtd->pcm_count = snd_pcm_lib_period_bytes(substream);
	prtd->pcm_irq_pos = 0;
	prtd->pcm_buf_pos = 0;
 	
	// 2009.08.13 by Sky :
	// only do this if "magus dsp TDM EN Register" is on off status
	if (*((unsigned int *)(magus_dsp_tdm_base_addr+4)) == 0)
	{
	// 2009.08.31 : 
	// clear the dma area data in order to prevent
	// from playing previous rest audio data at 
	// startup of current playback
	memset(runtime->dma_area, 0, runtime->dma_bytes);
	//
	/* sync with DSP tdm driver in case for underrun/overrun error recovery */
	alsa_cb_register(magus_pcm_callback);
	/* configure TDM */
////#ifndef CALARM_APB_ENABLE
	magus_pcm_tdm_config(substream, 1);
	/* wait for the TDM to be configured */
	// 2009.08.13 : seems we need more delay ?!
	///mdelay(100);
	mdelay(300);
	printk("reset abp\n");
	}
////#endif /*#ifndef CALARM_APB_ENABLE*/
	dbg("exit magus_pcm_prepare func\n");
	
	return 0;
}
#ifdef CALARM_APB_ENABLE

static int magus_pcm_trigger(struct snd_pcm_substream *substream, int cmd)
{
	struct magus_runtime_data *prtd = substream->runtime->private_data;
	int ret = 0;
	
	// 2009.08.13 by Sky :
	// use appl_ptr for reference of set "magus dsp TDM EN Register"
	snd_pcm_uframes_t appl_ptr;
	struct snd_pcm_runtime *runtime;
	runtime = prtd->substream->runtime;
	appl_ptr = runtime->control->appl_ptr;
	//
	
	dbg("enter magus_pcm_trigger function %d\n",cmd);
	///printk("enter magus_pcm_trigger function %d, %d\n",cmd, appl_ptr);
	spin_lock(&prtd->lock);

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
    dbg("start cmd received\n");
	case SNDRV_PCM_TRIGGER_RESUME:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
#ifdef DUMMY_TIMER_TEST    
	magus_pcm_timer_start(prtd);    
#else
    /* register callback function */
    alsa_cb_register(magus_pcm_callback);
	////magus_pcm_tdm_config(substream, 1);
	////mdelay(100);
	/* enable DSP BTDMP by setting its TDM EN Register */
	// 2009.08.13 by Sky :
	// only set "magus dsp TDM EN Register"
	// when it is off
	if (*((unsigned int *)(magus_dsp_tdm_base_addr+4)) == 0)
	{
		*((unsigned int *)(magus_dsp_tdm_base_addr+4)) = 1;
		printk("appl_ptr:%d\n", appl_ptr);
	/* start TDM interface */
    //magus_pcm_tdm_config(substream, 1);
		printk("apb init:%d\n", cmd);
	}
#endif
	break;

	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
	
    dbg("stop cmd received\n");
	////magus_pcm_tdm_config(substream, 0);
	////mdelay(300);
	/* disable DSP BTDMP by clearing its TDM EN Register */
	// 2009.08.13 by Sky :
	// only do the stop if appl_ptr >= threshold
	// (and magus dsp TDM EN Register is off)
	// to prevent from too many receiving stop cmds
	// at beginning of playback
	if (*((unsigned int *)(magus_dsp_tdm_base_addr+4)) == 1 && 
		appl_ptr >= runtime->rate/4) // we set to 0.25 sec
	{
	   *((unsigned int *)(magus_dsp_tdm_base_addr+4)) = 0;
		printk("apb exit:%d\n", cmd);
		magus_pcm_apb_state = 0;
	}
	//
	
	break;
	
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
#ifdef DUMMY_TIMER_TEST
	  magus_pcm_timer_stop(prtd);
#else 

	//magus_pcm_tdm_config(substream, 0);
	////magus_pcm_tdm_sync(substream);
	////mdelay(100);
	   *((unsigned int *)(magus_dsp_tdm_base_addr+4)) = 0;
	/* stop TDM interface */
    //magus_pcm_tdm_config(substream, 1);        
	printk("apb pause:%d\n", cmd);
#endif    
  	break;
	default:
		dbg("invalid cmd received\n");
		//spin_unlock(&prtd->lock);
		ret = -EINVAL;
		break;
	}
	
	spin_unlock(&prtd->lock);
	dbg("exit magus_pcm_trigger func, %d\n", ret);
	///printk("exit magus_pcm_trigger func, %d\n", ret);
	
	return ret;
}

EXPORT_SYMBOL_GPL(magus_pcm_apb_state);

static inline int magus_pcm_tdm_config(struct snd_pcm_substream *substream, uint8_t flag)
{
	/* local variables */
	piu_msg_t body;
	struct snd_dma_buffer *buf = &substream->dma_buffer;
    cal_mcumsg_apb_start      s_mcucal_apbstart;
    cal_mcumsg_apb_stop       s_mcucal_apbstop;
	struct snd_pcm_runtime *runtime = substream->runtime;
	//void *pv_calmsg;
	dbg("enter magus_pcm_tdm_config func\n");
	
	/* flag-1=init, flag-0=exit apb */
	/* initialize local variables */
	if(1 == flag)
	{
		struct magus_runtime_data *prtd = substream->runtime->private_data;
		
		//printk ("piu send message buffer address = %x \n",(uint32_t) buf->addr);
		/* set the audio playback params and send message to DSP */
		s_mcucal_apbstart.size = sizeof(cal_mcumsg_apb_start); 		
		s_mcucal_apbstart.msg_id = CAL_MSG_APB_INIT; 		
		s_mcucal_apbstart.apb_instance = 0; 
		s_mcucal_apbstart.buffer_addr = (uint32_t) buf->addr; 	
		s_mcucal_apbstart.buff_size = snd_pcm_lib_buffer_bytes(substream);
		printk("buffer_addr(phsical):0x%08x, area:0x%08x, "
			   "buff_size:0x%08x, buffer_size:%d\n", 
				(int)s_mcucal_apbstart.buffer_addr, 
				(uint32_t) buf->area, 
				(int)s_mcucal_apbstart.buff_size, 
				(int)runtime->buffer_size);
		s_mcucal_apbstart.buff_offset = prtd->pcm_buf_pos;
		s_mcucal_apbstart.frm_len_synch = (snd_pcm_lib_period_bytes(substream)/4) * 4;
		s_mcucal_apbstart.curr_timestamp = 0;
		s_mcucal_apbstart.sampling_frquency = substream->runtime->rate;
		s_mcucal_apbstart.packed_flag = CAL_APBMSG_PACKED;
		s_mcucal_apbstart.au_flag = CAL_APBMSG_AULAW_NONE; 		
		s_mcucal_apbstart.out_phase = CAL_APBMSG_PHASE_SINGLE; 	
		s_mcucal_apbstart.element_size = 2; //2 byte size 
		s_mcucal_apbstart.endian = CAL_APBMSG_LITTLE_ENDIAN;
		
		/* set the piu structure elements */
		body.len = sizeof(cal_mcumsg_apb_start);
		memcpy(body.p, &s_mcucal_apbstart, body.len);
	}
	else
	{
		/*set stop params and send message to DSP */
		s_mcucal_apbstop.size = sizeof(cal_mcumsg_apb_stop); 		
		s_mcucal_apbstop.msg_id = CAL_MSG_APB_EXIT; 		
		s_mcucal_apbstop.apb_instance = 0; 

		/* set the piu structure elements */
		body.len = sizeof(cal_mcumsg_apb_stop);
		memcpy(body.p, &s_mcucal_apbstop, body.len);
	}

	
	/*dbg("sent out TDM CONFIG MSG:flag=%d,interface=%d,sample rate=%d,buf_addr=0x%x,buf_size=%d,frm_size=%d\n",
	     flag, msg.interface, msg.sampling_rate, msg.buf_addr,msg.buf_size, msg.frame_size);*/
	/* fill the generic PIU message body */
	body.type = 1;
	
  /* transmit the message to DSP */
#if 1
	/* hack by meeeee */
	//printk ("piu send message flag = %d \n",flag);
	//if(0 != flag)
	if( piu_tx(MCU_DSP_COMMAND_ID_MMCT, &body) != 0)
	{
		printk ("Error: Tx PIU msg failed\n");
		return -1;
	}
#endif	
	dbg("exit magus_pcm_tdm_config func OK\n");
	
	return 0;
}

static inline int magus_pcm_tdm_sync(struct snd_pcm_substream *substream)
{
	/* local variables */
	piu_msg_t body;
	//int ret;
	//struct snd_dma_buffer *buf = &substream->dma_buffer;
	//struct magus_runtime_data *prtd = substream->runtime->private_data;
    cal_mcumsg_apb_pause      s_mcu_apbpause;
	
	dbg("enter magus_pcm_tdm_sync func\n");
	/* set all the params inside the strucutre */
	/* send a message to DSP to synch to this state */
	s_mcu_apbpause.size = sizeof(cal_mcumsg_apb_pause);
	s_mcu_apbpause.msg_id = CAL_MSG_APB_PAUSE;
	s_mcu_apbpause.apb_instance = 0;

	//msg.sampling_rate = substream->runtime->rate;
	
	
	/*dbg("sent out TDM SYNC MSG:interface=%d,sample rate=%d,buf_addr=0x%x,buf_size=%d,frm_size=%d\n",
	     msg.interface, msg.sampling_rate, msg.buf_addr,msg.buf_size, msg.frame_size);*/
	/* fill the generic PIU message body */
	body.type = 1;
	body.len = sizeof(cal_mcumsg_apb_pause);
	memcpy(body.p, &s_mcu_apbpause, body.len);
	/* transmit the message to DSP */

/* debug */
#if 1
	if(piu_tx(MCU_DSP_COMMAND_ID_MMCT, &body) != 0)
	{
		printk ("Error: Tx PIU msg failed\n");
		return -1;
	}
#endif
	dbg("exit magus_pcm_tdm_sync func OK\n");
	
	return 0;
}

#else /*ifdef CALARM_APB_ENABLE*/
static int magus_pcm_trigger(struct snd_pcm_substream *substream, int cmd)
{
	struct magus_runtime_data *prtd = substream->runtime->private_data;
	int ret = 0;
  int i;
	
  dbg("enter magus_pcm_trigger function\n");
  spin_lock(&prtd->lock);

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
    dbg("start cmd received\n");
  case SNDRV_PCM_TRIGGER_RESUME:
  case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
#ifdef DUMMY_TIMER_TEST    
	  magus_pcm_timer_start(prtd);    
#else
    /* register callback function */
    alsa_cb_register(magus_pcm_callback);

    /* enable DSP BTDMP by setting its TDM EN Register */
    *((unsigned int *)(magus_dsp_tdm_base_addr+4)) = 1;
	magus_pcm_abp_state = 1;
    /* start TDM interface */
    //magus_pcm_tdm_config(substream, 1);        
#endif    
		break;

	case SNDRV_PCM_TRIGGER_STOP:
    dbg("stop cmd received\n");
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:

#ifdef DUMMY_TIMER_TEST    
	  magus_pcm_timer_stop(prtd);    
#else 
    /* disable DSP BTDMP by clearing its TDM EN Register */
    *((unsigned int *)(magus_dsp_tdm_base_addr+4)) = 0;
	magus_pcm_abp_state = 0;
    /* stop TDM interface */
    //magus_pcm_tdm_config(substream, 1);        
#endif    
  	break;
	default:
		dbg("invalid cmd received\n");
		//spin_unlock(&prtd->lock);
		ret = -EINVAL;
		break;
	}
  dbg("exit magus_pcm_trigger func, %d\n", ret);
  spin_unlock(&prtd->lock);
	return ret;
}
static inline int magus_pcm_tdm_config(struct snd_pcm_substream *substream, uint8_t flag)
{
	magus_audio_ctl_msg_t msg;
	piu_msg_t body;
	int ret;
	struct snd_dma_buffer *buf = &substream->dma_buffer;
	struct magus_runtime_data *prtd = substream->runtime->private_data;
    
	dbg("enter magus_pcm_tdm_config func\n");
	
	/* send a message to DSP to start BTDMP interface */
	msg.msgid = MAGUS_TDM_CONFIG_MSG_TYPE;
	msg.flag = flag; //start or stop the tdm

	if(substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
		msg.interface = MAGUS_DSP_TDM_TX;
	else
		msg.interface = MAGUS_DSP_TDM_RX;
	
	msg.sampling_rate = substream->runtime->rate;
	msg.buf_addr = (uint32_t) buf->addr;
	msg.buf_size = 	snd_pcm_lib_buffer_bytes(substream);
	
	/* make sure frame size is aligned to 4 */
	msg.frame_size = (snd_pcm_lib_period_bytes(substream)/4) * 4;
	
	dbg("sent out TDM CONFIG MSG:flag=%d,interface=%d,sample rate=%d,buf_addr=0x%x,buf_size=%d,frm_size=%d\n",
	     flag, msg.interface, msg.sampling_rate, msg.buf_addr,msg.buf_size, msg.frame_size);
  /* fill the generic PIU message body */
  body.type = 1;
  body.len = sizeof(msg);
  memcpy(body.p, &msg, body.len);
  /* transmit the message to DSP */
#if 1
 
  if( piu_tx(PIU_ALSA_CTL_QID, &body) != 0)
  {
		printk ("Error: Tx PIU msg failed\n");
		return -1;
	}
#endif	
  dbg("exit magus_pcm_tdm_config func OK\n");
	return 0;
}

static inline int magus_pcm_tdm_sync(struct snd_pcm_substream *substream)
{
	magus_audio_ctl_msg_t msg;
	piu_msg_t body;
	int ret;
	struct snd_dma_buffer *buf = &substream->dma_buffer;
	struct magus_runtime_data *prtd = substream->runtime->private_data;
    
	dbg("enter magus_pcm_tdm_sync func\n");
	
	/* send a message to DSP to start BTDMP interface */
	msg.msgid = MAGUS_TDM_SYNC_MSG_TYPE;

	if(substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
		msg.interface = MAGUS_DSP_TDM_TX;
	else
		msg.interface = MAGUS_DSP_TDM_RX;
	
	msg.sampling_rate = substream->runtime->rate;
	msg.buf_addr = (uint32_t) buf->addr;
	msg.buf_size = 	snd_pcm_lib_buffer_bytes(substream);
	
	/* make sure frame size is aligned to 4 */
	msg.frame_size = (snd_pcm_lib_period_bytes(substream)/4) * 4;
	
	dbg("sent out TDM SYNC MSG:interface=%d,sample rate=%d,buf_addr=0x%x,buf_size=%d,frm_size=%d\n",
	     msg.interface, msg.sampling_rate, msg.buf_addr,msg.buf_size, msg.frame_size);
  /* fill the generic PIU message body */
  body.type = 1;
  body.len = sizeof(msg);
  memcpy(body.p, &msg, body.len);
  /* transmit the message to DSP */

  if( piu_tx(PIU_ALSA_CTL_QID, &body) != 0)
  {
		printk ("Error: Tx PIU msg failed\n");
		return -1;
	}

  dbg("exit magus_pcm_tdm_sync func OK\n");
	return 0;
}
#endif /*ifdef CALARM_APB_ENABLE*/

static snd_pcm_uframes_t magus_pcm_pointer(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct magus_runtime_data *prtd = runtime->private_data;
	snd_pcm_uframes_t tmp;
	tmp = bytes_to_frames(runtime, prtd->pcm_buf_pos);
	//dbg("enter magus_pcm_pointer func tmp=%d, position=0x%x\n", tmp, prtd->pcm_buf_pos);
	
	return tmp;
}

static int magus_pcm_open(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct magus_runtime_data *prtd;
	int ret;
	unsigned int dsp_phy_addr;
	dbg("enter magus_pcm_open func\n");
	
	snd_soc_set_runtime_hwparams(substream, &magus_pcm_hardware);

	/*
	 * Due to the BTDMP implementation on DSP side, the buffer size and frame size will need to aligned to 32 bytes,
	 * which is the TDM buffer transfer per interrupt. Let's add a rule to enforce that.
	 */
	ret = snd_pcm_hw_constraint_step(runtime, 0,
		SNDRV_PCM_HW_PARAM_PERIOD_BYTES, 32);
	if (ret)
		goto out;

	ret = snd_pcm_hw_constraint_step(runtime, 0,
		SNDRV_PCM_HW_PARAM_BUFFER_BYTES, 32);
	if (ret)
		goto out;

	ret = snd_pcm_hw_constraint_integer(runtime, SNDRV_PCM_HW_PARAM_PERIODS);
	if (ret < 0)
		goto out;

	prtd = kzalloc(sizeof(struct magus_runtime_data), GFP_KERNEL);
	if (prtd == NULL) {
		ret = -ENOMEM;
		goto out;
	}
#ifdef DUMMY_TIMER_TEST	
	init_timer(&prtd->timer);
	prtd->timer.data = (unsigned long) prtd;
	prtd->timer.function = magus_pcm_timer_function;
#endif	
	spin_lock_init(&prtd->lock);
	prtd->substream = substream;		

	runtime->private_data = prtd;
	
	g_prtd = prtd;

	/* enable PIU communication */
	dbg("enable PIU Tx and RX\n"); 
	alsa_piu_enable(1);
	
	dbg("register callback\n"); 
	alsa_cb_register(magus_pcm_callback);
	
	if(substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
		dsp_phy_addr = MAGUS_DSP_TDM_TX2_BASE_ADDR;
	else
		dsp_phy_addr = MAGUS_DSP_TDM_RX2_BASE_ADDR;
	
	/* IO re-map for TDM register block memory */
	magus_dsp_tdm_base_addr = (unsigned int)ioremap_nocache(dsp_phy_addr, 0x2000);
	if (0 == magus_dsp_tdm_base_addr)
	{
    	kfree(prtd);
	    ret = -ENOMEM;
    	goto out;
	}
	
	dbg("TDM phy addr= 0x%x, virt addr=0x%x\n", dsp_phy_addr, magus_dsp_tdm_base_addr);	
	dbg("exit magus_pcm_open func OK\n");
	
	return 0;
	
 out:
 	dbg("exit magus_pcm_open func error, %d\n",ret);
	return ret;
}

static int magus_pcm_close(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct magus_runtime_data *prtd = runtime->private_data;
	//int i = 0;
	
	dbg("enter magus_pcm_close func\n");
	
	magus_pcm_apb_state = 0;
	*((unsigned int *)(magus_dsp_tdm_base_addr+4)) = 0;
	/* configure TDM to disconnect the IRQ, disable TDMs*/
	magus_pcm_tdm_config(substream, 0);
	/* unregister callback if it havenot */
	alsa_cb_register(NULL); 
	mdelay(100);
	
	/* disable PIU communication */
	dbg("Disable PIU Tx and RX\n"); 
	printk("Disable PIU Tx and RX\n");
	alsa_piu_enable(0);
	
	/* IO unmap */
	if(magus_dsp_tdm_base_addr)
		iounmap((void *)magus_dsp_tdm_base_addr);
	
	kfree(prtd);
	g_prtd = NULL;
	dbg("exit magus_pcm_close func\n");
	
	return 0;
}

static int magus_pcm_mmap(struct snd_pcm_substream *substream,
	struct vm_area_struct *vma)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
  
  dbg("enter magus_pcm_mmap func\n");
	return dma_mmap_writecombine(substream->pcm->card->dev, vma,
				     runtime->dma_area,
				     runtime->dma_addr,
				     runtime->dma_bytes);
}

struct snd_pcm_ops magus_pcm_ops = {
	.open		= magus_pcm_open,
	.close		= magus_pcm_close,
	.ioctl		= snd_pcm_lib_ioctl,
	.hw_params	= magus_pcm_hw_params,
	.hw_free	= magus_pcm_hw_free,
	.prepare	= magus_pcm_prepare,
	.trigger	= magus_pcm_trigger,
	.pointer	= magus_pcm_pointer,
	.mmap		= magus_pcm_mmap,
};

static int magus_pcm_preallocate_dma_buffer(struct snd_pcm *pcm, int stream)
{
	struct snd_pcm_substream *substream = pcm->streams[stream].substream;
	struct snd_dma_buffer *buf = &substream->dma_buffer;
	size_t size = magus_pcm_hardware.buffer_bytes_max;
	
	dbg("enter magus_pcm_preallocate_dma_buffer func\n");
	
	buf->dev.type = SNDRV_DMA_TYPE_DEV;
	buf->dev.dev = pcm->card->dev;
	buf->private_data = NULL;
	buf->area = dma_alloc_writecombine(pcm->card->dev, size,
					   &buf->addr, GFP_KERNEL);
	if (!buf->area)
	{
		dbg("exit magus_pcm_preallocate_dma_buffer func Failed\n");
		return -ENOMEM;
	}
	buf->bytes = size;
	
	dbg("buf allocated at buf->addr=0x%x, buf->area=0x%x, size=%d\n", (unsigned int)buf->addr,(unsigned int) buf->area, (unsigned int)buf->bytes);
	printk("buf allocated at buf->addr=0x%x, buf->area=0x%x, size=%d\n", 
			(unsigned int)buf->addr,(unsigned int) buf->area, (unsigned int)buf->bytes);
	dbg("exit magus_pcm_preallocate_dma_buffer func OK\n");
	return 0;
}

static void magus_pcm_free_dma_buffers(struct snd_pcm *pcm)
{
	struct snd_pcm_substream *substream;
	struct snd_dma_buffer *buf;
	int stream;

	for (stream = 0; stream < 2; stream++) {
		substream = pcm->streams[stream].substream;
		if (!substream)
			continue;

		buf = &substream->dma_buffer;
		if (!buf->area)
			continue;

		dma_free_writecombine(pcm->card->dev, buf->bytes,
				      buf->area, buf->addr);
		buf->area = NULL;
	}
}

static u64 magus_pcm_dmamask = DMA_32BIT_MASK;

int magus_pcm_new(struct snd_card *card, struct snd_soc_codec_dai *dai,
	struct snd_pcm *pcm)
{
	int ret = 0;
 	 
  dbg("enter magus_pcm_new func\n");
  
	if (!card->dev->dma_mask)
		card->dev->dma_mask = &magus_pcm_dmamask;
	if (!card->dev->coherent_dma_mask)
		card->dev->coherent_dma_mask = DMA_32BIT_MASK;

	if (dai->playback.channels_min) {
		ret = magus_pcm_preallocate_dma_buffer(pcm,
			SNDRV_PCM_STREAM_PLAYBACK);
		if (ret)
			goto out;
	}

	if (dai->capture.channels_min) {
		ret = magus_pcm_preallocate_dma_buffer(pcm,
			SNDRV_PCM_STREAM_CAPTURE);
		if (ret)
			goto out;
	}

 out:
 	dbg("exit magus_pcm_new func %d\n", ret);
	return ret;
}

struct snd_soc_platform magus_soc_platform = {
	.name		= "magus-audio",
	.pcm_ops 	= &magus_pcm_ops,
	.pcm_new	= magus_pcm_new,
	.pcm_free	= magus_pcm_free_dma_buffers,
};

EXPORT_SYMBOL_GPL(magus_soc_platform);

MODULE_AUTHOR("JF Liu");
MODULE_DESCRIPTION("Magus PCM module");
MODULE_LICENSE("GPL");
