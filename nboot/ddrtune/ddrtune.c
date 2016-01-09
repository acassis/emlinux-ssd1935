#include <stdint.h>
#include "mytypes.h"
#include "platform.h"
#include "str.h"
#include "../include/nboot.h"

#include "sdrtype.h"
#include "dmacapi.h"
#include "dma.h"
#include "gpt.h"

#include "nand.h"
#include "nfc.h" 

#define MAX_DDR_TUNE_LOOP	(0x100>>2)
#define KB			0x400
#define SRAM_BASE		(MEMC_BASE + 0x8000)
#define FULL_LANE		4
#define ECC_ON 

#define PATTERN_TOGGLE		0
#define PATTERN_RANDOM		1

int	_g_type = 0;

int sdramc_ddr_init (void);
ulong ddr_tune_lane (ulong lane, ulong *dqs);
ulong ddr_tune_word (ulong *dqs);

void fill_toggle (ulong addr, ulong len, int type)
{
	ulong 	*p = (ulong *)addr;
	ulong 	i;
	ulong	d = 0;

	if (type == PATTERN_TOGGLE)
	{
		for (i = 0; i < (len >> 2); i++)
			p[i] = (i & 1UL) ? 0xFFFFFFFF : 0;
	}
	else
	{
		for (i = 0; i < (len >> 2); i++)
		{
			p[i] = d;
			d += 0x11111111;
		}
	}
	return;
}

int check_fill_toggle (ulong addr, ulong len, int type)
{
	ulong 	*p = (ulong *)addr;
	ulong 	i;
	ulong	d = 0;

	if (type == PATTERN_TOGGLE)
	{
		for (i = 0; i < (len >> 2); i++)
			if (p[i] != ((i & 1UL) ? 0xFFFFFFFF : 0))
				return -1;
	}
	else
	{
		for (i = 0; i < (len >> 2); i++, d+= 0x11111111)
			if (p[i] != d)
				return -1;
	}
	
	return 0;
}

int check_fill_toggle_lane (ulong addr, ulong len, ulong lane, int type)
{
	ulong 	*p = (ulong *)addr;
	ulong 	i;
	ulong	d = 0;

	if (type == PATTERN_TOGGLE)
	{
		for (i = 0; i < (len >> 2); i++)
			if ((p[i] & (0xFFUL << 8*lane)) != (((i & 1UL) ? 0xFFFFFFFF : 0) & (0xFFUL << 8*lane)))
			return -1;
	} 
	else
	{
		for (i = 0; i < (len >> 2); i++, d += 0x11111111)
			if ((p[i] & (0xFFUL << 8*lane)) != (d & (0xFFUL << 8*lane)))
			return -1;
	}
	
	return 0;
}

int test_transfer (unsigned long lane)
{
	ulong	len = 1*KB;

	dma_test_1_ex_bt (SDR_CSD0_BASE, SRAM_BASE, &len, 0, DMAC_BT_INCR16U);

	if (lane >= FULL_LANE)
	{
		if (check_fill_toggle (SRAM_BASE, len, _g_type) < 0)
		return -1;
	}
	else 
	{
		if (check_fill_toggle_lane (SRAM_BASE, len, lane, _g_type) < 0)
			return -1;
	}

	return 0;
}

