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
#include "plogo.h"
#include "lowbatt.h"
#include "../../../include/nboot.h"
#include "spir.h"
#include "spi.h"
#include "os.h"
#include "../../../include/config.h"

#undef FLT2_ADDR
#define FLT2_ADDR 0x53E80000

DECLARE_GLOBAL_DATA_PTR;

void burst8(char *dst, char *src, int len);
static void normal_logo(void);
static void lowbatt_logo(void);

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

#define WIN_SIZE	0xB0100		/* 480 * 272 */
//uint8_t logo[LOGO_SIZE];

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
#if 0
#ifdef CONFIG_BPP32
	uint32_t	fb32;
#else
	uint16_t	fb16;
#endif
#endif
	int		ppb, pgcnt, pgmax, blocks;
	int		delay;

	/* lcdc configure */
	REGD(0x08005008) = 0x00000001;
	REGD(0x08005014) = 0x0001013E;
	REGD(0x08005018) = 0x00000061;
	REGD(0x08005020) = 0x020C002F;
	REGD(0x08005024) = 0x011D000A;
	REGD(0x08005028) = 0x28000002;
	REGD(0x0800502c) = 0x06000000;
	REGD(0x08005058) = 0x00000000;
	REGD(0x0800505C) = 0x53F80000;
#ifdef CONFIG_BPP32
	REGD(0x08005050) = 0x0006024E;	/* 32bpp */
	REGD(0x08005060) = 0x00000780;	/* 32bpp */
#else
	REGD(0x08005050) = 0x0006024D;	/* 16bpp */
	REGD(0x08005060) = 0x000003C0;	/* 16bpp */
#endif
	REGD(0x08005064) = 0x010F01DF;
	REGD(0x08005008) = 0x00000001;

	/* flt win 2 */
#ifdef CONFIG_BPP32
	REGD(0x08005054) = 0x00003608;	/* 32bpp */
	REGD(0x0800507c) = 0x00000780;	/* 32bpp */
#else
//	REGD(0x08005054) = 0xffff240c;	/* 16bpp 565 */
	REGD(0x08005054) = 0x00002d08;	/* 16bpp 1555 */
	REGD(0x0800507c) = 0x000003C0;	/* 16bpp */
#endif
	REGD(0x08005078) = FLT2_ADDR;	// 0x53E80000;
	REGD(0x08005084) = 0x010F01DF;

	/* low battery detect */
        REGD(0x0810f31C) = 0xFFFFB7FF;
        REGD(0x0810f318) = 0x4800;
        delay = 500;
	while (delay--)
		udelay(1000);
	uint32_t lb_value = io_rd32(*(volatile uint32_t *)(0x0810f33C));
	uint32_t dc_charging = (lb_value & (1<<11))>>11;
        lb_value = (lb_value & (1<<14))>>14;	

	if(!lb_value)
		lowbatt_logo();
	else
		normal_logo();

#if 0
	j = 0;
	for (i = 0; i < WIN_SIZE; i++)
	{
#if 1 /* 1555 */
//		fb16 = (1 << 15) | (logo[j++] >> 3) | (logo[j++] >> 3 << 5) | (logo[j++] >> 3 << 10);
		fb16 = (1 << 15) | (logo[j] >> 3) | (logo[j + 1] >> 3 << 5) | (logo[j + 2] >> 3 << 10);
		j += 3;
		*((volatile uint16_t *)0x53E80000 + i) = fb16;
#endif
#if 0
//		fb32 = (0xff << 24) | logo[j++] | (logo[j++] << 8) | (logo[j++] << 16);
		fb32 = (0xff << 24) | logo[j] | (logo[j + 1] << 8) | (logo[j + 2] << 16);
		j += 3;
		*((volatile uint32_t *)0x53E80000 + i) = fb32;
#endif
	}
#endif
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

	/* enable power */
#if 0
	REGD(0x0810f218) |= 0x800;
	REGD(0x0810f220) |= 0x800;
	REGD(0x0810f23c) &= ~0x800;
