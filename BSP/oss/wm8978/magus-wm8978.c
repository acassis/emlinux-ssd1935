/*
 * magus.c  --  SoC audio for Magus Aphrodite
 *
 * Copyright 2005 Wolfson Microelectronics PLC.
 * Copyright 2005 Openedhand Ltd.
 *
 * Authors: Liam Girdwood <liam.girdwood@wolfsonmicro.com>
 *          Richard Purdie <richard@openedhand.com>
 *
 *
 * JF Liu updated for Magus Aphrodite platform, 10 Dec, 2007
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <sound/driver.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <linux/workqueue.h>
#include <asm/hardware.h>
#include <asm/gpio.h>
#include <asm/mach/irq.h>


#include "wm8978.h"
#include "../../oss/magus-pcm/magus-pcm.h"
#include "../../oss/magus-i2s/magus-i2s.h"


#define DETECT_POLL 0



#define MAGUS_DEBUG 0

#if MAGUS_DEBUG
#define dbg(format,args...) printk(KERN_ERR"magus-wm8978.c:%s() : "format,__FUNCTION__,##args);
#else
#define dbg(format, arg...)
#endif





static int magus_startup(struct snd_pcm_substream *substream)
{
	dbg("enter magus_startup func for wm8978\n");
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_codec *codec = rtd->socdev->codec;

	dbg("exit magus_startup func for wm8978\n");

	return 0;
}

static int magus_shutdown(struct snd_pcm_substream *substream)
{
	dbg("enter magus_shutdown func for wm8978\n");

	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_codec *codec = rtd->socdev->codec;
	dbg("exit magus_shutdown func for wm8978\n");

	return 0;
}

static int magus_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_codec_dai *codec_dai = rtd->dai->codec_dai;
	struct snd_soc_cpu_dai *cpu_dai = rtd->dai->cpu_dai;
	unsigned int clk = 0;
	unsigned int clkout=0;
	int ret = 0;
  dbg("enter magus_hw_params for wm8978  func\n");
	switch (params_rate(params)) {
	case 8000:
	//case 16000:
  case 32000:
	case 48000:
	case 96000:
		clk = 12000000;
		clkout = 12288000;//modified by santiego
		break;
	//case 11025:
	//case 22050:
	case 44100:
	case 88200: //Jianfeng added
		clk = 12000000;
		clkout = 11290000;
		break;
	}

	/* set codec DAI configuration */
	dbg("clk = %d,clkout = %d\n",clk,clkout);
	dbg("set codec DAI configuration\n");
	ret = codec_dai->dai_ops.set_fmt(codec_dai, SND_SOC_DAIFMT_DSP_B|
		SND_SOC_DAIFMT_NB_NF | SND_SOC_DAIFMT_CBM_CFM);
	if (ret < 0)
		return ret;
dbg("set cpu DAI configuration (unused) \n");
	/* set cpu DAI configuration (unused) */
	ret = cpu_dai->dai_ops.set_fmt(cpu_dai, SND_SOC_DAIFMT_DSP_B |
		SND_SOC_DAIFMT_NB_NF | SND_SOC_DAIFMT_CBS_CFM);//modefy by santiego
	if (ret < 0)
		return ret;

	/* set the codec system clock for DAC and ADC */
	/*	ret = codec_dai->dai_ops.set_sysclk(codec_dai, WM8731_SYSCLK, clk,
		SND_SOC_CLOCK_IN);
		*/
	
		dbg("set the codec system clock for DAC and ADC\n");
	ret = codec_dai->dai_ops.set_pll(codec_dai,WM8978_MCLK_PLL,clk,clkout);
	if (ret < 0)
		return ret;

	/* set the I2S system clock as input (unused) */
	dbg("set the I2S system clock as input (unused)\n");
	ret = cpu_dai->dai_ops.set_sysclk(cpu_dai, MAGUS_I2S_SYSCLK, 0,
		SND_SOC_CLOCK_IN);
	if (ret < 0)
		return ret;

  dbg("exit magus_hw_params func for wm8978 ok \n");
	return 0;
}

static struct snd_soc_ops magus_ops = {
	.startup = magus_startup,
	.hw_params = magus_hw_params,
	.shutdown = magus_shutdown,
};

/*
 * Logic for a wm8978 as connected on a ARM Device
 */
