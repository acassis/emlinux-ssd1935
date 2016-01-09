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
#include "lowbatt.h"
#include "../../../include/nboot.h"
#include "../../../include/config.h"
#include "../../../include/lcdc.h"

#include "spir.h"
#include "spi.h"
#include "os.h"
#include "ak4182.h"

#ifdef CONFIG_ACCIO_P1_SK01
#include "sk-plogo.h"
#else
#include "plogo.h"
#endif

DECLARE_GLOBAL_DATA_PTR;

/* define float windows 2 address */
#if defined CONFIG_ACCIO_P1_SK01
#define FLT2_ADDR	0x53E80000
#elif defined CONFIG_ACCIO_P1_LITE
#define FLT2_ADDR	0x51F8E000
#endif

#define REGD(a)		(*(volatile uint32_t *)(a))
void burst8(char *dst, char *src, int len);
static void normal_logo(void);
static void lowbatt_logo(void);
static void sk01_lcd_init(void);
static int lcd2119_spi_init(void);

// for bmp logo
#define BMP_MAX_SIZE (DISPLAY_WIDTH*DISPLAY_HEIGHT*PIXEL_BYTE + TAG_SIZE + 70) // TAG_SIZE(BMP tag) + 70(BMP data offset)

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

#ifdef CONFIG_ACCIO_P1_SK01
#define WIN_SIZE	0xB0100		/* 480 * 272 */
#define COL_SIZE	480
#define ROW_SIZE	272
#else
#define WIN_SIZE	0x12c00		/* 320 * 240 */
#endif
uint8_t logo[LOGO_SIZE];

#define GPIO_PC_MODE  			                (SSD1933_GPIO_BASE + 0x0218)
#define GPIO_PC_DIR  			                (SSD1933_GPIO_BASE + 0x0220)
#define GPIO_PC_DREG  			                (SSD1933_GPIO_BASE + 0x023C)
#define GPIO_AMPLIFY_MUTE	                    (1 << 21)
#define GPIO_AMPLIFY_SLEEP	                    (1 << 22)


/******************************************************************************/
/**
 *   Amplify_Init Function
**/
/******************************************************************************/
void Amplify_Init(void)
{
    REGD(GPIO_PC_MODE) |= (GPIO_AMPLIFY_MUTE | GPIO_AMPLIFY_SLEEP);
    REGD(GPIO_PC_DIR) |= (GPIO_AMPLIFY_MUTE | GPIO_AMPLIFY_SLEEP); // output pin
    REGD(GPIO_PC_DREG) &= ~(GPIO_AMPLIFY_MUTE | GPIO_AMPLIFY_SLEEP); // MUTEn and SLEEPn be pulled low
}

#define GPIO_PC_PULLEN  			        (SSD1933_GPIO_BASE + 0x021C)
#define GPIO_PD_PULLEN  			        (SSD1933_GPIO_BASE + 0x031C)
#define GPIO_TILT_PIN0	                    (1 << 15)
#define GPIO_TILT_PIN1	                    (1 << 0)

/******************************************************************************/
/**
 *   Posture_Detect Function
**/
/******************************************************************************/
int Tilt_Detect(void)
{
    // (pin0,pin1) = (0,0)-> 180 degree, (0,1)-> 270 degree, (1,0)-> 90 degree, (1,1)-> 0 degree
    // direction is the clock direction
    REGD(GPIO_PC_MODE) |= GPIO_TILT_PIN0;
    REGD(GPIO_PC_DIR) &= ~GPIO_TILT_PIN0;
    REGD(GPIO_PC_PULLEN) &= ~GPIO_TILT_PIN0;
    REGD(GPIO_PD_MODE) |= GPIO_TILT_PIN1;
    REGD(GPIO_PD_DIR) &= ~GPIO_TILT_PIN1; // input pin
    REGD(GPIO_PD_PULLEN) &= ~GPIO_TILT_PIN1;

    udelay(100000);
    // read tilt
    if ((REGD(GPIO_PC_DREG) & GPIO_TILT_PIN0) == 0)
    {
        if ((REGD(GPIO_PD_DREG) & GPIO_TILT_PIN1) == 0)
            return 2; // 180 degree
        else
            return 3; // 270 degree
    }
    else
    {
        if ((REGD(GPIO_PD_DREG) & GPIO_TILT_PIN1) == 0)
            return 1; // 90 degree
        else
            return 0; // 0 degree
    }
}

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
	int		ret, i, j = 0;
#if 0
#ifdef CONFIG_BPP32
	uint32_t	fb32;
#else
	uint16_t	fb16;
