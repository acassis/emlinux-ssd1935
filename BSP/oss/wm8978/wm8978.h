/*
 * wm8978.h  --  WM8978 Soc Audio driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _WM8978_H
#define _WM8978_H

/* WM8978 register space */

#define WM8978_RESET		0x0
#define WM8978_POWER1		0x1
#define WM8978_POWER2		0x2
#define WM8978_POWER3		0x3
#define WM8978_IFACE		0x4
#define WM8978_COMP			0x5
#define WM8978_CLOCK		0x6
#define WM8978_ADD			0x7
#define WM8978_GPIO			0x8
#define WM8978_JACK1        0x9
#define WM8978_DAC			0xa
#define WM8978_DACVOLL	    0xb
#define WM8978_DACVOLR      0xc
#define WM8978_JACK2        0xd
#define WM8978_ADC			0xe
#define WM8978_ADCVOLL		0xf
#define WM8978_ADCVOLR      0x10
#define WM8978_EQ1			0x12
#define WM8978_EQ2			0x13
#define WM8978_EQ3			0x14
#define WM8978_EQ4			0x15
#define WM8978_EQ5			0x16
#define WM8978_DACLIM1		0x18
#define WM8978_DACLIM2		0x19
#define WM8978_NOTCH1		0x1b
#define WM8978_NOTCH2		0x1c
#define WM8978_NOTCH3		0x1d
#define WM8978_NOTCH4		0x1e
#define WM8978_ALC1			0x20
#define WM8978_ALC2			0x21
#define WM8978_ALC3			0x22
#define WM8978_NGATE		0x23
#define WM8978_PLLN			0x24
#define WM8978_PLLK1		0x25
#define WM8978_PLLK2		0x26
#define WM8978_PLLK3		0x27
#define WM8978_VIDEO		0x28
#define WM8978_3D           0x29
#define WM8978_BEEP         0x2b
#define WM8978_INPUT		0x2c
#define WM8978_INPPGAL  	0x2d
#define WM8978_INPPGAR      0x2e
#define WM8978_ADCBOOSTL	0x2f
#define WM8978_ADCBOOSTR    0x30
#define WM8978_OUTPUT		0x31
#define WM8978_MIXL	        0x32
#define WM8978_MIXR         0x33
#define WM8978_HPVOLL		0x34
#define WM8978_HPVOLR       0x35
#define WM8978_SPKVOLL      0x36
#define WM8978_SPKVOLR      0x37
#define WM8978_OUT3MIX		0x38
#define WM8978_MONOMIX      0x39

#define WM8978_CACHEREGNUM 	58

/*
 * WM8978 Clock dividers
 */
#define WM8978_MCLKDIV 		0
#define WM8978_BCLKDIV		1
#define WM8978_OPCLKDIV		2
#define WM8978_DACOSR		3
#define WM8978_ADCOSR		4
#define WM8978_MCLKSEL		5

#define WM8978_MCLK_MCLK		(0 << 8)
#define WM8978_MCLK_PLL			(1 << 8)

#define WM8978_MCLK_DIV_1		(0 << 5)
#define WM8978_MCLK_DIV_1_5		(1 << 5)
#define WM8978_MCLK_DIV_2		(2 << 5)
#define WM8978_MCLK_DIV_3		(3 << 5)
#define WM8978_MCLK_DIV_4		(4 << 5)
#define WM8978_MCLK_DIV_5_5		(5 << 5)
#define WM8978_MCLK_DIV_6		(6 << 5)

#define WM8978_BCLK_DIV_1		(0 << 2)
#define WM8978_BCLK_DIV_2		(1 << 2)
#define WM8978_BCLK_DIV_4		(2 << 2)
#define WM8978_BCLK_DIV_8		(3 << 2)
#define WM8978_BCLK_DIV_16		(4 << 2)
#define WM8978_BCLK_DIV_32		(5 << 2)

#define WM8978_DACOSR_64		(0 << 3)
#define WM8978_DACOSR_128		(1 << 3)

#define WM8978_ADCOSR_64		(0 << 3)
#define WM8978_ADCOSR_128		(1 << 3)

#define WM8978_OPCLK_DIV_1		(0 << 4)
#define WM8978_OPCLK_DIV_2		(1 << 4)
#define WM8978_OPCLK_DIV_3		(2 << 4)
#define WM8978_OPCLK_DIV_4		(3 << 4)

struct wm8978_setup_data {
	unsigned short i2c_address;
};

extern int g_running;
extern struct snd_soc_codec_dai wm8978_dai;
extern struct snd_soc_codec_device soc_codec_dev_wm8978;



/* wm8987 context support */
////////////////////////////////////////////////////////
#include <sound/soc.h>
#include <sound/soc-dapm.h>

#define WM8978_INIT_CMD              10
#define WM8978_POWER_DOWN_SET_CMD    20
#define WM8978_SAMPLE_RATE_SET_CMD   30
#define WM8978_OUTPUT_VOLUME_SET_CMD 40
#define WM8978_INPUT_VOLUME_SET_CMD  50
#define WM8978_SHUTDOWN_CMD          60
#define WM8978_REGISTER_RESTORE_CMD  70

#define WM8978_GENERIC_REGISTER_SET_CMD   80
#define WM8978_GENERIC_REGISTER_GET_CMD   90


/* Audio control related message Id */
#define PIUMSG_AUDIOCTL_TX	0x13
#define PIUMSG_AUDIOCTL_RX	0x14

#define PIU_AUDIO_CTL_QID      2

#define PIU_CMD	1
#define PIU_REP	2
/* Sample Rates */
typedef enum
{
	SAMPLE_RATE_ADC_96_DAC_96		    = 0x07,
	SAMPLE_RATE_ADC_88_2_DAC_88_2		= 0x0F,
	SAMPLE_RATE_ADC_48_DAC_48		    = 0x00,
	SAMPLE_RATE_ADC_44_1_DAC_44_1		= 0x08,
	SAMPLE_RATE_ADC_32_DAC_32		    = 0x06,
	SAMPLE_RATE_ADC_8_021_DAC_8_021	= 0x0B,
	SAMPLE_RATE_ADC_8_DAC_8		      = 0x03
} SAMPLE_RATES;

struct acmsg_t
{
  unsigned char		msgid;	// msg id
  unsigned int  	data;	// one 32 bits data for sending
  unsigned int	  buf_addr;	// data buf address	
  unsigned int    len;	// data length
};

struct acmsg_rep_t
{
 unsigned char		msgid;	// msg id
 unsigned int		ret;	// return
};

struct piu_msg_t
{
	unsigned int	type;
	unsigned int	len;
	unsigned char	p[64];
};

extern struct snd_soc_device *g_socdev;

unsigned int wm8978_read_reg_cache(struct snd_soc_codec *codec,
	                                 unsigned int reg);
int wm8978_write(struct snd_soc_codec *codec, unsigned int reg,
	unsigned int value);

int wm8978_set_dai_pll(struct snd_soc_codec_dai *codec_dai,
		int pll_id, unsigned int freq_in, unsigned int freq_out);
////////////////////////////////////////////////////////

#endif
