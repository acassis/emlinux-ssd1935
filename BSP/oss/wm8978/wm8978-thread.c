/*
 * linux/sound/magus/magus-pcm-thread.c -- ALSA PCM interface for the Magus chip
 *
 * Author:	JF Liu
 * Created:	Nov 30, 2007
 * Copyright:	(C) 2007 Solomon Systech
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/version.h>

#if defined(MODVERSIONS)
#include <linux/modversions.h>
#endif
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/signal.h>

#include <asm/semaphore.h>

#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <asm/hardware.h>
#include <asm/gpio.h>

#include "../../oss/magus-pcm/magus-pcm.h"
#include "wm8978.h"
#include "wm8978-thread.h"

#define WM8978_THREAD_DEBUG 0
#define  DETECT_AMPLIFIER 1
#if WM8978_THREAD_DEBUG
#define dbg(format, arg...) printk(format, ## arg)
#else
#define dbg(format, arg...)
#endif
/* Local func prototype */
extern int piu_tx(uint32_t msg, piu_msg_p body);
/* wm8978 thread data */
wm8978_thread_t wm8978_thread_data;

/* codec register cache from DSP */
u16 wm8978_reg_dsp_cache[WM8978_CACHEREGNUM];

u8 wm8978_reg_dsp_cache_flag = 0;
/* public functions */


/* This callback function will be invoked when receive cmd from DSP */
/* This func will directly configure codec registers */
void wm8978_audio_ctl_callback_func(void* data)
{
  struct acmsg_t* acmsg = (struct acmsg_t*)data;
	char cmd;
  struct snd_soc_codec *codec = g_socdev->codec;

	if (acmsg->msgid != PIUMSG_AUDIOCTL_TX)
	{
    printk("WM8978 Codec Err: Invalid msg from DSP %d\n", acmsg->msgid);
		return;
  }
	cmd = (acmsg->data >> 16) & 0xFF;
	if(cmd == WM8978_INIT_CMD) /* the first command */
  {
    dbg("WM8978_thread started\n");
    wm8978_reg_dsp_cache_flag = 1;
	  codec->restore_flag = 1;
  }
#if 0
  else if(cmd == WM8978_SHUTDOWN_CMD)
  {
    dbg("WM8978_thread killed \n");
    wm8978_reg_dsp_cache_flag = 0;
    stop_wm8978_thread(&wm8978_thread_data);
    init_completion(&wm8978_thread_data.flag);
    return;      
	}
#endif
  /* set the completion flag to wakeup the thread */
  wm8978_thread_data.data = acmsg->data;
  complete(&wm8978_thread_data.flag);	
  
  return;
}