#endif
#endif
	int		ppb, pgcnt, pgmax, blocks;
	int		delay;
    image_header_t *kernel_hdr = NULL;
#if 0
//#if defined CONFIG_ACCIO_P1_LITE
	lcd2119_spi_init();
#endif

    Amplify_Init();

#if defined CONFIG_ACCIO_P1_SK01
	sk01_lcd_init();
#else
#if 0
	/* lcdc configure */
	/* LCD DEN down */
//#ifndef CONFIG_ACCIO_P1_LITE
	REGD(0x0810f118) |= 0x00000008;
	REGD(0x0810f120) |= 0x00000008;
	REGD(0x0810f13c) &= ~0x00000008;
//#endif
	REGD(0x08005008) = 0x00000007;
	REGD(0x08005014) = 0x0000B723;
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
#endif
#endif

	/* detect power*/
	uint32_t bvalue = ak4182_init();
	if (bvalue < 3600)
		lowbatt_logo();
	else
		normal_logo();

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
        kernel_hdr = hdr;
	}
	else
	{
		pages = KERN_SIZE;
	}

	if (pages > KERN_SIZE)
	{
		pages = KERN_SIZE;
	}

    // for BMP image
    pages += BMP_MAX_SIZE;

	pg++;
	p += pgsz;
	pages >>= geo.page; /* the rest pages */
	pgcnt = pages > ppb ? ppb - 1 : pages - 1;
	while (pgcnt--)
	{
		if(ret = nfc_nand_read(&t, pg)>0)
		{
			printf("ecc mode = %d\n",t.eccmode);
			printf("ecc corrected page = %d count = %d\n",pg,ret);
		}
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
			if(ret = nfc_nand_read(&t, pg)>0)
			{
				printf("ecc mode = %d\n",t.eccmode);
				printf("ecc corrected page = %d count = %d\n",pg,ret);
			}
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

	/* decrease Dynamic Backlight Control effect. */
	REGD(0x08005048)=0x0;
	for (i=0;i<256;i++) {
	   REGD(0x08005044) = i*256*256 + i*256 + i;
	}

	/* delay 100ms for power stable */
	delay = 100;
	while (delay--)
		udelay(1000);

#if 0
	/* backlight, pwm */
	REGD(0x0810D008) = 0x00000003;
	REGD(0x0810D010) = 0x00FF0001;
	REGD(0x0810D008) = 0x00000001;
#endif
#if 0
	/* backlight, gpio */
	REGD(0x0810f518) |= 0x00000400;
	REGD(0x0810f520) |= 0x00000400;
	REGD(0x0810f53c) |= 0x00000400;
#endif
#if 0
//#if defined CONFIG_ACCIO_P1_LITE
	// Pull down/Disable Camera PE0
    REGD(0x0810f418) |= 0x00000001;
    REGD(0x0810f420) |= 0x00000001;
    REGD(0x0810f43c) &= ~0x00000001;
#endif

	/* low battery auto power down */
	if (bvalue < 3600) {
		delay = 1500;
		while(delay--)
			udelay(1000);
		REGD(0x0810f33C) &= ~0x1;
	}
	rd_maintain();

    uint32_t   logo_addr = ntohl(kernel_hdr->ih_load) + ntohl(kernel_hdr->ih_size);
    void       *pLogoPtr;
    int        degree;

    Panel_Init();
    Lcdc_Init();

    //Show_BmpLogo(logo_addr);
    pLogoPtr = Check_BmpLogo(logo_addr);
    degree = Tilt_Detect();
    //printf("Anderson : degree = %d\n", degree);
    pLogoPtr += (BITMAP_OFFSET * degree);
#if defined BPP_32
    Show_BmpLogo((uint32_t *)pLogoPtr);
#elif defined BPP_16
    Show_BmpLogo((uint16_t *)pLogoPtr);
#endif

    Backlight_Init();
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

#if defined CONFIG_ACCIO_P1_SK01
static void normal_logo(void)
{
	int value, count, total = 0;
	int i, j = 0, k = 0;
#ifdef CONFIG_BPP32
    uint32_t    fb32;
#else
    uint16_t    fb16;
#endif

	for(i=0;i<ROW_SIZE;i++)
	{
		for(j=0;j<COL_SIZE;j++)
		{
#ifdef CONFIG_BPP32
			fb32 = 0xff000000;
			*((volatile uint32_t *)FLT2_ADDR + i*COL_SIZE+j) = fb32;
#else
			fb16=0x7fff;
			*((volatile uint16_t *)FLT2_ADDR + i*COL_SIZE+j) = fb16;
#endif
		}
	}

	for(i=74;i<198;i++)
	{
		for(j=0;j<COL_SIZE;j++)
		{
			if(j<120||j>=360)
			{
				continue;
			}
			else
			{
#ifdef CONFIG_BPP32
				fb32 =  (sk_plogo[k+2] *256*256)+ (sk_plogo[k+1] *256) + (sk_plogo[k] );
				fb32 |= 0xff000000;
				*((volatile uint32_t *)FLT2_ADDR +  i*COL_SIZE+j) = fb32;
				k+=3;
#else
				fb16 = (1 << 15) | (sk_plogo[k] ) | (sk_plogo[k+1]<<8);
				*((volatile uint16_t *)FLT2_ADDR +  i*COL_SIZE+j) = fb16;
				k+=2;
#endif
			}

		}
	}
}
#else
static void normal_logo(void)
{
	int value, count, total = 0;
	int	i, j = 0;
#ifdef CONFIG_BPP32
	uint32_t	fb32;
#else
	uint16_t	fb16;
#endif
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

	j = 0;
	for (i = 0; i < WIN_SIZE; i++)
	{
#ifdef CONFIG_BPP32
		fb32 = (0xff << 24) | logo[j++] | (logo[j++] << 8) | (logo[j++] << 16);
		*((volatile uint32_t *)FLT2_ADDR + i) = fb32;
#else
		/* 1555 */
		fb16 = (1 << 15) | (logo[j++] >> 3) | (logo[j++] >> 3 << 5) | (logo[j++] >> 3 << 10);
		*((volatile uint16_t *)FLT2_ADDR + i) = fb16;
#endif
	}
}
#endif

static void lowbatt_logo(void)
{
	int i;
#ifdef CONFIG_BPP32
	uint32_t	fb32, *fb, n = 4;
#else
	uint16_t        fb16, *fb, n = 2;
#endif
        fb = (void *)FLT2_ADDR;
        memset(fb, 0, WIN_SIZE * n);
        fb = (uint8_t *)FLT2_ADDR + (320 * 101 - 100) * n;
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
				fb += (320 - BATT_WIDTH);
        }
}

