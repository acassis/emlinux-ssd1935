/*
 * board/magus/magus.c
 *
 * (c) Copyright 2007
 * Solomon Systech Ltd
 *
 * Sasi Nair <sasin@solomon-systech.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/byteorder.h>
#include "nand.h"
#include "nfcr.h"
#include "nfc.h"
#include "../../../include/nboot.h"
#include "../../../include/config.h"
#ifdef CONFIG_CPT480X272
#include "plogo480x272.h"
#include "spi.h"
#elif CONFIG_TPO800X480
//#include "plogo800x480.h"		/* 800x480 bitmap too large to include in bootloader... */
#include "spi.h"
#else
#include "plogo.h"
#endif

DECLARE_GLOBAL_DATA_PTR;

#define REGD(a)		(*(volatile uint32_t *)(a))
void burst8(char *dst, char *src, int len);

int board_init (void)
{
	gd->bd->bi_arch_number = 1933;
	gd->bd->bi_boot_params = CONFIG_LOAD_BASE_ADDR + 0x100;	/* adress of boot parameters	*/
	icache_enable();

#if 0
	/* CS0-5: CS0-SRAM, CS4-LAN */
	REGD(0x0800100c) = 0xff;
	REGD(0x08001010) = 0x1d0020;
	REGD(0x08001020) = 0x1d0021;
	REGD(0x08001030) = 0x1d0021;
	REGD(0x08001040) = 0x1d0021;
	REGD(0x08001050) = 0x1d0020;	/* LAN needs more cycles, no */
	REGD(0x08001054) = 0x111122;
#endif
	return 0;
}

#ifdef CONFIG_CPT480X272
#define WIN_SIZE	0x1FE00		/* 480 * 272 */
#elif CONFIG_TPO800X480
#define WIN_SIZE	0x5DC00		/* 800 * 480 */
#else
#define WIN_SIZE	0x12c00		/* 320 * 240 */
uint8_t logo[LOGO_SIZE];
#endif