void main (void)
{
	sdr_t *sdr = (sdr_t *) SDR_BASE;
	ulong	dqs = 0;
	ulong	reading = 0;
	ulong	dqs_lane = 0;
	int i;
	char    s[80], p[80];

	_g_type = PATTERN_RANDOM;
	fill_toggle (SDR_CSD0_BASE, 1*KB, _g_type);
	if (check_fill_toggle (SDR_CSD0_BASE, 1*KB, _g_type) < 0)
	{
		puts ("arm<->ddram accesses failed\n");
		goto end;
	}

#ifdef DEBUG
	puts ("arm<->ddram accesses passed\n");
#endif

	gpt_init (GPT2_BASE, 60);
	gpt_reset ();

	if (_g_type == PATTERN_RANDOM)
		puts ("Word Level Tuning (Random)\n");
	gpt_start ();
	if (ddr_tune_word (&dqs) == 0)
	{
		sdr->dly_dat = dqs;
	}
	gpt_stop (&reading);

	strcpy (s, "dqs = "); strcat (s, long2str (p, sdr->dly_dat)); strcat (s, "\n");
	puts (s);
//	strcpy (s, "reading (us) = "); strcat (s, long2str (p, reading)); strcat (s, "\n");
//	puts (s);

end:
{
	nfc_t			t;
	nand_id_t		id;
	nand_geo_t		geo;
	void			(*spl)(void);
	char			*p;
	int			pgsz, pg, npages;
	struct nboot_desc	*nboot;
#ifdef ECC_ON
#warning "ecc on"
	t.ecc = 1;
#else
#warning "ecc off"
	t.ecc = 0;
#endif
	t.r = (void *)0x40000000;
	nfc_init(&t);
	nfc_nand_rst(&t);
	nfc_nand_id(&t, &id);
	nand_geo(&id, &geo);
	nfc_cfg(&t, &geo);

	pgsz = 1 << geo.page;
	pg = (IPL_LEN+TUNE_LEN) >> geo.page;

	nboot = (struct nboot_desc *)(LDR_UBOOT_BASE + TUNE_LEN - sizeof(struct nboot_desc));

	if (nboot->magic == NBOOT_MAGIC && nboot->len < NBOOT_MAX_LEN)
	{
		npages = (nboot->len >> geo.page) + 1;
	}
	else
	{
		npages = NBOOT_MAX_LEN >> geo.page;
	}

	spl = (void *)0x52F40000;
	p = (void *)spl;
	while (npages--)
	{
		nfc_nand_read(&t, pg);
		burst8(p, (char *)0x40001000, pgsz);
		pg++;
		p += pgsz;
	}
	p = (void*)spl;
	spl();
}
	return 0;
}

ulong ddr_tune_word (ulong *dqs)
{
	sdr_t *sdr = (sdr_t *) SDR_BASE;
	ulong	first_dqs = 0;
	ulong	last_dqs = 0;
	int	pass = 0;
	int	i;
	char    s[80], p[80];
	
	if (!dqs)
		return -1;

	// backup
	*dqs = sdr->dly_dat;

#ifdef  WORD_LOOP
do {
#endif
	sdr->ctl &= ~(SDR_CTL_TAP | SDR_CTL_STUNEN);
	sdr->dly_dat = 0;

	for (i = 0, pass = 0; i < MAX_DDR_TUNE_LOOP; i++, sdr->dly_dat += 0x04040404)
	{
		if (!pass)
		{
			if (test_transfer (FULL_LANE) < 0)
				continue;
			else 
			{
				if (test_transfer (FULL_LANE) == 0 && test_transfer (FULL_LANE) == 0)
				{
					pass = 1;
					first_dqs = sdr->dly_dat & 0xFFUL;
				}
			}
		}
		else
		{
			if (test_transfer (FULL_LANE) == 0 && test_transfer (FULL_LANE) == 0 && 
				test_transfer (FULL_LANE) == 0)
				continue;
			else 
			{
				pass = 0;
				last_dqs = (sdr->dly_dat & 0xFFUL) - 0x04;
				break;
			}
		}
	}

	if (i == MAX_DDR_TUNE_LOOP)
	{
		if (pass)
		{
			last_dqs = 0xFCUL;
			puts ("dqs tuning (i==MAX_DDR_TUNE_LOOP)\n");
		}
		else
		{
			first_dqs = last_dqs = 0;
			puts ("dqs tuning failed(+259)\n");
			sdr->dly_dat = *dqs;
			return -1;
		}
	}
#ifdef WORD_LOOP
} while (1);
#endif

#if 1 //Debug dqs
	strcpy (s, "first dqs="); strcat (s, long2str (p, first_dqs)); strcat (s, "\n");
	puts (s);

	strcpy (s, "last dqs="); strcat (s, long2str (p, last_dqs)); strcat (s, "\n");
	puts (s);
	
	strcpy (s, "restore dqs="); strcat (s, long2str (p, *dqs)); strcat (s, "\n");
	puts (s);

#endif


	first_dqs = (first_dqs+last_dqs) >> 1;
	// restore dqs
	sdr->dly_dat = *dqs;
	*dqs = (first_dqs << 24) | (first_dqs << 16) | (first_dqs << 8) | first_dqs;

	return 0;
}