#endif
	REGD(0x0810f418) |= 0x10;
	REGD(0x0810f420) |= 0x10;
	REGD(0x0810f43c) &= ~0x10;

	/* decrease Dynamic Backlight Control effect. */
	REGD(0x08005048)=0x0;
	for (i=0;i<256;i++) {
	   REGD(0x08005044) = i*256*256 + i*256 + i;
	}

	/* Enable DBC. */
	REGD(0x08005120)=0x1680;
	REGD(0x08005118)=0x6f;
	REGD(0x0810D008) = 0x00000003;
	REGD(0x0810D010) = 0x0300000b;
	

	/* delay 100ms for power stable */
	delay = 100;
	while (delay--)
		udelay(1000);

	/* disable amplifier */
	REGD(0x0810f318) |= 0x8;
	REGD(0x0810f320) |= 0x8;
	REGD(0x0810f33c) &= ~0x8;

	/* system power LED */
	REGD(0x0810f218) |= 0x4000000;
	REGD(0x0810f220) |= 0x4000000;
	REGD(0x0810f23c) &= ~0x4000000;

	/* backlight, pwm */
	REGD(0x0810D008) = 0x00000003;
	REGD(0x0810D010) = 0x03FF0003;
	REGD(0x0810D008) = 0x00000001;

	/*Disable DBC*/
	//REGD(0x08005118)=0x50;
	
	/* enable pwm2 */
	REGD(0x0810f518) |= 0x00002000;
	REGD(0x0810f520) |= 0x00002000;
	REGD(0x0810f53C) |= 0x00002000;
	
        /* enable codec mclk */
        REGD(0x0810f318) |= 0x2000;
        REGD(0x0810f320) |= 0x2000;
        REGD(0x0810f33c) |= 0x2000;

	if(!lb_value) {
		while(!dc_charging) {
			int timer = 10;
			while(--timer) {
				dc_charging = io_rd32(*(volatile uint32_t*)(0x0810f33C));
				dc_charging = (dc_charging&(1<<11))>>11;
				if(dc_charging) {
					REGD(0x0810f43C) &= ~0x10;
					normal_logo();
					break;
				} else {
					delay = 2000;	//delay 3s
					while(delay--)
						udelay(1000);
				}
			}
			if(!dc_charging)
				REGD(0x0810f43c) |= 0x10;
		}
	}

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


static void normal_logo(void)
{
        int count,loop,i;
#ifdef CONFIG_BPP32
	uint32_t	fb32, *fb;
#else
	uint16_t        fb16, *fb;
#endif
	fb = (void *)FLT2_ADDR;
        for (i = 0; i < PLOGO_SIZE; i += 4) {
                count = plogo[i + 3];
                for (loop = 0; loop < count; loop++) {
#ifdef CONFIG_BPP32
			fb32 = (0xff << 24) | plogo[i] | (plogo[i + 1] << 8) | (plogo[i + 2] << 16);
			*(fb++) = fb32;
#else
                        fb16 = (1 << 15) | (plogo[i] >> 3) | (plogo[i + 1] >> 3 << 5) | (plogo[i + 2] >> 3 << 10);
                        *(fb++) = fb16;
#endif
                }
        }
	
}

static void lowbatt_logo(void)
{
	int i;
#ifdef CONFIG_BPP32
	uint32_t        fb32, *fb, n = 4;
#else
	uint16_t        fb16, *fb, n = 2;
#endif
		fb = (void *)FLT2_ADDR;
        memset(fb, 0, WIN_SIZE * n);
        fb = (uint8_t *)FLT2_ADDR + (480 * 136 - 180) * n;
        for (i = 0; i < BATT_SIZE; i += 3)
        {
#ifdef CONFIG_BPP32
		fb32 = (0xff << 24) | lowbatt[i] | (lowbatt[i + 1] << 8) | (lowbatt[i + 2] << 16);
		*(fb++) = fb32;
#else
                fb16 = (1 << 15) | (lowbatt[i] >> 3) | (lowbatt[i + 1] >> 3 << 5) | (lowbatt[i + 2] >> 3 << 10);
                *(fb++) = fb16;
#endif
                if (i % (3 * BATT_WIDTH) == 0)
                        fb += (480 - BATT_WIDTH);
        }

}