int board_late_init (void)
{
#if 0
	setenv ("stdout", "serial");
	setenv ("stderr", "serial");
#endif

	nfc_t		t;
	nand_id_t	id;
	nand_geo_t	geo;
	char		*p;
	uint32_t	*nfcbuf;
	int		pgsz, pg, pages;
	int		i, j = 0;
#ifdef CONFIG_BPP32
	uint32_t	fb32;
#else
	uint16_t	fb16;
#endif
	int		ppb, pgcnt, pgmax, blocks;
	int		delay;

	/* lcdc configure */	
#ifdef CONFIG_CPT480X272
	lcm_ssd2123_init();					// init CPT LCM (SSD2123 driver IC)
	/* LCD DEN high */
	REGD(0x0810f118) |= 0x00000008;
	REGD(0x0810f120) |= 0x00000008;
	REGD(0x0810f13c) |= 0x00000008;

	REGD(0x08005008) = 0x00000007;
	REGD(0x08005014) = 0x00012214;		// pixel clk freq ~8.54MHz(60fps) (0x0001526E ~10MHz (70fps))
	REGD(0x08005018) = 0x00000071;
	REGD(0x08005020) = 0x01FF0010;		// horizontal total = 512, horizontal display period start = 16
	REGD(0x08005024) = 0x01150004;		// vertical total = 278, vertical display period start = 4
	REGD(0x08005028) = 0x01000000;
	REGD(0x0800502c) = 0x01000000;
	REGD(0x08005030) = 0x00000000;
	REGD(0x08005034) = 0x00000000;
	REGD(0x08005038) = 0x00000000;
	REGD(0x0800503c) = 0x00000000;	
	REGD(0x08005058) = 0x00000000;
	REGD(0x0800505C) = 0x50000000;
#ifdef CONFIG_BPP32
	REGD(0x08005050) = 0x00062006;
	REGD(0x08005060) = 0x00000780;		// 480 x 4 bytes (32bpp)
#else	
	REGD(0x08005050) = 0x00062005;
	REGD(0x08005060) = 0x000003C0;		// 480 x 2 bytes (16bpp)
#endif		
	REGD(0x08005064) = 0x010F01DF;		// 272 x 480
	REGD(0x08005008) = 0x00000001;
	/* flt win 2 */
#ifdef CONFIG_BPP32	
	REGD(0x08005054) = 0x00003608;	/* 8888 */		
	REGD(0x0800507c) = 0x00000780;	
#else	
	REGD(0x08005054) = 0x00002d08;	/* 1555 */		
	REGD(0x0800507c) = 0x000003C0;
#endif	
	REGD(0x08005078) = FLT2_ADDR;
	REGD(0x08005084) = 0x010F01DF;
#elif CONFIG_TPO800X480
	lcm_ssd2123_init();					// init TPO LCM
	/* LCD DEN high */
	REGD(0x0810f118) |= 0x00000008;
	REGD(0x0810f120) |= 0x00000008;
	REGD(0x0810f13c) |= 0x00000008;

	REGD(0x08005008) = 0x00000007;
	REGD(0x08005014) = 0x000467B6;		// pixel clk freq ~33.2MHz (60fps)
	REGD(0x08005018) = 0x00000071;
	REGD(0x08005020) = 0x041F00D8;		// horizontal total = 1056, horizontal display period start = 216
	REGD(0x08005024) = 0x020C0023;		// vertical total = 525, vertical display period start = 35
	REGD(0x08005028) = 0x01000000;
	REGD(0x0800502c) = 0x01000000;
	REGD(0x08005030) = 0x00000000;
	REGD(0x08005034) = 0x00000000;
	REGD(0x08005038) = 0x00000000;
	REGD(0x0800503c) = 0x00000000;	
	REGD(0x08005058) = 0x00000000;
	REGD(0x0800505C) = 0x50000000;
#ifdef CONFIG_BPP32
	REGD(0x08005050) = 0x00060006;
	REGD(0x08005060) = 0x00000C80;		// 800 x 4 bytes (32bpp)
#else	
	REGD(0x08005050) = 0x00060005;
	REGD(0x08005060) = 0x00000640;		// 800 x 2 bytes (16bpp)
#endif		
	REGD(0x08005064) = 0x01DF031F;		// 480 x 800
	REGD(0x08005008) = 0x00000001;
	/* flt win 2 */
#ifdef CONFIG_BPP32	
	REGD(0x08005054) = 0x00003608;	/* 8888 */		
	REGD(0x0800507c) = 0x00000C80;	
#else	
	REGD(0x08005054) = 0x00002d08;	/* 1555 */		
	REGD(0x0800507c) = 0x00000640;
#endif	
	REGD(0x08005078) = FLT2_ADDR;
	REGD(0x08005084) = 0x01DF031F;	
#else	
	/* LCD DEN down */	
	REGD(0x0810f118) |= 0x00000008;
	REGD(0x0810f120) |= 0x00000008;
	REGD(0x0810f13c) &= ~0x00000008;

	REGD(0x08005008) = 0x00000007;
	REGD(0x08005014) = 0x00014555;
	REGD(0x08005018) = 0x00000071;
	REGD(0x08005020) = 0x01970044;
	REGD(0x08005024) = 0x01050012;
	REGD(0x08005028) = 0x01000000;
	REGD(0x0800502c) = 0x01000000;
	REGD(0x08005030) = 0x00000000;
	REGD(0x08005034) = 0x00000000;
	REGD(0x08005038) = 0x00000000;
	REGD(0x0800503c) = 0x00000000;
	REGD(0x08005058) = 0x00000000;
	REGD(0x0800505C) = 0x50000000;
#ifdef CONFIG_BPP32
    REGD(0x08005050) = 0x00060006;
    REGD(0x08005060) = 0x00000500;
#else
	REGD(0x08005050) = 0x00060005;
	REGD(0x08005060) = 0x00000280;
#endif
	REGD(0x08005064) = 0x00EF013F;
	REGD(0x08005008) = 0x00000001;
	/* flt win 2 */
#ifdef CONFIG_BPP32
	REGD(0x08005054) = 0x00003608;
	REGD(0x0800507c) = 0x00000500;
#else
//	REGD(0x08005054) = 0xffff240c;	/* 565 */
	REGD(0x08005054) = 0x00002d08;	/* 1555 */
	REGD(0x0800507c) = 0x00000280;
#endif
	REGD(0x08005078) = FLT2_ADDR;
	REGD(0x08005084) = 0x00ef013f;

	int value;
	int count;
	int total = 0;
	for (i = 0; i < PLOGO_SIZE; i++) {
		value = plogo[i] >> 24;
		count = plogo[i] << 8 >> 8;
		if (total + count <= LOGO_SIZE) {
			for (j = total; j < total + count; j++) {
				logo[j] = value;
			}
			total += count;
		}
	}
#endif	

	j = 0;
	for (i = 0; i < WIN_SIZE; i++)
	{
#ifdef CONFIG_BPP32
		/* 8888 */
#ifdef CONFIG_CPT480X272
		fb32 = 0xff000000 | (logo[j] << 16);
		j++;
		fb32 |= (logo[j] << 8);
		j++;
		fb32 |= logo[j];
		j++;
		*((volatile uint32_t *)FLT2_ADDR + i) = fb32;
#elif CONFIG_TPO800X480			
		for (i = 0; i < (WIN_SIZE/3); i++)
		{
			*((volatile uint32_t *)FLT2_ADDR + i) = 0xFFFF0000;
		}
		for (i = (WIN_SIZE/3); i < (2*WIN_SIZE/3); i++)
		{
			*((volatile uint32_t *)FLT2_ADDR + i) = 0xFF00FF00;
		}
		for (i = (2*WIN_SIZE/3); i < WIN_SIZE; i++)
		{
			*((volatile uint32_t *)FLT2_ADDR + i) = 0xFF0000FF;
		}				
#else
		fb32 = (0xff << 24) | logo[j++] | (logo[j++] << 8) | (logo[j++] << 16);
		*((volatile uint32_t *)FLT2_ADDR + i) = fb32;
#endif
#else
		/* 1555 */
#ifdef CONFIG_CPT480X272
		fb16 = (1 << 15) | ((logo[j] >> 3) << 10);
		j++;
		fb16 |= ((logo[j] >> 3) << 5);
		j++;
		fb16 |= (logo[j] >> 3);
		j++;
		*((volatile uint16_t *)FLT2_ADDR + i) = fb16;
#elif CONFIG_TPO800X480
		for (i = 0; i < (WIN_SIZE/3); i++)
		{
			*((volatile uint16_t *)FLT2_ADDR + i) = 0xFC00;
		}
		for (i = (WIN_SIZE/3); i < (2*WIN_SIZE/3); i++)
		{
			*((volatile uint16_t *)FLT2_ADDR + i) = 0x83E0;
		}
		for (i = (2*WIN_SIZE/3); i < WIN_SIZE; i++)
		{
			*((volatile uint16_t *)FLT2_ADDR + i) = 0x801F;
		}				
#else
		fb16 = (1 << 15) | (logo[j++] >> 3) | (logo[j++] >> 3 << 5) | (logo[j++] >> 3 << 10);
		*((volatile uint16_t *)FLT2_ADDR + i) = fb16;
#endif
#endif
#if 0
		/* 565 */
		fb16 = (logo[j++] >> 3) | (logo[j++] >> 2 << 5) | (logo[j++] >> 3 << 11);
		*((volatile uint16_t *)FLT2_ADDR + i) = fb16;
#endif
	}

	/* load kernel */
	t.r = (void *)0x40000000;
	t.ecc = 1;
	t.cs = 0;

	nfc_init(&t);
	nfc_nand_rst(&t);
	nfc_nand_id(&t, &id);
	nand_geo(&id, &geo);
	nfc_cfg(&t, &geo);

	nfcbuf = (uint32_t *)(((char *)t.r) + 0x1000);
	p = (void *)CFG_LOAD_ADDR;
	pg = BOOT_SIZE >> geo.page;
	pgsz = 1 << geo.page;
	ppb = 1 << (geo.block - geo.page);
	pgmax = 1 << (geo.size + 20 - geo.page);

	/* 
	 * TODO: skip bb but not exceed the KERN_SIZE limitation. 
	 * maybe expand the KERN_SIZE before do it is better choice.
	 * but when do the expansion? just use max for now.
	 */
	/* the first valid block */
	while (block_isbad(&t, pg, pgsz))
	{
		pg += ppb;
		if (pg > pgmax)
		{
			printf("error: bad blocks so many.\n");
			return -1;
		}
		continue;
	}

	burst8(p, (char *)nfcbuf, pgsz);
	if (IH_MAGIC == ntohl(*((uint32_t *)p)))
	{
		image_header_t *hdr = (image_header_t *)p;
		pages = ntohl(hdr->ih_size) + sizeof(image_header_t);
	}
	else
	{
		pages = KERN_SIZE;
	}

	if (pages > KERN_SIZE)
	{
		pages = KERN_SIZE;
	}

	pg++;
	p += pgsz;
	pages >>= geo.page; /* the rest pages */
	pgcnt = pages > ppb ? ppb - 1 : pages - 1;
	while (pgcnt--)
	{
		nfc_nand_read(&t, pg);
		burst8(p, (char *)nfcbuf, pgsz);
		pg++;
		p += pgsz;
	}

	/* the rest blocks */
	blocks = (pages + ppb - 1) >> (geo.block - geo.page);
	pgcnt = 0;
	while (blocks - 1 > 0)
	{
		/* skip bb */
		if (block_isbad(&t, pg, pgsz))
		{
			pg += ppb;
			if (pg > pgmax)
			{
				printf("error: bad blocks so many.\n");
				return -1;
			}
			continue;
		}

		pgcnt = pages - pgcnt > ppb ? ppb : pages - pgcnt;
		for (i = 0; i < pgcnt; i++)
		{
			nfc_nand_read(&t, pg);
			burst8(p, (char *)nfcbuf, pgsz);
			pg++;
			p += pgsz;
		}
		blocks--;
	}

	/* enable power & disable amplifier, accio */
	REGD(0x0810f318) |= 0x00002001;
	REGD(0x0810f320) |= 0x00002001;
	REGD(0x0810f33c) |= 0x00000001;
	REGD(0x0810f33c) &= ~0x00002000;
	
	/* Disable WIFI Power PD5; BlueTooth PD22; DTV Power Enable PD8*/
	REGD(0x0810f318) |=  0x00400120;
	REGD(0x0810f320) |=  0x00400120;
	REGD(0x0810f33c) &= ~0x00400120;
	
	/* Disable GPS Power PE1*/
	REGD(0x0810f418) |=  0x00000002;
	REGD(0x0810f420) |=  0x00000002;
	REGD(0x0810f43c) &= ~0x00000002;
	
	/* decrease Dynamic Backlight Control effect. */
	REGD(0x08005048)=0x0;
	for (i=0;i<256;i++) {
	   REGD(0x08005044) = i*256*256 + i*256 + i;
	}
	
	/* delay 100ms for power stable */
	delay = 100;
	while (delay--)
		udelay(1000);

	/* backlight, pwm */
	REGD(0x0810D008) = 0x00000003;
	REGD(0x0810D010) = 0x00FF0001;
	REGD(0x0810D008) = 0x00000001;
#if 0
	/* backlight, gpio */
	REGD(0x0810f518) |= 0x00000400;
	REGD(0x0810f520) |= 0x00000400;
	REGD(0x0810f53c) |= 0x00000400;
#endif

	return 0;
}