ulong ddr_tune_lane (ulong lane, ulong *dqs)
{
	sdr_t *sdr = (sdr_t *) SDR_BASE;
	ulong	first_dqs = 0;
	ulong	last_dqs = 0;
	int	pass = 0;
	int	i;

	*dqs = sdr->dly_dat;

	sdr->dly_dat = 0;
	for (i = 0, pass = 0; i < MAX_DDR_TUNE_LOOP; i++, sdr->dly_dat += 0x04040404)
	{
		if (!pass)
		{
			if (test_transfer (lane) < 0)
				continue;
			else 
			{
				if (test_transfer (lane) == 0 && test_transfer (lane) == 0)
				{
					pass = 1;
					first_dqs = (sdr->dly_dat >> 8*lane) & 0xFFUL;
				}
			}
		}
		else
		{
			if (test_transfer (lane) == 0 && test_transfer (lane) == 0 && test_transfer (lane) == 0)
				continue;
			else 
			{
				pass = 0;
				last_dqs = ((sdr->dly_dat >> 8*lane) & 0xFFUL) - 0x04;
				break;
			}
		}
	}

	if (i == MAX_DDR_TUNE_LOOP)
	{
		if (pass)
			last_dqs = 0xFCUL;
		else
		{
			first_dqs = last_dqs = 0;
			puts ("dqs tuning failed(+322)\n");
			sdr->dly_dat = *dqs;
			return -1;
		}
	}

	sdr->dly_dat = *dqs;
	*dqs = (first_dqs+last_dqs) >> 1;

#ifdef DEBUG
	strcpy (s, "first dqs = "); strcat (s, long2str (p, first_dqs)); strcat (s, "\n");
	puts (s);

	strcpy (s, "last dqs = "); strcat (s, long2str (p, last_dqs)); strcat (s, "\n");
	puts (s);

	strcpy (s, "dqs = "); strcat (s, long2str (p, *dqs)); strcat (s, "\n");
	puts (s);
#endif

	return 0;
}

#if 0
#define REGD(x)		tmr_sleep (1);*((ulong *)(x))

int sdramc_ddr_init(void)
{
	REGD(0x810000C) = 0x0110;
	REGD(0x800100C) = 0xff;
	REGD(0x8001010) = 0x1d0020;
	REGD(0x8001020) = 0x1d0021;
	REGD(0x8001030) = 0x1d0021;
	REGD(0x8001040) = 0x1d0021;
	REGD(0x8001050) = 0x7d0020;
	REGD(0x8001054) = 0x111122;
	REGD(0x08100828) = 0x0a000000;
	REGD(0x08000008) = 0x00000003;
	REGD(0x0800000c) = 0x793877ff;
	REGD(0x08000010) = 0x793877fe;
	REGD(0x08000008) = 0x00000041;
	REGD(0x08000008) = 0x00000049;
	REGD(0x08000008) = 0x10000049;
	REGD(0x50000000) = 0x0;
	REGD(0x50000000) = 0x0;
	//REGD(0x90000000) = 0x0;
	//REGD(0x90000000) = 0x0;
	REGD(0x08000008) = 0x20000049;
	REGD(0x50000000) = 0x0;
	REGD(0x50000000) = 0x0;
	REGD(0x50000000) = 0x0;
	REGD(0x50000000) = 0x0;
	REGD(0x50000000) = 0x0;
	REGD(0x50000000) = 0x0;
	REGD(0x50000000) = 0x0;
	REGD(0x50000000) = 0x0;
	//REGD(0x90000000) = 0x0;
	//REGD(0x90000000) = 0x0;
	//REGD(0x90000000) = 0x0;
	//REGD(0x90000000) = 0x0;
	//REGD(0x90000000) = 0x0;
	//REGD(0x90000000) = 0x0;
	//REGD(0x90000000) = 0x0;
	//REGD(0x90000000) = 0x0;
	REGD(0x08000018) = 0x00000031;
	REGD(0x08000008) = 0x30000049;
	REGD(0x50000000) = 0x0;
	//REGD(0x90000000) = 0x0;
	REGD(0x08000018) = 0x00020000;
	REGD(0x08000008) = 0x30000049;
	REGD(0x50000000) = 0x0;
	//REGD(0x90000000) = 0x0;
	REGD(0x08000008) = 0x20000049;
	REGD(0x50000000) = 0x0;
	REGD(0x50000000) = 0x0;
	//REGD(0x90000000) = 0x0;
	//REGD(0x90000000) = 0x0;
	REGD(0x08000024) = 0x1f200001;
	REGD(0x08000008) = 0x00000099;
	REGD(0x08000020) = 0x48484848;
	REGD(0x08000008) = 0x0000009d;

	return 0;
}
#endif

/* end */