static int magus_wm8978_init(struct snd_soc_codec *codec)
{
	int i, err;
  dbg("enter magus_wm8978_init func\n");
#if 0
	snd_soc_dapm_set_endpoint(codec, "LLINEIN", 0);
	snd_soc_dapm_set_endpoint(codec, "RLINEIN", 0);
	snd_soc_dapm_set_endpoint(codec, "MICIN", 1);

	/* Add magus specific controls here, if any */
	snd_soc_dapm_set_endpoint(codec, "DAC", 1);
	
	/* sync the registers */
	snd_soc_dapm_sync_endpoints(codec);
#endif	
	dbg("exit magus_wm8978_init func\n");
	return 0;
}

/* magus digital audio interface glue - connects codec <--> CPU */
static struct snd_soc_dai_link magus_dai = {
	.name = "WM8978",
	.stream_name = "WM8978",
	.cpu_dai = &magus_i2s_dai,
	.codec_dai = &wm8978_dai,
	.init = magus_wm8978_init,
	.ops = &magus_ops,
};

/* magus audio machine driver */
static struct snd_soc_machine snd_soc_machine_magus = {
	.name = "magus_audio",
	.dai_link = &magus_dai,
	.num_links = 1,
};

/* magus audio private data */
static struct wm8978_setup_data magus_wm8978_setup = {
	.i2c_address = 0x1b,
};

/* magus audio subsystem */
static struct snd_soc_device magus_snd_devdata = {
	.machine = &snd_soc_machine_magus,
	.platform = &magus_soc_platform,
	.codec_dev = &soc_codec_dev_wm8978,
	.codec_data = &magus_wm8978_setup,
};



static struct platform_device *magus_snd_device;

/////////////////////////////////////////////////////////
///////////////////////////////////////////////////////
#if DETECT_POLL
#define DETECT_DELAY (HZ)
#define HPIN 0 
#define HPOUT 1
static int gpio_hp_in = 0;
static int gpio_amplifier_out = 0;
static struct timer_list detect_timer;
/*
save the status last detection
*/
static int key_status = HPIN;

static void detect_timer_handle()
{
	//dbg("enter......\n");
	int ret,irq;
	 gpio_get_value(gpio_hp_in);
    int val = gpio_get_value(gpio_hp_in);
	//headphone in
	if(val)
	{
		if(key_status == HPOUT)
		{
			printk(KERN_ERR"HPOUT-->HPIN,codec running?%d",g_running);
			if(g_running)
				gpio_set_value(gpio_amplifier_out,0);//disable amplifier
			
		}
		key_status = HPIN;
	}else
	{
		if(key_status == HPIN)
		{
			printk(KERN_ERR"HPIN-->HPOUT,codec running?%d",g_running);
		/*
			if codec is stop,keep amplifier close
		*/
			if(g_running)
				gpio_set_value(gpio_amplifier_out,1);
			
		}
		key_status = HPOUT;	
	}
	del_timer(&detect_timer);
	detect_timer.expires = jiffies + DETECT_DELAY;
	add_timer(&detect_timer);
	//dbg("exit......\n");

}

#endif
//////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
static int __init magus_init(void)
{
	int ret,i,state;
  dbg("enter magus_init func for wm8978\n");
  /* add magus to device */

	magus_snd_device = platform_device_alloc("soc-audio", -1);
	if (!magus_snd_device)
		return -ENOMEM;
  
	platform_set_drvdata(magus_snd_device, &magus_snd_devdata);
	magus_snd_devdata.dev = &magus_snd_device->dev;
	dbg("before platform_device_Add\n");
	ret = platform_device_add(magus_snd_device);
  if(ret)
  {
  /* adding device error */
	platform_device_put(magus_snd_device);

  	dbg("exit magus_init func Error\n");
	return ret;

  }
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#if DETECT_POLL
gpio_hp_in = GPIO_NUM(3,13);
gpio_amplifier_out = GPIO_NUM(3,11);
init_timer(&detect_timer);
	detect_timer.function = detect_timer_handle;
	gpio_direction_input(gpio_hp_in);//intiate pd14,in
	gpio_direction_output(gpio_amplifier_out,0);//initiate pd3,out

  //close the amplifier first
  gpio_direction_output(gpio_amplifier_out,0);
		detect_timer.expires = jiffies + DETECT_DELAY;
		add_timer(&detect_timer);

#endif
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

  
      dbg("exit magus_init func OK, device added\n");
      return 0;
}

static void __exit magus_exit(void)
{
	dbg("enter");
	#if DETECT_POLL
	del_timer(&detect_timer);
	#endif
	platform_device_unregister(magus_snd_device);
	dbg("exit...");
}

module_init(magus_init);
module_exit(magus_exit);

/* Module information */
MODULE_AUTHOR("JF Liu");
MODULE_DESCRIPTION("ALSA SoC Magus");
MODULE_LICENSE("GPL");