int dram_init (void)
{
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;
#if 0
	sdramc_start();
#endif
	return 0;
}


void lowlevel_init(void) { }

void reset_cpu(unsigned long adr) { }

#include "sdr.h"

int sdramc_start (void)
{
	sdr_cfg_t cfg;

	/* enable lvcmos */
	REGD(0x08100828) = 0x02000000;

	if (sdr_init (SDR_REG_BASE) < 0)
	{
		printf ("sdramc initialisation failed\n");
		return -1;
	}

	if (sdr_reset () < 0)
	{
		printf ("sdramc reset failed\n");
		return -1;
	}

	sdr_enable ();	  // clk running, cke low for 200us
	udelay (200);
	sdr_power_up ();	// clk running, cke high for 200us
	udelay (200);

	// configure cs0
	cfg.cs = 0;
	cfg.dsz = 1;	// 1=32-bit, 0=16-bit
	cfg.row = 13;
	cfg.col = 9;
	cfg.bl = 8;
	cfg.cas = 3;

	if (sdr_start (&cfg) < 0)
	{
		printf ("sdramc csd0 start up failed\n");
		return -1;
	}

	// configure cs1
	cfg.cs = 1;
	if (sdr_start (&cfg) < 0)
	{
		printf ("sdramc csd1 start up failed\n");
		return -1;
	}

	return 0;
}