static void sk01_lcd_init(void)
{
	/* lcdc configure */
	/* LCD DEN down */
	REGD(0x0810f118) |= 0x00000008;
	REGD(0x0810f120) |= 0x00000008;
	REGD(0x0810f13c) &= ~0x00000008;

	REGD(0x0810f318)|= 0x00000204;
	REGD(0x0810f320)|= 0x00000204;
	REGD(0x0810f33c)|= 0x00000204;

	REGD(0x08005008) = 0x00000001;
	REGD(0x08005014) = 0x0001013E;
	REGD(0x08005018) = 0x00000071;
	REGD(0x08005020) = 0x020C002F;//for qimei
	//REGD(0x08005020) = 0x020C0012;//for lp
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
	REGD(0x08005078) = FLT2_ADDR;
	REGD(0x08005084) = 0x010F01DF;
}

static int lcd2119_spi_init(void)
{
	spi_t		s;
	spi_cfg_t	c;
	spir_p		r;
	int i;

	r = (spir_p)0x08107000;
	io_wr32(r->ctl, 0x1);
	io_wr32(r->ctl, io_rd32(r->ctl) | 0x3);

	for (i = 0; i < SPI_TOUT; i++)
	{
		if ((io_rd32(r->ctl) & 0x3) == 0x1)
		{
			break;
		}
	}
	/* configure spi register */
	io_wr32(r->op, 0x80000268);
 	io_wr32(r->master, 0x10B);
	io_wr32(r->ier,0x0);
	io_wr32(r->fifo,0x3);

	/* transfer spi command to initial LCD */
	io_wr32(r->tx, 0x28);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x100);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x106);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x0);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x100);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x101);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x01);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x172);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x1EF);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x02);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x106);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x100);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x03);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x160);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x164);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x10);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x100);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x100);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x11);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x14E);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x170);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x07);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x100);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x133);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x25);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x180);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x100);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x0B);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x153);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x108);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x0C);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x100);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x105);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x0D);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x100);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x10D);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x0E);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x128);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x100);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x1E);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x100);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x1B6);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x30);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x100);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x100);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x31);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x104);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x100);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x32);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x100);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x107);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x33);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x107);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x100);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x34);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x100);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x107);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x35);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x107);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x100);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x36);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x107);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x107);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x37);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x100);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x104);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x3A);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x11A);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x100);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x3B);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x100);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x108);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x0F);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x100);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x100);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x15);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x100);
	spi_tx_wait(r);
	io_wr32(r->tx, 0x150);
	spi_tx_wait(r);
}