int wm8978_thread(void* data)
{
  char cmd, data1,data2;  
  unsigned int clk = 0;
  unsigned int clkout = 0;
  struct piu_msg_t	ac_piu_r;
  struct acmsg_rep_t	acmsg_rep;
  int ret, reg, i,state;
  u16 DAC_reg;
  struct snd_soc_codec *codec = g_socdev->codec;
	 wm8978_thread_t* thread_data = data;

  
  /* store all register values configured from DSP */
  memcpy(wm8978_reg_dsp_cache, codec->reg_cache, sizeof(wm8978_reg_dsp_cache));      

  /* main loop to handle DSP codec config requests */
  for(;;)
  {
    /* fall asleep for one second */
    ret = wait_for_completion_interruptible_timeout(&thread_data->flag, HZ);
    if (ret==0)                // time out
     {
      /* check if a terminate signal received or not */
             if (thread_data->terminate)
     			 break;
     		 else
       		 continue;
    }
    if (ret == -ERESTARTSYS)
                        break;

	            
    /* check if a terminate signal received or not */
    if (thread_data->terminate)
      break;    
	
    cmd = (thread_data->data >> 16) & 0xFF;
	  data1 = (thread_data->data) & 0xFF;
	  data2 = (thread_data->data >>8) & 0xFF;
  	switch(cmd)
	  {
		case WM8978_INIT_CMD: /* codec init command */

#if 0
/* cody */
//wm8978_write(codec,WM8978_RESET,0X0000);
	wm8978_write(codec, WM8978_POWER1, 0x012d);
	wm8978_write(codec, WM8978_POWER2, 0x0180);
	wm8978_write(codec, WM8978_POWER3, 0x000f);

#endif
#if 0
      /* turn on codec */
      wm8978_write(codec, WM8978_PWR, 0x4e);
	      
      /* enable DAC */	    
      reg = wm8978_read_reg_cache(codec, WM8978_APANA);
	    wm8978_write(codec, WM8978_APANA, reg | 0x0010);

      /* Disable Digital Mute */
      reg = wm8978_read_reg_cache(codec, WM8978_APDIGI) & 0xfff7;
  		wm8978_write(codec, WM8978_APDIGI, reg);

#if 1
    	/* enable input line */
      reg = wm8978_read_reg_cache(codec, WM8978_LINVOL) & 0xff7f;
	    wm8978_write(codec, WM8978_LINVOL, reg);
	    reg = wm8978_read_reg_cache(codec, WM8978_RINVOL) & 0xff7f;
	    wm8978_write(codec, WM8978_RINVOL, reg);
     
	    /* set ADC input (default is line-in) */
	    
	    /* enable Microphone */
#endif 	    
	    /* set to 16bits format */      
      reg = wm8978_read_reg_cache(codec, WM8978_IFACE) & 0xfff3;
	    wm8978_write(codec, WM8978_IFACE, reg);

	    /* set to Master mode */      
      reg = wm8978_read_reg_cache(codec, WM8978_IFACE);
	    wm8978_write(codec, WM8978_IFACE, reg | 0x0040);

	    /* set Data format to DSP */
      reg = wm8978_read_reg_cache(codec, WM8978_IFACE);
	    wm8978_write(codec, WM8978_IFACE, reg | 0x0003);

    	/* set sample rate for default 48k, it will be changed later base on stream info */
      /* Normal mode, Mclk:12.288Mhz, BOSR:0 (256fs), SR3-SR0:0000 */
      wm8978_write(codec, WM8978_SRATE, 0x0000);
#if 0	    
  	  /* disable bypass */
      reg = wm8978_read_reg_cache(codec, WM8978_APANA)& 0xFFF7;
	    wm8978_write(codec, WM8978_APANA, reg);
#endif
	    /* activate Digital Interface */
      wm8978_write(codec, WM8978_ACTIVE, 0x1);

		  dbg("codec Init ok\n");
#endif

      break;
		case WM8978_POWER_DOWN_SET_CMD:
#if 1
      /* Set power down register */
	DAC_reg = wm8978_read_reg_cache(codec,WM8978_DAC) & 0xffbf;;
	 dbg("WM8978_POWER_DOWN_SET_CMD\n");
      if(data1 == 0xff)
      {
        /* de-activate Digital Interface */
       // wm8987_write(codec, WM8987_ACTIVE, 0x0);
       dbg("de-active digital interface\n");		
		
	#if DETECT_AMPLIFIER
			gpio_set_value(GPIO_NUM(3,11),0);
			g_running = 0;
		#endif
		wm8978_write(codec,WM8978_DAC,DAC_reg|0x40);
  		wm8978_write(codec, WM8978_POWER1, 0x0003);
		wm8978_write(codec, WM8978_POWER2, 0x0);
		wm8978_write(codec, WM8978_POWER3, 0x0);
      }
      else
      {
      	dbg("power up codec\n");
		wm8978_write(codec,WM8978_DAC,DAC_reg|0x40);
		#if DETECT_AMPLIFIER
		
			gpio_get_value(GPIO_NUM(3,13));
			state = gpio_get_value(GPIO_NUM(3,13));
			if(state)
				gpio_set_value(GPIO_NUM(3,11),0);
			else
				gpio_set_value(GPIO_NUM(3,11),1);
			g_running = 1;
		#endif
		wm8978_write(codec, WM8978_POWER1, 0x012d);
		wm8978_write(codec, WM8978_POWER3, 0x000f);
		wm8978_write(codec, WM8978_POWER2, 0x0180);
		wm8978_write(codec,WM8978_DAC,DAC_reg);
      }
#endif
      break;
		case WM8978_SAMPLE_RATE_SET_CMD:
                        switch(data1)
                        {
                                case SAMPLE_RATE_ADC_96_DAC_96:
								case SAMPLE_RATE_ADC_48_DAC_48:
          /*Rate: 96Khz USB  mode, Mclk:12Mhz, BOSR:0 (256fs), SR4-SR0:01110 */
         wm8978_write(codec, WM8978_ADD, 0x0000);
		 clk = 12000000;
		 clkout = 12280000;
          break;
                                case SAMPLE_RATE_ADC_44_1_DAC_44_1:
          /*Rate: 44.1Khz USB mode, Mclk:12Mhz, BOSR:0 (256fs), SR4-SR0:10001 */
                  wm8978_write(codec, WM8978_ADD, 0x0000);
		 clk = 12000000;
		 clkout = 11290000;
          break;
                                case SAMPLE_RATE_ADC_32_DAC_32:
          /*Rate: 32Khz USB mode, Mclk:12Mhz, BOSR:0 (256fs), SR4-SR0:01100 */
         wm8978_write(codec, WM8978_ADD, 0x0002);
		 clk = 12000000;
		 clkout = 12280000;
          break;
                                case SAMPLE_RATE_ADC_8_DAC_8:
          /*Rate: 8Khz USB mode, Mclk:12Mhz, BOSR:0 (256fs), SR4-SR0:00110 */
         wm8978_write(codec, WM8978_ADD, 0x000a);
		 clk = 12000000;
		 clkout = 12280000;
          break;
                                default:
          printk("WM8978 Codec: Wrong sampling rate from DSP, use default 48KHz\n");
         wm8978_write(codec, WM8978_ADD, 0x0000);
		 clk = 12000000;
		 clkout = 12280000;
          break;
                        }
		wm8978_set_dai_pll(g_socdev, WM8978_MCLK_PLL, clk, clkout);
                        dbg("sampling rate set =%d\n",data1);

#if 0
			switch(data1)
			{
				case SAMPLE_RATE_ADC_96_DAC_96: 
          /*Rate: 96Khz Normal mode, Mclk:12.288Mhz, BOSR:0 (256fs), SR3-SR0:0111 */
          wm8978_write(codec, WM8978_SRATE, 0x001c);	    
          break;
				case SAMPLE_RATE_ADC_48_DAC_48: 
          /*Rate: 48Khz Normal mode, Mclk:12.288Mhz, BOSR:0 (256fs), SR3-SR0:0000 */
          wm8978_write(codec, WM8978_SRATE, 0x0000);	    
          break;
				case SAMPLE_RATE_ADC_44_1_DAC_44_1: 
          /*Rate: 44.1Khz Normal mode, Mclk:12.288Mhz, BOSR:0 (256fs), SR3-SR0:1000 */
          wm8978_write(codec, WM8978_SRATE, 0x0020);	    
          break;
				case SAMPLE_RATE_ADC_32_DAC_32: 
          /*Rate: 32Khz Normal mode, Mclk:12.288Mhz, BOSR:0 (256fs), SR3-SR0:0110 */
          wm8978_write(codec, WM8978_SRATE, 0x0018);	    
          break;
				case SAMPLE_RATE_ADC_8_DAC_8:
          /*Rate: 8Khz Normal mode, Mclk:12.288Mhz, BOSR:0 (256fs), SR3-SR0:0011 */
          wm8978_write(codec, WM8978_SRATE, 0x000c);	    
          break;
				default:
          printk("WM8978 Codec: Wrong sampling rate from DSP, use default 48KHz\n");
          wm8978_write(codec, WM8978_SRATE, 0x0000);	    
          break;
			}
			dbg("sampling rate set =%d\n",data1);
#endif
      break;
		case WM8978_OUTPUT_VOLUME_SET_CMD:
      dbg("output volume value is %d, for ch %d\n", data1, data2);
	  #if 0
      if(data1 > 47)
        data1 = 47;
      if(data2 == 0) //left channel
      {
        reg = wm8978_read_reg_cache(codec, WM8978_HPVOLL) & 0xffc0; // clear bits 5:0
              wm8978_write(codec, WM8978_HPVOLL, reg | data1);
      }
      else if(data2 == 1) //right channel
      {
              reg = wm8978_read_reg_cache(codec, WM8978_HPVOLR) & 0xffc0;
              wm8978_write(codec, WM8978_HPVOLR, reg | data1);
      }
      else if(data2 == 2) //both channels
      {
        reg = wm8978_read_reg_cache(codec, WM8978_HPVOLL) & 0xffc0;
              wm8978_write(codec, WM8978_HPVOLL, reg | data1);
            reg = wm8978_read_reg_cache(codec,WM8978_HPVOLR) & 0xffc0;
              wm8978_write(codec, WM8978_HPVOLR, reg | data1);
      }
	  #endif
      break;
		case WM8978_INPUT_VOLUME_SET_CMD:
#if 0
      dbg("line-in volume value is %d, for ch %d\n", data1, data2);
      if(data1 > 0x1F)  // maximum +12dB
        data1 = 0x1F;
      if(data2 == 0) //left channel 
      {
        reg = wm8978_read_reg_cache(codec, WM8978_LINVOL) & 0xffe0; // clear bits 4:0;
	      wm8978_write(codec, WM8978_LINVOL, reg | data1);
      }
      else if(data2 == 1) //right channel
      {
	      reg = wm8978_read_reg_cache(codec, WM8978_RINVOL) & 0xffe0;;
	      wm8978_write(codec, WM8978_RINVOL, reg | data1);
      }
      else if(data2 == 2) //both channels
      {
        reg = wm8978_read_reg_cache(codec, WM8978_LINVOL) & 0xffe0;;
	      wm8978_write(codec, WM8978_LINVOL, reg | data1);
	      reg = wm8978_read_reg_cache(codec, WM8978_RINVOL) & 0xffe0;;
	      wm8978_write(codec, WM8978_RINVOL, reg | data1);
      }
#endif
      break;
    case WM8978_GENERIC_REGISTER_SET_CMD:
      dbg("write codec register 0x%x,with value=0x%x\n", data2, data1);
      wm8978_write(codec, data2, data1);
      break;
    case WM8978_GENERIC_REGISTER_GET_CMD:
      reg = wm8978_read_reg_cache(codec, data2);
      dbg("read codec register 0x%x value=0x%x\n", data2, reg);
      break;
#if 0
    case WM8978_REGISTER_RESTORE_CMD:
      /* restore the previous register values */
      for(i=0;i<WM8978_CACHE_REGNUM;i++)
        wm8978_write(codec, i, wm8978_reg_dsp_cache[i]);
      dbg("codec regisgters restored\n");
      break;
#endif
		default:
      printk("WM8978 Error: Invalid audio ctl cmd 0x%x from DSP\n", cmd);
      break;
	  } 
    /* store all register values configured from DSP */
    if( wm8978_reg_dsp_cache_flag == 1)
      memcpy(wm8978_reg_dsp_cache, codec->reg_cache, sizeof(wm8978_reg_dsp_cache));
    /* send out a response */  
	  acmsg_rep.msgid = PIUMSG_AUDIOCTL_TX;
	  acmsg_rep.ret = 1;
    /* for register read cmd, put the register value on lowest 8 bits */
    if(cmd == WM8978_GENERIC_REGISTER_GET_CMD)
      acmsg_rep.ret = (1<<8) + reg;
	  ac_piu_r.type = PIU_REP;
	  ac_piu_r.len = sizeof(acmsg_rep);
	  memcpy(ac_piu_r.p, &acmsg_rep, ac_piu_r.len);
    /* transmit the message to DSP */
    if( piu_tx(PIU_AUDIO_CTL_QID, &ac_piu_r) != 0)
      printk ("WM8978 Codec: Error tx PIU msg error\n"); 
  }
  codec->restore_flag = 0;
  /* here we go only in case of termination of the thread */

  /* cleanup the thread, leave */
  dbg("wm8978_thread killed\n");
  //thread_data->terminate = 1;
  thread_data->thread_task = NULL;
  // tell the parent process the thread is killed by itself
  complete_and_exit(&thread_data->thread_notifier, 0);

} 
/********************************END_OF_FILE*********************************/


